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
	} else if (type->getMembers ().size ()) {
		std::vector <Type *> members;
		for (const ASTnode & node : type->getMembers ()) {
			members.push_back (getType (node));
		}
		s = StructType::get (context, members);
	} else {
		const NumbatRawType * rawType = dynamic_cast <const NumbatRawType *> (type);
		if (rawType) {
			switch (rawType->getRawType ()) {
				case NumbatRawType::FLOAT:
					switch (rawType->getSize ()) {
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
					s = Type::getIntNTy (context, type->getSize ());
			}
		} else {
			s = Type::getVoidTy (context);
		}
		
	}
	
	if (type) {
		std::cerr << type->toString () << " : ";
	} else {
		std::cerr << "void : ";
	}
	s->dump ();
	std::cerr << std::endl;
	return structures [type] = s;
	
}

Value * BodyGenerator::getVariableHandle (const NumbatVariable * var) {
	
	Value * hand = namedValues [var];
	
	if (!hand) {
		
		//TODO: handle references to const objects
		
		
		
		Type * type = getType (var->getType().get ());
		//TODO: apply type modifiers
		hand = createEntryBlockAlloc (activeFunction, type, var->getIden ());
		namedValues [var] = hand;
		
	}
	
	if (!alias)
		hand = builder.CreateLoad (hand, var->getIden ());
	
	return hand;
	
}

void BodyGenerator::makeCompare (const ASTnode & exp) {
	exp->accept (*this);
	Value * v = stack.top ();
	if (v->getType()->getIntegerBitWidth () != 1) {
		if (v->getType ()->isFloatingPointTy ()) {
			v = builder.CreateFCmpONE (v, ConstantFP::get (v->getType (), 0.0));
		} else {
			v = builder.CreateICmpNE (v, ConstantInt::get (v->getType (), APInt (v->getType ()->getIntegerBitWidth (), 0))); // compare with zero
		}
		stack.top () = v;
	}
}

void BodyGenerator::registerFunction (const FunctionDecleration * func) {
	
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
			std::cerr << "SEVERE: cstyle function has made it to the generator" << std::endl;
		}
		
	} else {
		
		if (funcType.size () == 1 and funcType.front ()->isVoidTy ()) {
			retType = funcType.front ();
		} else if (funcType.size ()) {
			retType = StructType::get (context, funcType);
		} else {
			retType = Type::getVoidTy (context);
		}
		
	}
	
	FunctionType * ft = FunctionType::get (retType, args, false);
	Function * f = Function::Create (ft, Function::ExternalLinkage, name.c_str (), module);
	f->setCallingConv (CallingConv::C);
	
	size_t index=0;
	for (auto ai = f->arg_begin (); ai != f->arg_end (); ++index, ++ ai) {
		ai->setName (func->getArgs ()[index]->getIden ());
	}
	
	functions [func] = f;
	
}


void BodyGenerator::visit (AbstractSyntaxTree & ast) {
	
	for (const std::pair <string, shared_ptr <FunctionDecleration>> & func : ast.getFunctions ()) {
		registerFunction (func.second.get ());
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
			activeFunction->dump ();
			verifyFunction (*activeFunction);
			if (fpm)
				fpm->run (*activeFunction);
		}
	}
	
	module->dump ();
	
}

void BodyGenerator::visit (ASTbody & exp) {
	
	for (const ASTnode & node : exp.getBody ()) {
		node->accept (*this);
		while (stack.size ()) {
			if (stack.top ())
				stack.top ()->dump ();
			stack.pop ();
		}
	}
	
}

void BodyGenerator::visit (ASTcall & exp) {
	
	Function * func = functions [exp.getFunction ().get ()];
	std::vector <Value *> args;// (exp.getArgs ().size ());
	
	bool oldAlias = alias;
	auto argItt = exp.getArgs ().begin (), argEnd = exp.getArgs ().end ();
	auto param = func->arg_begin (), paramEnd = func->arg_end ();
	for (; param != paramEnd and argItt != argEnd; ++argItt, ++param) {
		alias = param->getType ()->isPointerTy ();
		(*argItt)->accept (*this);
		args.push_back (stack.top ()); stack.top ()->getType ()->dump (); stack.pop ();
		param->getType ()->dump ();
	}
	alias = oldAlias;
	
	stack.push (builder.CreateCall (func, args));
	
}

