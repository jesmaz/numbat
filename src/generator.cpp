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

Type * BodyGenerator::getType (const ASTnode & node) {
	
	Type * t = getType (node->getType ().get ());
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

Value * BodyGenerator::allocteArray (Value * length, NumbatPointerType * type) {
	
	size_t mdsize=0;
	for (auto & t : type->getMembers ()) {
		mdsize += dataLayout->getTypeAllocSize (getType (t));
	}
	
	size_t esize = dataLayout->getTypeAllocSize (getType (type->getDataType ()));
	
	Value * mdbytes = builder.getInt64 (mdsize);
	Value * bytes;
	bytes = builder.CreateMul (length, builder.getInt64 (esize));
	bytes = builder.CreateAdd (bytes, mdbytes);
	
	Value * call = builder.CreateCall (memalloc, std::vector <Value *> (1, bytes));
	
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

Value * BodyGenerator::createTemp (Value * val) {
	
	if (ref) {
		Value * alloc = createEntryBlockAlloc (activeFunction, val->getType (), "temp");
		builder.CreateStore (val, alloc);
		val = alloc;
	}
	return val;
	
}

Value * BodyGenerator::getVariableHandle (const NumbatVariable * var) {
	
	Value * hand = namedValues [var];
	
	if (!hand) {
		
		//TODO: handle references to const objects
		
		
		
		Type * type = getType (var->getType().get ());
		//TODO: apply type modifiers
		
		if (var->isGlobal()) {
			hand = new GlobalVariable (*module, type, var->isConst (), GlobalValue::ExternalLinkage, GlobalValue::getNullValue (type), var->getIden ());
		} else {
			hand = createEntryBlockAlloc (activeFunction, type, var->getIden ());
		}
		namedValues [var] = hand;
		
		if (const ASTnode & init = var->getInit ()) {
			bool oldRef = ref;
			ref = true;
			init->accept (*this);
			builder.CreateStore (builder.CreateLoad (stack.top ()), hand);
			stack.pop ();
			ref = oldRef;
		} else {
			builder.CreateStore (initialise (var->getType ().get ()), hand);
		}
		
	}
	
	if (!ref)
		hand = builder.CreateLoad (hand, var->getIden ());
	
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
			val = builder.CreateInsertValue (val, initialise (arg->getType ().get ()), index);
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

void BodyGenerator::makeCompare (const ASTnode & exp) {
	exp->accept (*this);
	Value * v = stack.top ();
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
			stack.top () = v;
		}
	} else {
		std::cerr << "Invalid type" << std::endl;
	}
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
	
	if (func->hasTag ("malloc") and !memalloc) {
		//TODO: function type testing
		memalloc = f;
	}
	
	if (func->hasTag ("free") and !memfree) {
		//TODO: function type testing
		memfree = f;
	}
	
	return functions [func] = f;
	
}


void BodyGenerator::visit (AbstractSyntaxTree & ast) {
	
	for (const std::pair <string, shared_ptr <FunctionDecleration>> & func : ast.getFunctions ()) {
		getFunction (func.second.get ());
	}
	
	for (const std::pair <string, shared_ptr <FunctionDecleration>> & func : ast.getFunctions ()) {
		activeFunctionDecleration = func.second.get ();
		activeFunction = functions [activeFunctionDecleration];
		ASTnode body = func.second->getBody ();
		if (body) {
			builder.SetInsertPoint (BasicBlock::Create (context, "entry", activeFunction));
			for (const ASTnode & node : func.second->getArgs ()) {
				node->accept (*this);
			}
			body->accept (*this);
			//TODO: weight calculations for inlining
			verifyFunction (*activeFunction);
			if (fpm)
				fpm->run (*activeFunction);
		}
	}
	
}

void BodyGenerator::visit (ASTallocate & exp) {
	
	NumbatPointerType * type = dynamic_cast <NumbatPointerType *> (exp.getType ().get ());
	
	if (!type) {
		return;
	}
	
	bool oldRef = ref;
	ref = true;
	exp.getAmount ()->accept (*this);
	ref = oldRef;
	Value * count;
	if (!stack.empty ()) {
		count = builder.CreateLoad (stack.top ()); stack.pop ();
	} else {
		return;
	}
	
	stack.push (allocteArray (count, type));
	
}

void BodyGenerator::visit (ASTbody & exp) {
	
	for (const ASTnode & node : exp.getBody ()) {
		node->accept (*this);
		while (stack.size ()) {
			stack.pop ();
		}
	}
	
}

void BodyGenerator::visit (ASTbranch & exp) {
	
	BasicBlock * body = BasicBlock::Create (context, "body", activeFunction);
	BasicBlock * alt = BasicBlock::Create (context, "alt", activeFunction);
	BasicBlock * resume = BasicBlock::Create (context, "alt", activeFunction);
	bool oldRef = ref;
	ref = true;
	exp.getCond ()->accept (*this);
	Value * cond = makeCompare (builder.CreateLoad (stack.top ()));
	stack.pop ();
	ref = oldRef;
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
	
	Function * func = getFunction (exp.getFunction ().get ());
	std::vector <Value *> args;// (exp.getArgs ().size ());
	
	bool oldAlias = ref;
	auto argItt = exp.getArgs ().begin (), argEnd = exp.getArgs ().end ();
	auto param = func->arg_begin (), paramEnd = func->arg_end ();
	for (; param != paramEnd and argItt != argEnd; ++argItt, ++param) {
		ref = param->getType ()->isPointerTy ();
		(*argItt)->accept (*this);
		args.push_back (stack.top ()); stack.pop ();
	}
	ref = oldAlias;
	
	stack.push (builder.CreateCall (func, args));
	
}

void BodyGenerator::visit (ASTcallindex & exp) {
	
	auto & call = exp.getCall ();
	Value * retVal = tempValues [call.get ()];
	
	if (!retVal) {
		call->accept (*this);
		retVal = tempValues [call.get ()] = stack.top (); stack.pop ();
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
	stack.push (createTemp (retVal));
	
}

void BodyGenerator::visit (ASTconcat & exp) {
	
	NumbatPointerType * type = dynamic_cast <NumbatPointerType *> (exp.getLhs ()->getType ().get ());
	
	if (!type) {
		return;
	}
	
	auto oldRef = ref;
	ref = true;
	exp.getLhs ()->accept (*this);
	Value * lhs = stack.top ();
	stack.pop ();
	exp.getRhs ()->accept (*this);
	Value * rhs = stack.top ();
	stack.pop ();
	
	ASTbase::getLength (exp.getLhs ())->accept (*this);
	Value * llen = builder.CreateLoad (stack.top ());
	stack.pop ();
	
	ASTnode n = ASTbase::getLength (exp.getRhs ());
	Value * rlen;
	if (n) {
		n->accept (*this);
		rlen = builder.CreateLoad (stack.top ());
		stack.pop ();
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
	
	ref = oldRef;
	
}

void BodyGenerator::visit (ASTconstantFPInt & exp) {
	
	Type * type = getType (exp.getType ().get ());
	stack.push (createTemp (ConstantFP::get (type, exp.getValue ())));
	
}

void BodyGenerator::visit (ASTconstantInt & exp) {
	
	Type * type = getType (exp.getType ().get ());
	if (type->isIntegerTy ())
		stack.push (createTemp (builder.getInt (APInt (exp.getBitSize (), exp.getValue ()))));
	else
		stack.push ((Value *)nullptr);
	
}

void BodyGenerator::visit (ASTconstantCString & exp) {
	
	StructType * strType = reinterpret_cast <StructType *> (getType (exp.getType ().get ()));
	
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
	stack.push (createTemp (builder.CreateInsertValue (GlobalValue::getNullValue (strType), gep, std::vector <uint32_t> (1, 0))));
	
}

void BodyGenerator::visit (ASTgep & exp) {
	
	bool oldRef = ref;
	ref = true;
	exp.getRef ()->accept (*this);
	Value * ptr = builder.CreateLoad (stack.top ());
	stack.pop ();
	exp.getIndex ()->accept (*this);
	Value * index = builder.CreateLoad (stack.top ());
	stack.pop ();
	ref = oldRef;
	
	Value * gep = builder.CreateGEP (ptr, std::vector <Value *> (1, index));
	if (!ref) {
		gep = builder.CreateLoad (gep);
	}
	stack.push (gep);
	
}

void BodyGenerator::visit (ASTmemcpy & exp) {
	
	bool oldRef = ref;
	ref = true;
	exp.getDest ()->accept (*this);
	Value * dest = stack.top ();
	stack.pop ();
	exp.getSource ()->accept (*this);
	Value * source = stack.top ();
	stack.pop ();
	
	shared_ptr <ASTcallable> conv = exp.getConv ();
	Value * length = nullptr;
	
	if (ASTnode len = ASTbase::getLength (exp.getDest ())) {
		len->accept (*this);
		length = builder.CreateLoad (stack.top ());
		stack.pop ();
	} else if (exp.getDest ()->isArray ()) {
		dest = builder.CreateLoad (dest);
	}
	
	createMemCpy (dest, source, length, conv);
	ref = oldRef;
	stack.push (dest);
	
}

void BodyGenerator::visit (ASTnumbatInstr & exp) {
	
	Value * val = nullptr;
	std::vector <Value *> args;
	const string & instr = exp.getInstr ();
	
	if (instr == "cast") {
		bool oldref = ref;
		ref = true;
		exp.getArgs () [0]->accept (*this);
		Value * arg = builder.CreateLoad (stack.top ()); stack.pop ();
		Type * type = getType (exp.getArgs () [1]->getType ().get ());
		Value * val;
		ref = oldref;
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
		bool oldAlias = ref;
		ref = true;
		exp.getArgs () [0]->accept (*this);
		ref = oldAlias;
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
		bool oldAlias = ref;
		ref = true;
		exp.getArgs () [0]->accept (*this);
		Value * lhs = stack.top (); stack.pop ();
		exp.getArgs () [1]->accept (*this);
		Value * rhs = builder.CreateLoad (stack.top ()); stack.pop ();
		ref = oldAlias;
		builder.CreateStore (rhs, lhs);
		if (ref) {
			stack.push (lhs);
		} else {
			stack.push (builder.CreateLoad (lhs));
		}
		return;
	} else if (instr == "redir") {
		bool oldAlias = ref;
		ref = true;
		exp.getArgs () [0]->accept (*this);
		Value * lhs = stack.top (); stack.pop ();
		exp.getArgs () [1]->accept (*this);
		Value * rhs = stack.top (); stack.pop ();
		ref = oldAlias;
		if (rhs->getType ()->getPointerElementType ()->isPointerTy ()) {
			rhs = builder.CreateLoad (rhs);
		}
		if (lhs->getType ()->getPointerElementType ()->isPointerTy ()) {
			stack.push (builder.CreateStore (rhs, lhs));
		} else if (shared_ptr <ASTvariable> var = std::dynamic_pointer_cast <ASTvariable> (exp.getArgs () [0])) {
			stack.push (namedValues [var->getVariable ().get ()] = rhs);
		}
		return;
	}
	
	for (const ASTnode & arg : exp.getArgs ()) {
		bool oldRef = ref;
		ref = true;
		arg->accept (*this);
		args.push_back (builder.CreateLoad ((stack.top ()))); stack.pop ();
		ref = oldRef;
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
				namedValues [exp.getVariable ().get ()] = ai;
			} else {
				ref = true;
				Value * v = getVariableHandle (exp.getVariable ().get ());
				ref = false;
				builder.CreateStore (ai, v);
			}
			return;
		}
	}
	
}

void BodyGenerator::visit (ASTreturn & exp) {
	
	const ASTtuple * tuple = dynamic_cast <const ASTtuple *> (exp.getExpr ().get ());
	std::vector <Value *> members;
	if (tuple) {
		for (const ASTnode & node : tuple->getElements ()) {
			node->accept (*this);
			if (!stack.empty ()) {
				members.push_back (stack.top ());
			} else {
				return;
			}
		}
		stack.top () = builder.CreateAggregateRet (members.data (), members.size ());
	} else {
		exp.getExpr ()->accept (*this);
		if (stack.size ()) {
			
			members.push_back (stack.top ());
			if (!activeFunctionDecleration->hasTag ("cstyle")) {
				stack.top () = builder.CreateAggregateRet (members.data (), 1);
			} else {
				stack.top () = builder.CreateRet (stack.top ());
			}
		}
	}
}

void BodyGenerator::visit (ASTreturnvoid & exp) {
	stack.push (builder.CreateRetVoid ());
}

void BodyGenerator::visit (ASTstructIndex & exp) {
	
	bool oldRef = ref;
	ref = true;
	exp.getExpr ()->accept (*this);
	Value * val = stack.top (); stack.pop ();
	ref = oldRef;
	
	if (NumbatPointerType * type = dynamic_cast <NumbatPointerType *> (exp.getExpr ()->getType ().get ())) {
		Value * dptr = pointerTypeGEP (builder.CreateLoad (val), type, exp.getIndex ());
		if (ref) {
			stack.push (dptr);
		} else {
			stack.push (builder.CreateLoad (dptr));
		}
	} else {
		std::vector <Value *> indicies (2);
		indicies [0] = ConstantInt::get (Type::getInt64Ty (context), APInt (64, 0));
		indicies [1] = ConstantInt::get (Type::getInt32Ty (context), APInt (32, exp.getIndex ()));
		val = builder.CreateGEP (val, indicies);
		if (ref) {
			stack.push (val);
		} else {
			stack.push (builder.CreateLoad (val));
		}
	}
	
}

void BodyGenerator::visit (ASTvariable & exp) {
	
	Value * v = getVariableHandle (exp.getVariable ().get ());
	stack.push (v);
	
}

void BodyGenerator::visit (ASTtuplecall & exp) {
	
	auto lhsItt = exp.getLhsArgs ().begin ();
	auto rhsItt = exp.getRhsArgs ().begin ();
	std::list <Value *> lhsArgs, rhsArgs;
	
	bool oldAlias = ref;
	for (const shared_ptr <ASTcallable> & call : exp.getCalls ()) {
		Function * func = getFunction (call->getFunction ().get ());
		auto param = func->arg_begin ();
		ref = param->getType ()->isPointerTy ();
		(*lhsItt)->accept (*this);
		lhsArgs.push_back (stack.top ()); stack.pop ();
		++lhsItt;
		++param;
		ref = param->getType ()->isPointerTy ();
		(*rhsItt)->accept (*this);
		rhsArgs.push_back (stack.top ()); stack.pop ();
		++rhsItt;
	}
	ref = oldAlias;
	
	auto lhsParam = lhsArgs.begin ();
	auto rhsParam = rhsArgs.begin ();
	for (const shared_ptr <ASTcallable> & call : exp.getCalls ()) {
		Function * func = getFunction (call->getFunction ().get ());
		stack.push (builder.CreateCall2 (func, *lhsParam, *rhsParam));
		++lhsParam;
		++rhsParam;
	}
	
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
	makeCompare (exp.getCondition ());
	Value * cond = stack.top (); stack.pop ();
	builder.CreateCondBr (cond, loopBlock, brakeBlk);
	
	builder.SetInsertPoint (loopBlock);
	exp.getBody ()->accept (*this);
	if (!builder.GetInsertBlock ()->getTerminator())
		builder.CreateBr (contBlk);
	
	builder.SetInsertPoint (brakeBlk);
	
	continueBlock = oldCont;
	breakBlock = oldBreak;
	
}

void BodyGenerator::visit (const shared_ptr <Module> & nbtMod) {
	
	if (!memfree) {
		memfree = getFunction (parser::Module::getFree ().get ());
	}
	if (!memalloc) {
		memalloc = getFunction (parser::Module::getMalloc ().get ());
	}
	
	if (!main) {
		FunctionDecleration funcDecl;
		activeFunctionDecleration = &funcDecl;
		FunctionType * ft = FunctionType::get (Type::getVoidTy (context), false);
		activeFunction = main = Function::Create (ft, Function::ExternalLinkage, "__entry__", module);
		builder.SetInsertPoint (BasicBlock::Create (context, "entry", activeFunction));
		for (const ASTnode & n : parser::Module::getMain ()) {
			n->accept (*this);
		}
		builder.CreateRetVoid ();
		verifyFunction (*activeFunction);
		if (fpm)
			fpm->run (*activeFunction);
	}
	
	auto itt = builtModules.find (nbtMod.get ());
	if (itt != builtModules.end ()) return;
	builtModules.insert (nbtMod.get ());
	
	for (const shared_ptr <Module> & mod : nbtMod->getDependencies ()) {
		visit (mod);
	}
	
	for (const std::pair <string, shared_ptr <FunctionDecleration>> & func : nbtMod->getFunctions ()) {
		activeFunctionDecleration = func.second.get ();
		activeFunction = getFunction (func.second.get ());
		ASTnode body = func.second->getBody ();
		if (body and activeFunction->empty ()) {
			builder.SetInsertPoint (BasicBlock::Create (context, "entry", activeFunction));
			for (const ASTnode & node : func.second->getArgs ()) {
				node->accept (*this);
			}
			body->accept (*this);
			verifyFunction (*activeFunction);
			if (fpm)
				fpm->run (*activeFunction);
		}
	}
	
}



};
};
