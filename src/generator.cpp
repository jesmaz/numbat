#include "../include/generator.hpp"

namespace numbat {
using namespace lexer;
using std::shared_ptr;

namespace parser {

using namespace llvm;
using visitor::Visitor;

AllocaInst * createEntryBlockAlloc (Function * func, Type * type, const std::string & name, Value * arraySize=nullptr) {
	//std::cerr << "Generating entry block alloc" << std::endl;
	IRBuilder <> tmp (&(func->getEntryBlock ()), func->getEntryBlock ().begin ());
	return tmp.CreateAlloca (type, arraySize, name.c_str ());
}

DIFile BodyGenerator::getDIFile (const ParsingContext * context) {
	auto itt = diFiles.find (context);
	if (itt != diFiles.end ()) {
		return itt->second;
	}
	return diFiles [context] = diBuilder.createFile (context->file, context->path);
}

Type * BodyGenerator::getType (const ASTnode & node) {
	
	Type * t = getType (node->getType ());
	if (node->isAlias ())
		t = t->getPointerTo ();
	return t;
	
}

Type * BodyGenerator::getType (const NumbatType * type) {
	
	Type * s = structures [type];
	if (s) return s;
	
	if (!type) {
		s = Type::getVoidTy (context);
	} else {
		const NumbatRawType * rawType = dynamic_cast <const NumbatRawType *> (type);
		const NumbatPointerType * pointerType = dynamic_cast <const NumbatPointerType *> (type);
		if (rawType) {
			switch (rawType->getRawType ()) {
				case NumbatRawType::FLOAT:
					switch (rawType->getBitSize ()) {
						case 16:
							s = Type::getHalfTy (context);
							break;
						case 32:
							s = Type::getFloatTy (context);
							break;
						case 64:
							s = Type::getDoubleTy (context);
							break;
						case 80:
							s = Type::getX86_FP80Ty (context);
							break;
						case 128:
							s = Type::getFP128Ty (context);
							break;
						default:
							s = Type::getVoidTy (context);
							break;
					}
					break;
				default:
					s = Type::getIntNTy (context, type->getBitSize ());
			}
		} else if (pointerType) {
			s = getType (pointerType->getDataType ())->getPointerTo ();
		} else if (type->getMembers ().size ()) {
			std::vector <Type *> members;
			for (const ASTnode & node : type->getMembers ()) {
				members.push_back (getType (node));
			}
			s = StructType::get (context, members);
		} else {
			s = Type::getVoidTy (context);
		}
		
	}
	
	return structures [type] = s;
	
}

Value * BodyGenerator::allocteArray (Value * length, const NumbatPointerType * type) {
	
	size_t mdsize=0;
	for (auto & t : type->getMembers ()) {
		mdsize += dataLayout->getTypeAllocSize (getType (t));
	}
	
	size_t esize = dataLayout->getTypeAllocSize (getType (type->getDataType ()));
	
	Value * mdbytes = builder.getInt64 (mdsize);
	Value * bytes;
	bytes = builder.CreateMul (length, builder.getInt64 (esize));
	bytes = builder.CreateAdd (bytes, mdbytes);
	
	Value * call = builder.CreateCall (getFunction (type->getDataType ()->getType ()->getMalloc ()), std::vector <Value *> (1, bytes));
	
	Value * addressInt = builder.CreateAdd (builder.CreatePtrToInt (call, Type::getInt64Ty (context)), mdbytes);
	
	Type * ptrt = getType (type);
	
	Value * array = builder.CreateIntToPtr (addressInt, ptrt);
	
	int lenDex = type->findMember ("length");
	if (0 <= lenDex) {
		Value * lenPtr = pointerTypeGEP (array, type, type->findMember ("length"));
		builder.CreateStore (length, lenPtr);
	}
	
	return createTemp (array);
	
}

Value * BodyGenerator::createTemp (std::vector <Value *> vals, bool alloc) {
	std::vector <Type *> types;
	std::vector <Value *> temps;
	for (Value * val : vals) {
		Value * tmp = val;
		if (alloc) tmp = createTemp (val);
		types.push_back (tmp->getType ());
		temps.push_back (tmp);
	}
	Type * type = StructType::get (context, types);
	Value * val = GlobalValue::getNullValue (type);
	size_t index = 0;
	for (Value * v : temps) {
		val = builder.CreateInsertValue (val, v, index++);
	}
	return val;
}

Value * BodyGenerator::createTemp (Value * val) {
	
	Value * alloc = createEntryBlockAlloc (activeFunction, val->getType (), "temp");
	builder.CreateStore (val, alloc);
	val = alloc;
	return val;
	
}

Value * BodyGenerator::getValue (ASTbase * node,  bool ref) {
	
	DebugLoc oldLoc = builder.getCurrentDebugLocation ();
	if (node->getLineNo () and debugContext.line != node->getLineNo ()) {
		debugContext.line = node->getLineNo ();
		builder.SetCurrentDebugLocation (DebugLoc::get (debugContext.line, 0, debugContext.scope));
	}
	auto itt = values.find (node);
	Value * val = nullptr;
	if (itt != values.end ()) {
		val = itt->second;
	} else {
		node->accept (*this);
		if (!stack.empty ()) {
			val = stack.top ();
			stack.pop ();
		}
	}
	if (!ref and node->isAlias () and typeid (*node) == typeid (ASTvariable)) {
		val = builder.CreateLoad (val);
	}
	builder.SetCurrentDebugLocation (oldLoc);
	return val;
	
}

Value * BodyGenerator::getVariableHandle (const NumbatVariable * var) {
	
	Value * hand = namedValues [var];
	
	if (!hand) {
		
		//TODO: handle references to const objects
		
		
		
		Type * type = getType (var->getType());
		if (var->isAlias ()) type = type->getPointerTo ();
		//TODO: apply type modifiers
		
		BasicBlock * block = builder.GetInsertBlock ();
		Function * func = activeFunction;
		if (var->isGlobal()) {
			activeFunction = main;
			builder.SetInsertPoint (&main->getEntryBlock ());
			hand = new GlobalVariable (*module, type, var->isConst (), GlobalValue::ExternalLinkage, GlobalValue::getNullValue (type), var->getIden ());
		} else {
			hand = createEntryBlockAlloc (activeFunction, type, var->getIden ());
		}
		namedValues [var] = hand;
		
		if (const ASTnode & init = var->getInit ()) {
			if (var->getType ()->isArray ()) {
				const NumbatPointerType * pType = static_cast <const NumbatPointerType *> (var->getType ());
				Value * val = getValue (init);
				if (val->getType ()->isStructTy ()) {
					Value * arrayPtr = builder.CreateLoad (builder.CreateExtractValue (val, {0}));
					for (size_t i=0; i<pType->getMembers ().size (); ++i) {
						Value * dest = pointerTypeGEP (arrayPtr, pType, i), * source = builder.CreateLoad (builder.CreateExtractValue (val, {i+1}));
						builder.CreateStore (source, dest);
					}
					if (var->isAlias ()) {
						builder.CreateStore (builder.CreateExtractValue (val, {0}), hand);
					} else {
						builder.CreateStore (arrayPtr, hand);
					}
				} else {
					if (var->isAlias ()) {
						builder.CreateStore (val, hand);
					} else {
						builder.CreateStore (builder.CreateLoad (val), hand);
					}
				}
			} else {
				builder.CreateStore (builder.CreateLoad (getValue (init)), hand);
			}
		} else if (var->isAlias ()) {
			builder.CreateStore (GlobalValue::getNullValue (type), hand);
		} else {
			builder.CreateStore (initialise (var->getType ()), hand);
		}
		activeFunction = func;
		builder.SetInsertPoint (block);
		
	}
	
	return hand;
	
}

Value * BodyGenerator::initialise (const NumbatType * ntype) {
	
	Type * ltype = getType (ntype);
	Value * val = GlobalValue::getNullValue (ltype);
	
	if (ltype->isPointerTy ()) {
		if (ntype->isArray ()) {
			static Value * zeroArray = new GlobalVariable (*module, Type::getInt64Ty (context), true, GlobalValue::ExternalLinkage, builder.getInt64 (0), "default array");
			Value * pint = builder.CreatePtrToInt (zeroArray, Type::getInt64Ty (context));
			Value * cpos = builder.CreateAdd (pint, builder.getInt64 (dataLayout->getPointerSize ()));
			val = builder.CreateIntToPtr (cpos, ltype);
		} else {
			//Maybe point to something useful?
		}
	} else if (ltype->isStructTy ()) {
		std::vector <uint32_t> index (1, 0);
		for (ASTnode arg : ntype->getMembers ()) {
			val = builder.CreateInsertValue (val, initialise (arg->getType ()), index);
			++(index [0]);
		}
	}
	return val;
	
}

Value * BodyGenerator::makeCompare (Value * val) {
	
	if (val->getType ()->isPointerTy ()) {
		
		Value * ptrint = builder.CreatePtrToInt (val, Type::getInt64Ty (context));
		val = builder.CreateICmpNE (ptrint, builder.getInt64 (0));
		
	} else {
		if (val->getType()->isStructTy ()) {
			val = builder.CreateExtractValue (val, 0);
			val = makeCompare (val);
		} else if (val->getType ()->isFloatingPointTy ()) {
			val = builder.CreateFCmpONE (val, ConstantFP::get (val->getType (), 0.0));
		} else if (val->getType ()->isIntegerTy ()) {
			if (val->getType ()->getIntegerBitWidth () != 1) {
				val = builder.CreateICmpNE (val, builder.getInt (APInt (val->getType ()->getIntegerBitWidth (), 0)));
			}
		}
	}
	return val;
	
}

Value * BodyGenerator::pointerTypeGEP (Value * val, const NumbatPointerType * ptrType, size_t index) {
	
	size_t offset=0;
	auto & members = ptrType->getMembers ();
	for (size_t i=index, l=members.size (); i<l; ++i) {
		offset += dataLayout->getTypeAllocSize (getType (members [index]));
	}
	Value * oset = ConstantInt::get (Type::getInt64Ty (context), APInt (64, offset));
	Value * iptr = builder.CreatePtrToInt (val, Type::getInt64Ty (context));
	Type * type = getType (members [index]);
	Value * dataiptr = builder.CreateSub (iptr, oset);
	return builder.CreateIntToPtr (dataiptr, type->getPointerTo ());
	
}

void BodyGenerator::createMemCpy (Value * dest, Value * source, Value * length, const shared_ptr <ASTcallable> & conv) {
	
	if (!conv) {
		if (length) {
			size_t size = dataLayout->getTypeAllocSize (dest->getType()->getPointerElementType ()->getPointerElementType ());
			builder.CreateMemCpy (builder.CreateLoad (dest), builder.CreateLoad (source), builder.CreateMul (length, builder.getInt64 (size)), false);
		} else {
			builder.CreateStore (builder.CreateLoad (source), dest);
		}
	} else {
		//TODO: complex data types
	}
	
}

Value * BodyGenerator::makeCompare (const ASTnode & exp) {
	Value * v = getValue (exp);
	if (v->getType ()->isStructTy () and v->getType ()->getStructNumElements () == 1) {
		v = builder.CreateExtractValue (v, 0);
	}
	if (v->getType ()->isPointerTy ()) {
		v = builder.CreateLoad (v);
	}
	if (v->getType ()->isIntegerTy ()) {
		if (v->getType()->getIntegerBitWidth () != 1) {
			if (v->getType ()->isFloatingPointTy ()) {
				v = builder.CreateFCmpONE (v, ConstantFP::get (v->getType (), 0.0));
			} else {
				v = builder.CreateICmpNE (v, ConstantInt::get (v->getType (), APInt (v->getType ()->getIntegerBitWidth (), 0))); // compare with zero
			}
		}
	} else {
		std::cerr << "Invalid type" << std::endl;
	}
	return v;
}

Function * BodyGenerator::getFunction (const FunctionDecleration * func) {
	
	Function * f = functions [func];
	if (f) return f;
	
	std::vector <Type *> args;
	std::string name = func->getIden ();
	
	for (const ASTnode & node : func->getArgs ()) {
		args.push_back (getType (node));
	}
	
	Type * retType;
	
	std::vector <Type *> funcType;
	for (const ASTnode & node : func->getType ()) {
		funcType.push_back (getType (node));
	}
	
	if (func->hasTag ("cstyle")) {
		
		if (funcType.size () == 1) {
			retType = funcType.front ();
		} else {
			retType = Type::getVoidTy (context);
		}
		
	} else {
		
		name = "nbt." + name;
		
		if (funcType.size () == 1 and funcType.front ()->isVoidTy ()) {
			retType = funcType.front ();
		} else if (funcType.size ()) {
			retType = StructType::get (context, funcType);
		} else {
			retType = Type::getVoidTy (context);
		}
		
	}
	
	FunctionType * ft = FunctionType::get (retType, args, false);
	f = Function::Create (ft, Function::ExternalLinkage, name.c_str (), module);
	f->setCallingConv (CallingConv::C);
	
	size_t index=0;
	for (auto ai = f->arg_begin (); ai != f->arg_end (); ++index, ++ ai) {
		ai->setName (func->getArgs ()[index]->getIden ());
	}
	
	functions [func] = f;
	ASTnode body = func->getBody ();
	if (body and !body->isNil ()) {
		auto oldDBC = debugContext;
		Function * prev = activeFunction;
		const FunctionDecleration * decl = activeFunctionDecleration;
		BasicBlock * block = builder.GetInsertBlock ();
		activeFunction = f;
		activeFunctionDecleration = func;
		debugContext.file = getDIFile (func->getContext ());
		DIArray params;
		DICompositeType ftype = diBuilder.createSubroutineType (debugContext.file, params);
		size_t line = func->getBody ()->getLineNo ();
		line = line ? line : 1;
		debugContext.scope = diBuilder.createFunction (debugContext.cu, func->getIden (), f->getName (), debugContext.file, line, ftype, false, true, 0, 0, false, f);
		debugContext.line = 0;
		builder.SetInsertPoint (BasicBlock::Create (context, "entry", activeFunction));
		auto beg = activeFunction->arg_begin ();
		auto end = activeFunction->arg_end ();
		for (const ASTnode & node : func->getArgs ()) {
			Value * v = getValue (node.get (), true);
			builder.CreateStore (beg, v);
			++beg;
		}
		getValue (body);
		verifyFunction (*activeFunction);
		if (fpm)
			fpm->run (*activeFunction);
		activeFunction = prev;
		builder.SetInsertPoint (block);
		activeFunctionDecleration = decl;
		debugContext = oldDBC;
	}
	return f;
	
}


void BodyGenerator::visit (AbstractSyntaxTree & ast) {
	
	for (auto mod : ast.getDependencies ()) {
		visit (mod);
	}
	
	FunctionDecleration funcDecl;
	activeFunctionDecleration = &funcDecl;
	if (!main) {
		FunctionType * ft = FunctionType::get (Type::getVoidTy (context), false);
		main = Function::Create (ft, Function::ExternalLinkage, "__entry__", module);
		builder.SetInsertPoint (BasicBlock::Create (context, "entry", main));
		
		debugContext.cu = diBuilder.createCompileUnit (dwarf::DW_LANG_C99, "numbat", "/usr/local/bin", string ("numbat-0.2.2"), true, string (""), 0);
		debugContext.file = diBuilder.createFile ("numbat", "/usr/local/bin");
		
		DIArray params;
		DICompositeType ftype = diBuilder.createSubroutineType (debugContext.file, params);
		debugContext.scope = diBuilder.createFunction (debugContext.cu, "__entry__", main->getName (), debugContext.file, 0, ftype, false, true, 0, 0, false, main);
		debugContext.scope = diBuilder.createLexicalBlock (debugContext.scope, debugContext.file, 0, 0, 0);
	} else {
		debugContext.scope = diBuilder.createLexicalBlockFile (debugContext.scope, debugContext.file = getDIFile (getContext (&ast)));
	}
	activeFunction = main;
	
	if (builder.GetInsertBlock ()->getTerminator ()) builder.GetInsertBlock ()->getTerminator ()->eraseFromParent ();
	
	visit (static_cast <NumbatScope &> (ast));
	
	builder.CreateRetVoid ();
	verifyFunction (*activeFunction);
	if (fpm)
		fpm->run (*activeFunction);
	
}

void BodyGenerator::visit (ASTallocate & exp) {
	
	const NumbatPointerType * type = dynamic_cast <const NumbatPointerType *> (exp.getType ());
	
	Value * count = builder.CreateLoad (getValue (exp.getAmount ()));
	
	stack.push (allocteArray (count, type));
	
}

void BodyGenerator::visit (ASTbody & exp) {
	
	Value * val;
	for (const ASTnode & node : exp.getBody ()) {
		val = getValue (node);
	}
	stack.push (val);
	
}

void BodyGenerator::visit (ASTbranch & exp) {
	
	BasicBlock * body = BasicBlock::Create (context, "body", activeFunction);
	BasicBlock * alt = BasicBlock::Create (context, "alt", activeFunction);
	BasicBlock * resume = BasicBlock::Create (context, "alt", activeFunction);
	exp.getCond ()->accept (*this);
	Value * cond = makeCompare (builder.CreateLoad (stack.top ()));
	stack.pop ();
	builder.CreateCondBr (cond, body, alt);
	builder.SetInsertPoint (body);
	exp.getBody ()->accept (*this);
	builder.CreateBr (resume);
	builder.SetInsertPoint (alt);
	exp.getAlt ()->accept (*this);
	builder.CreateBr (resume);
	builder.SetInsertPoint (resume);
	
}

void BodyGenerator::visit (ASTcall & exp) {
	
	Function * func = getFunction (exp.getFunction ());
	std::vector <Value *> args;// (exp.getArgs ().size ());
	
	auto argItt = exp.getArgs ().begin (), argEnd = exp.getArgs ().end ();
	//auto param = func->arg_begin (), paramEnd = func->arg_end ();
	auto param = exp.getFunction ()->getArgs ().begin (), paramEnd = exp.getFunction ()->getArgs ().end ();
	for (; param != paramEnd and argItt != argEnd; ++argItt, ++param) {
		Value * val = getValue (*argItt);
		if (!(*param)->isAlias ()) {val = builder.CreateLoad (val);}
		args.push_back (val);
	}
	
	Value * val = builder.CreateCall (func, args);
	if (!val->getType ()->isVoidTy ()) {
		if (val->getType ()->isStructTy ()) {
			uint32_t l = val->getType ()->getStructNumElements ();
			std::vector <Value *> vals (l);
			for (uint32_t i=0; i<l; ++i) {
				vals [i] = builder.CreateExtractValue (val, {i});
			}
			val = createTemp (vals);
		} else {
			val = createTemp (val);
		}
	}
	stack.push (val);
	
}

void BodyGenerator::visit (ASTcallindex & exp) {
	
	ASTcallable * call = exp.getCall ();
	Value * retVal = tempValues [call];
	
	if (!retVal) {
		retVal = tempValues [call] = getValue (exp.getCall ());
	}
	
	if (!call->getFunction ()->hasTag ("cstyle")) {
		if (retVal->getType ()->isPointerTy ()) {
			std::vector <Value *> indicies (2);
			indicies [0] = ConstantInt::get (Type::getInt64Ty (context), APInt (64, 0));
			indicies [1] = ConstantInt::get (Type::getInt32Ty (context), APInt (32, exp.getIndex ()));
			retVal = builder.CreateGEP (retVal, indicies);
		} else {
			retVal = builder.CreateExtractValue (retVal, exp.getIndex ());
		}
	}
	stack.push (retVal);
	
}

void BodyGenerator::visit (ASTconcat & exp) {
	
	const NumbatPointerType * type = dynamic_cast <const NumbatPointerType *> (exp.getLhs ()->getType ());
	
	if (!type) {
		return;
	}
	
	Value * lhs = getValue (exp.getLhs ());
	Value * rhs = getValue (exp.getRhs ());
	
	Value * llen = builder.CreateLoad (getValue (ASTbase::getLength (exp.getLhs ())));
	
	ASTnode n = ASTbase::getLength (exp.getRhs ());
	Value * rlen;
	if (n) {
		rlen = builder.CreateLoad (getValue (n));
	} else {
		rlen = builder.getInt64 (1);
	}
	Value * length = builder.CreateAdd (llen, rlen);
	
	Value * array = allocteArray (length, type);
	
	createMemCpy (array, lhs, llen, exp.getConv ());
	Value * rdestint = builder.CreatePtrToInt (builder.CreateLoad (array), llen->getType ());
	Value * rharray = builder.CreateIntToPtr (builder.CreateAdd (rdestint, llen), array->getType ()->getPointerElementType ());
	rharray = createTemp (rharray);
	if (n) {
		createMemCpy (rharray, rhs, rlen, exp.getConv ());
	} else {
		createMemCpy (builder.CreateLoad (rharray), rhs, nullptr, exp.getConv ());
	}
	
	stack.push (array);
	
}

void BodyGenerator::visit (ASTconstantFPInt & exp) {
	
	Type * type = getType (exp.getType ());
	stack.push (createTemp (ConstantFP::get (type, exp.getValue ())));
	
}

void BodyGenerator::visit (ASTconstantInt & exp) {
	
	Type * type = getType (exp.getType ());
	if (type->isIntegerTy ())
		stack.push (createTemp (builder.getInt (APInt (exp.getBitSize (), exp.getValue ()))));
	else
		stack.push ((Value *)nullptr);
	
}

void BodyGenerator::visit (ASTconstantCString & exp) {
	
	auto itt = strConstants.find (exp.getValue());
	if (itt != strConstants.end ()) {stack.push (createTemp (itt->second)); return;}
	
	StructType * strType = reinterpret_cast <StructType *> (getType (exp.getType ()));
	
	Type * arrType = strType->getStructElementType (0);
	std::vector <Type *> members (exp.getValue ().size () + 2, arrType->getPointerElementType ());
	std::vector <Constant *> string (exp.getValue ().size () + 2);
	string [0] = ConstantInt::get (Type::getInt64Ty (context), APInt (64, exp.getValue ().size ()));
	for (size_t i=1, l=exp.getValue ().size ()+1; i<l; ++i) {
		string [i] = ConstantInt::get (Type::getInt8Ty (context), APInt (8, exp.getValue () [i-1]));
	}
	string [exp.getValue ().size ()+1] = ConstantInt::get (Type::getInt8Ty (context), APInt (8, 0));
	members [0] = Type::getInt64Ty (context);
	
	StructType * type = StructType::get (context, members);
	Constant * value = ConstantStruct::get (type, string);
	GlobalVariable * str = new GlobalVariable (*module, type, true, GlobalValue::ExternalLinkage, value, "str");
	
	Value * gep = builder.CreateStructGEP (str, 1, "gep");
	Value * global = builder.CreateInsertValue (GlobalValue::getNullValue (strType), gep, std::vector <uint32_t> (1, 0));
	
	strConstants [exp.getValue ()] = global;
	
	stack.push (createTemp (global));
	
}

void BodyGenerator::visit (ASTgep & exp) {
	
	Value * ptr = builder.CreateLoad (getValue (exp.getRef ()));
	Value * index = builder.CreateLoad (getValue (exp.getIndex ()));
	
	Value * gep = builder.CreateGEP (ptr, std::vector <Value *> (1, index));
	stack.push (gep);
	
}

void BodyGenerator::visit (ASTmemcpy & exp) {
	
	Value * dest = getValue (exp.getDest ());
	Value * source = getValue (exp.getSource ());
	
	shared_ptr <ASTcallable> conv = exp.getConv ();
	Value * length = nullptr;
	
	if (ASTnode len = ASTbase::getLength (exp.getDest ())) {
		Value * llen = builder.CreateLoad (getValue (len));
		ASTnode rlen = ASTbase::getLength (exp.getSource ());
		
		BasicBlock * body = BasicBlock::Create (context, "body", activeFunction);
		BasicBlock * resume = BasicBlock::Create (context, "resume", activeFunction);
		
		Value * srclen = builder.CreateLoad (getValue (rlen));
		
		Value * cond = builder.CreateICmpULT (llen, srclen);
		
		builder.CreateCondBr (cond, body, resume);
		builder.SetInsertPoint (body);
		builder.CreateStore (builder.CreateLoad (allocteArray (srclen, dynamic_cast <const NumbatPointerType *> (exp.getDest ()->getType ()))), dest);
		builder.CreateBr (resume);
		builder.SetInsertPoint (resume);
		length = srclen;
	} else if (exp.getDest ()->isArray ()) {
		dest = builder.CreateLoad (dest);
	}
	
	createMemCpy (dest, source, length, conv);
	stack.push (dest);
	
}

void BodyGenerator::visit (ASTnumbatInstr & exp) {
	
	Value * val = nullptr;
	std::vector <Value *> args;
	const string & instr = exp.getInstr ();
	
	if (instr == "cast") {
		exp.getArgs () [0]->accept (*this);
		Value * arg = builder.CreateLoad (stack.top ()); stack.pop ();
		Type * type = getType (exp.getArgs () [1]->getType ());
		Value * val;\
		if (arg->getType ()->isFloatingPointTy ()) {
			if (type->isFloatingPointTy ()) {
				if (arg->getType ()->getPrimitiveSizeInBits () < type->getPrimitiveSizeInBits ()) {
					val = builder.CreateFPExt (arg, type);
				} else {
					val = builder.CreateFPTrunc (arg, type);
				}
			} else {
				if (exp.getArgs () [1]->getType ()->isSigned ()) {
					val = builder.CreateFPToSI (arg, type);
				} else {
					val = builder.CreateFPToUI (arg, type);
				}
			}
		} else {
			if (type->isFloatingPointTy ()) {
				if (exp.getArgs () [0]->getType ()->isSigned ()) {
					val = builder.CreateSIToFP (arg, type);
				} else {
					val = builder.CreateUIToFP (arg, type);
				}
			} else {
				bool argSinged = exp.getArgs () [0]->getType ()->isSigned ();
				if (argSinged) {
					if (arg->getType ()->getPrimitiveSizeInBits () != type->getPrimitiveSizeInBits ()) {
						val = builder.CreateSExtOrTrunc (arg, type);
					} else {
						val = arg;
					}
				} else {
					if (arg->getType ()->getPrimitiveSizeInBits () != type->getPrimitiveSizeInBits ()) {
						val = builder.CreateZExtOrTrunc (arg, type);
					} else {
						val = arg;
					}
				}
				
			}
		}
		stack.push (createTemp (val));
		return;
	} else if (instr == "gep") {
		exp.getArgs () [0]->accept (*this);
		Value * lhs = stack.top (); stack.pop ();
		int l = exp.getArgs().size ();
		for (int i=1; i<l; ++i) {
			exp.getArgs () [i]->accept (*this);
			Value * v = stack.top (); stack.pop ();
			if (v->getType ()->isIntegerTy ()) {
				if (v->getType ()->getPrimitiveSizeInBits () != 64) {
					v = builder.CreateZExtOrTrunc (v, Type::getInt64Ty (context));
				}
				args.push_back (v);
			} else {
				//TODO: throw an error
			}
		}
		stack.push (builder.CreateGEP (lhs, args));
		return;
	} else if (instr == "mov") {
		exp.getArgs () [0]->accept (*this);
		Value * lhs = stack.top (); stack.pop ();
		exp.getArgs () [1]->accept (*this);
		Value * rhs = builder.CreateLoad (stack.top ()); stack.pop ();
		builder.CreateStore (rhs, lhs);
		stack.push (lhs);
		return;
	} else if (instr == "redir") {
		exp.getArgs () [0]->accept (*this);
		Value * lhs = stack.top (); stack.pop ();
		exp.getArgs () [1]->accept (*this);
		Value * rhs = stack.top (); stack.pop ();
		if (rhs->getType ()->getPointerElementType ()->isPointerTy ()) {
			rhs = builder.CreateLoad (rhs);
		}
		if (lhs->getType ()->getPointerElementType ()->isPointerTy ()) {
			stack.push (builder.CreateStore (rhs, lhs));
		} else if (shared_ptr <ASTvariable> var = std::dynamic_pointer_cast <ASTvariable> (exp.getArgs () [0])) {
			stack.push (namedValues [var->getVariable ()] = rhs);
		}
		return;
	}
	
	for (const ASTnode & arg : exp.getArgs ()) {
		Value * val = getValue (arg);
		args.push_back (builder.CreateLoad (val));
	}
	
	if (instr == "add") {
		val = builder.CreateAdd (args [0], args [1], instr);
	} else if (instr == "and") {
		val = builder.CreateAnd (args [0], args [1], instr);
	} else if (instr == "cmpe") {
		val = builder.CreateICmpEQ (args [0], args [1], instr);
	} else if (instr == "cmplt") {
		if (exp.getArgs () [0]->getType ()->isSigned ()) {
			val = builder.CreateICmpSLT (args [0], args [1], instr);
		} else {
			val = builder.CreateICmpULT (args [0], args [1], instr);
		}
	} else if (instr == "div") {
		if (exp.getArgs () [0]->getType ()->isSigned ()) {
			val = builder.CreateSDiv (args [0], args [1], instr);
		} else {
			val = builder.CreateUDiv (args [0], args [1], instr);
		}
	} else if (instr == "fadd") {
		val = builder.CreateFAdd (args [0], args [1], instr);
	} else if (instr == "fdiv") {
		val = builder.CreateFDiv (args [0], args [1], instr);
	} else if (instr == "fmul") {
		val = builder.CreateFMul (args [0], args [1], instr);
	} else if (instr == "fneg") {
		val = builder.CreateFNeg (args [0], instr);
	} else if (instr == "frem") {
		val = builder.CreateFRem (args [0], args [1], instr);
	} else if (instr == "fsub") {
		val = builder.CreateFSub (args [0], args [1], instr);
	} else if (instr == "load") {
		val = args [0];
		if (val->getType ()->isPointerTy ())
			val = builder.CreateLoad (val);
	} else if (instr == "mul") {
		val = builder.CreateMul (args [0], args [1], instr);
	} else if (instr == "neg") {
		val = builder.CreateNeg (args [0], instr);
	} else if (instr == "not") {
		val = builder.CreateNot (args [0], instr);
	} else if (instr == "or") {
		val = builder.CreateOr (args [0], args [1], instr);
	} else if (instr == "rem") {
		if (exp.getArgs () [0]->getType ()->isSigned ()) {
			val = builder.CreateSRem (args [0], args [1], instr);
		} else {
			val = builder.CreateURem (args [0], args [1], instr);
		}
	} else if (instr == "sub") {
		val = builder.CreateSub (args [0], args [1], instr);
	} else if (instr == "xor") {
		val = builder.CreateXor (args [0], args [1], instr);
	} else {
		std::cerr << "Invalid instruction: " + instr << std::endl;
		exit (1);
	}
	stack.push (createTemp (val));
	
}

void BodyGenerator::visit (ASTparamater & exp) {
	
	size_t index=0;
	for (Function::arg_iterator ai = activeFunction->arg_begin (); ai != activeFunction->arg_end (); ++index, ++ ai) {
		if (ai->getName () == exp.getIden ()) {
			if (exp.isAlias ()) {
				namedValues [exp.getVariable ()] = ai;
			} else {
				Value * v = getVariableHandle (exp.getVariable ());
				builder.CreateStore (ai, v);
			}
			return;
		}
	}
	
}

void BodyGenerator::visit (ASTredirect & exp) {
	
	Value * target = getValue (exp.getLhs ().get (), true);
	Value * source = getValue (exp.getRhs ().get ());
	builder.CreateStore (source, target);
	
}

void BodyGenerator::visit (ASTreinterpretCast & exp) {
	
	Type * target = getType (exp.getASTType ());
	Value * source = getValue (exp.getArg ());
	if (exp.loadSource ()) source = builder.CreateLoad (source);
	if (target->isPointerTy () and source->getType ()->isPointerTy ()) {
		stack.push (builder.CreateBitCast (source, target));
	} else if (source->getType ()->isPointerTy ()) {
		stack.push (createTemp (builder.CreatePtrToInt (source, target)));
	} else if (target->isPointerTy ()) {
		stack.push (builder.CreateIntToPtr (source, target));
		if (exp.getType ()->isArray ()) {
			stack.top () = createTemp (stack.top ());
		}
	} else {
		abort ();
	}
	
}

void BodyGenerator::visit (ASTreturn & exp) {
	
	Value * ret, * val = getValue (exp.getExpr ());
	if (val->getType ()->isStructTy ()) {
		if (activeFunctionDecleration->hasTag ("cstyle")) {
			val = builder.CreateExtractValue (val, {0});
		}
		ret = builder.CreateRet (val);
	} else {
		val = builder.CreateLoad (val);
		if (activeFunctionDecleration->hasTag ("cstyle")) {
			ret = builder.CreateRet (val);
		} else {
			ret = builder.CreateAggregateRet (&val, 1);
		}
	}
	stack.push (ret);
}

void BodyGenerator::visit (ASTreturnvoid & exp) {
	stack.push (builder.CreateRetVoid ());
}

void BodyGenerator::visit (ASTstructIndex & exp) {
	
	Value * val = getValue (exp.getExpr ());
	
	if (const NumbatPointerType * type = dynamic_cast <const NumbatPointerType *> (exp.getExpr ()->getType ())) {
		Value * dptr = pointerTypeGEP (builder.CreateLoad (val), type, exp.getIndex ());
		stack.push (dptr);
	} else {
		std::vector <Value *> indicies (2);
		indicies [0] = ConstantInt::get (Type::getInt64Ty (context), APInt (64, 0));
		indicies [1] = ConstantInt::get (Type::getInt32Ty (context), APInt (32, exp.getIndex ()));
		val = builder.CreateGEP (val, indicies);
		stack.push (val);
	}
	
}

void BodyGenerator::visit (ASTtuple & exp) {
	
	std::vector <Value *> args;
	for (const ASTnode & node : exp.getElements ()) {
		args.push_back (getValue (node));
	}
	stack.push (createTemp (args, false));
	
}

void BodyGenerator::visit (ASTtuplecall & exp) {
	
	auto lhsItt = exp.getLhsArgs ().begin ();
	auto rhsItt = exp.getRhsArgs ().begin ();
	std::list <Value *> lhsArgs, rhsArgs;
	
	for (const shared_ptr <ASTcallable> & call : exp.getCalls ()) {
		Function * func = getFunction (call->getFunction ());
		auto param = func->arg_begin ();
		param->getType ()->isPointerTy ();
		(*lhsItt)->accept (*this);
		lhsArgs.push_back (stack.top ()); stack.pop ();
		++lhsItt;
		++param;
		param->getType ()->isPointerTy ();
		(*rhsItt)->accept (*this);
		rhsArgs.push_back (stack.top ()); stack.pop ();
		++rhsItt;
	}
	
	auto lhsParam = lhsArgs.begin ();
	auto rhsParam = rhsArgs.begin ();
	for (const shared_ptr <ASTcallable> & call : exp.getCalls ()) {
		Function * func = getFunction (call->getFunction ());
		stack.push (builder.CreateCall2 (func, *lhsParam, *rhsParam));
		++lhsParam;
		++rhsParam;
	}
	
}

void BodyGenerator::visit (ASTvariable & exp) {
	
	Value * v = getVariableHandle (exp.getVariable ());
	stack.push (v);
	
}

void BodyGenerator::visit (numbat::parser::ASTwhileloop & exp) {
	
	Function * func = builder.GetInsertBlock ()->getParent ();
	
	BasicBlock * oldCont = continueBlock;
	BasicBlock * oldBreak = breakBlock;
	
	BasicBlock *  contBlk = continueBlock = BasicBlock::Create (context, "condition", func);
	BasicBlock * loopBlock = BasicBlock::Create (context, "loop", func);
	BasicBlock *  brakeBlk = breakBlock = BasicBlock::Create (context, "break", func);
	builder.CreateBr (contBlk);
	
	builder.SetInsertPoint (contBlk);
	Value * cond = makeCompare (exp.getCondition ());
	builder.CreateCondBr (cond, loopBlock, brakeBlk);
	
	builder.SetInsertPoint (loopBlock);
	getValue (exp.getBody ());
	if (!builder.GetInsertBlock ()->getTerminator())
		builder.CreateBr (contBlk);
	
	builder.SetInsertPoint (brakeBlk);
	
	continueBlock = oldCont;
	breakBlock = oldBreak;
	stack.push (createTemp (builder.getInt (APInt (1, 0))));
	
}

void BodyGenerator::visit (NumbatScope & exp) {
	
	for (NumbatEnumType * et : exp.getEnums ()) {
		for (const auto & node : et->getMembers ()) {
			getValue (node);
		}
	}
	
	Value * val = nullptr;
	for (const auto & node : exp.getBody ()) {
		if (!node->isNil ()) val = getValue (node);
	}
	
	for (const auto & func : exp.getFunctions ()) {
		getFunction (func.second.get ());
	}
	stack.push (val);
	
}

void BodyGenerator::visit (const shared_ptr <Module> & nbtMod) {
	
	//std::cerr << getContext (nbtMod->getAST ())->file << std::endl;
	//debugContext.scope = diBuilder.createCompileUnit (dwarf::DW_LANG_C99, getContext (nbtMod->getAST ())->file, getContext (nbtMod->getAST ())->path, string ("numbat-0.2.2"), true, string (""), 0);
	//debugContext.scope = diBuilder.createFile (getContext (nbtMod->getAST ())->file, getContext (nbtMod->getAST ())->path);
	auto itt = builtModules.find (nbtMod.get ());
	if (itt != builtModules.end ()) return;
	builtModules.insert (nbtMod.get ());
	
	visit (*nbtMod->getAST ());
	
}



};
};