void BodyGenerator::visit (ASTcallindex & exp) {
	
	auto & call = exp.getCall ();
	Value * retVal = tempValues [call.get ()];
	
	if (!retVal) {
		call->accept (*this);
		retVal = tempValues [call.get ()] = stack.top (); stack.pop ();
	}
	
	if (call->getFunction ()->hasTag ("cstyle")) {
		stack.push (retVal);
	} else {
		if (retVal->getType ()->isPointerTy ()) {
			std::vector <Value *> indicies (2);
			indicies [0] = ConstantInt::get (Type::getInt64Ty (context), APInt (64, 0));
			indicies [1] = ConstantInt::get (Type::getInt32Ty (context), APInt (32, exp.getIndex ()));
			stack.push (builder.CreateGEP (retVal, indicies));
		} else {
			stack.push (builder.CreateExtractValue (retVal, exp.getIndex ()));
		}
	}
	
}

void BodyGenerator::visit (ASTconstantInt & exp) {
	
	Type * type = getType (exp.getType ().get ());
	if (type->isIntegerTy ())
		stack.push (ConstantInt::get (type, APInt (exp.getSize (), exp.getValue ())));
	else
		stack.push ((Value *)nullptr);
	
}

void BodyGenerator::visit (ASTnumbatInstr & exp) {
	
	Value * val = nullptr;
	std::vector <Value *> args;
	const string & instr = exp.getInstr ();
	
	if (instr == "cast") {
		exp.getArgs () [0]->accept (*this);
		Value * arg = stack.top (); stack.pop ();
		Type * type = getType (exp.getArgs () [1]->getType ().get ());
		if (arg->getType ()->isFloatingPointTy ()) {
			if (type->isFloatingPointTy ()) {
				if (arg->getType ()->getPrimitiveSizeInBits () < type->getPrimitiveSizeInBits ()) {
					stack.push (builder.CreateFPExt (arg, type));
				} else {
					stack.push (builder.CreateFPTrunc (arg, type));
				}
			} else {
				if (exp.getArgs () [0]->getType ()->isSigned ()) {
					stack.push (builder.CreateFPToSI (arg, type));
				} else {
					stack.push (builder.CreateFPToUI (arg, type));
				}
			}
		} else {
			if (type->isFloatingPointTy ()) {
				if (exp.getArgs () [1]->getType ()->isSigned ()) {
					stack.push (builder.CreateSIToFP (arg, type));
				} else {
					stack.push (builder.CreateUIToFP (arg, type));
				}
			} else {
				stack.push (builder.CreateSExtOrTrunc (arg, type));
			}
		}
		return;
	} else if (instr == "mov") {
		bool oldAlias = alias;
		alias = true;
		exp.getArgs () [0]->accept (*this);
		alias = oldAlias;
		Value * lhs = stack.top (); stack.pop ();
		exp.getArgs () [1]->accept (*this);
		Value * rhs = stack.top (); stack.pop ();
		stack.push (builder.CreateStore (rhs, lhs));
		return;
	}
	
	for (const ASTnode & arg : exp.getArgs ()) {
		arg->accept (*this);
		args.push_back (stack.top ()); stack.pop ();
	}
	
	if (instr == "add") {
		val = builder.CreateAdd (args [0], args [1], instr);
	} else if (instr == "and") {
		val = builder.CreateAnd (args [0], args [1], instr);
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
	}
	stack.push (val);
	
}

void BodyGenerator::visit (ASTparamater & exp) {
	
	size_t index=0;
	for (Function::arg_iterator ai = activeFunction->arg_begin (); ai != activeFunction->arg_end (); ++index, ++ ai) {
		if (ai->getName () == exp.getIden ()) {
			if (exp.isAlias ()) {
				namedValues [exp.getVariable ().get ()] = ai;
			} else {
				alias = true;
				Value * v = getVariableHandle (exp.getVariable ().get ());
				alias = false;
				builder.CreateStore (ai, v);
			}
			return;
		}
	}
	
}

void BodyGenerator::visit (ASTreturn & exp) {
	
	exp.getExpr ()->accept (*this);
	if (stack.size ()) {
		std::vector <Value *> members;
		members.push_back (stack.top ());
		if (!activeFunctionDecleration->hasTag ("cstyle")) {
			stack.top () = builder.CreateAggregateRet (members.data (), 1);
		} else {
			stack.top () = builder.CreateRet (stack.top ());
		}
	}
	
}

void BodyGenerator::visit (ASTreturnvoid & exp) {
	stack.push (builder.CreateRetVoid ());
}

void BodyGenerator::visit (ASTstructIndex & exp) {
	exp.getExpr ()->accept (*this);
	Value * val = stack.top (); stack.pop ();
	if (val->getType ()->isPointerTy ()) {
		val = builder.CreateGEP (val, 0);
	}
	stack.push (builder.CreateExtractValue (val, exp.getIndex ()));
}

void BodyGenerator::visit (ASTvariable & exp) {
	
	Value * v = getVariableHandle (exp.getVariable ().get ());
	stack.push (v);
	
}



};
};
