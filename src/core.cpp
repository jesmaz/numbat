#include "../include/core.hpp"

namespace numbat {
namespace parser {


ASTnode makeFunctionCall (NumbatScope * scope, const std::shared_ptr <ASTcallable> & callable) {
	static size_t count = 0;
	ASTnode callIndex = ASTnode (new ASTcallindex (callable->getLineNo (), callable, 0));
	NumbatVariable * var = createVariable (scope, callIndex->getASTType (), callIndex, callable->getIden () + " " + std::to_string (count++), false, true);
	return ASTnode (new ASTvariable (callIndex->getLineNo (), var));
}


const std::map <string, string> instructions = [] {
	std::map <string, string> mp;
	mp ["- "] = "neg";
	mp ["not "] = "lnot";
	mp ["! "] = "lnot";
	mp ["~ "] = "not";
	
	mp [" * "] = "mul";
	mp [" / "] = "div";
	mp [" % "] = "rem";
	
	mp [" + "] = "add";
	mp [" - "] = "sub";
	
	mp [" << "] = "shl";
	mp [" >> "] = "shr";
	
	mp [" & "] = "and";
	
	mp [" | "] = "or";
	
	mp [" ^ "] = "xor";
	
	mp [" < "] = "cmplt";
	mp [" <= "] = "cmple";
	mp [" > "] = "cmpgt";
	mp [" >= "] = "cmpge";
	
	mp [" == "] = "cmpe";
	mp [" != "] = "cmpne";
	return mp;
}();


ASTnode defArithmetic (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	if (args [0]->getType () and args [0]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [0]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (args [0]->getLineNo (), instr, std::vector <ASTnode> ({args [0], createStaticCast (args [1], args [0], 1)}), args [0]));
		
	} else if (args [1]->getType () and args [1]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [1]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (args [0]->getLineNo (), instr, std::vector <ASTnode> ({createStaticCast (args [0], args [1], 1), args [1]}), args [1]));
		
	} else {
		return ASTnode (new ASTerror (args [0]->getLineNo (), "No suitable function found"));
	}
	
}

ASTnode defAs (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	//TODO: determine is cast makes sense
	ASTnode type (new ASTtype (args [0]->getLineNo (), !(args [1]->getType ()->isArray () or args [1]->getType ()->getIden () == "ptrint"), args [1]->isConst (), args [1]->getType ()));
	return ASTnode (new ASTreinterpretCast (args [0]->getLineNo (), args [0], type, args [0]->getType ()->isArray () or args [0]->getType ()->getIden () == "ptrint"));
	
}

ASTnode defAssign (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	std::vector <FunctionDecleration *> candidates = args [0]->getType ()->getMethods (func);
	auto callable = findBestMatch (std::vector <ASTnode> {args [0], args [1]}, candidates);
	if (callable->isValid ()) {
		return makeFunctionCall (scope, callable);
	} else {
		ASTnode lhs = args [0], rhs = createStaticCast (args [1], args [0]);
		if (lhs->getType ()->isSimple () or lhs->getType ()->isArray ()) {
			return ASTnode (new ASTmemcpy (lhs->getLineNo (), lhs, rhs));
		} else {
			NumbatScope * fScope = createChild (scope);
			ASTnode lhstype (new ASTtype (lhs->getLineNo (), true, false, lhs->getType ()));
			ASTnode rhstype (new ASTtype (lhs->getLineNo (), true, true, lhs->getType ()));
			ASTnode rettype (new ASTtype (lhs->getLineNo (), true, true, lhs->getType ()));
			ASTnode varLhs (new ASTvariable (lhs->getLineNo (), createVariable (fScope, lhstype, nullptr, "lhs", false, false)));
			ASTnode varRhs (new ASTvariable (lhs->getLineNo (), createVariable (fScope, rhstype, nullptr, "rhs", false, false)));
			ASTnode varRet (new ASTvariable (lhs->getLineNo (), createVariable (fScope, rettype, nullptr, "ret", false, false)));
			FunctionDecleration * funcDec = createFunctionDecleration (scope, func, {varLhs, varRhs}, {varRet}, {}, fScope);
			setFunction (fScope, funcDec);
			
			if (funcDec) {
				const NumbatVariable * var = getVariable (fScope, "lhs");
				auto & members = var->getType ()->getMembers ();
				size_t i=0, l=members.size ();
				ASTnode tmpLhs (fScope->resolveSymbol ("lhs"));
				ASTnode tmpRhs (fScope->resolveSymbol ("rhs"));
				for (; i<l; ++i) {
					addToBody (fScope, defAssign (fScope, func, {ASTnode (new ASTstructIndex (tmpLhs->getLineNo (), i, tmpLhs)), ASTnode (new ASTstructIndex (tmpRhs->getLineNo (), i, tmpRhs))}));
				}
				addToBody (fScope, ASTnode (new ASTreturn (lhs->getLineNo (), fScope->resolveSymbol ("lhs"))));
				const_cast <NumbatType *> (lhs->getType ())->addMethod (func, funcDec);
				funcDec->assignBody (* new ASTnode (fScope));
				return defAssign (scope, func, args);
			} else {
				return ASTnode (new ASTerror (lhs->getLineNo (), "Unable to create a default assignment operator for '" + lhs->getASTType ()->getIden () + "'"));
			}
		}
	}
	
}

ASTnode defCompare (NumbatScope * scope, const string & func, const std::vector< ASTnode > & args) {
	
	const NumbatType * nType = getType (scope, "bool");
	if (!nType) {
		return ASTnode (new ASTerror (args [0]->getLineNo (), "'bool' is not declared"));
	}
	ASTnode type (new ASTtype (args [0]->getLineNo (), false, true, nType));
	if (args [0]->getType () and args [0]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [0]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (args [0]->getLineNo (), instr, std::vector <ASTnode> ({args [0], createStaticCast (args [1], args [0], 1)}), type));
		
	} else if (args [1]->getType () and args [1]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [1]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (args [0]->getLineNo (), instr, std::vector <ASTnode> ({createStaticCast (args [0], args [1], 1), args [1]}), type));
		
	} else {
		return ASTnode (new ASTerror (args [0]->getLineNo (), "No suitable function found"));
	}
	
}

ASTnode defConcat (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	ASTnode init = ASTnode (new ASTconcat (args [0]->getLineNo (), args [0], args [1]));
	NumbatVariable * var = createVariable (scope, args [0]->getASTType (), init, "concat", false, true);
	return ASTnode (new ASTvariable (args [0]->getLineNo (), var));
	
}

ASTnode defLogic (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	return ASTnode (new ASTerror (args [0]->getLineNo (), "Logical operators are not yet implemented"));
	
}

ASTnode defNegation (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	if (args [0]->getType () and args [0]->getType ()->isRaw ()) {
		auto itt = instructions.find (func);
		string instr = (args [0]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (args [0]->getLineNo (), instr, std::vector <ASTnode> ({args [0]}), args [0]));
	} else {
		return ASTnode (new ASTerror (args [0]->getLineNo (), "No suitable function found"));
	}
	
}


ASTnode parseArrayDecleration (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	std::vector <ASTnode> dimentions;
	switch (args.size ()) {
		case 2:
			dimentions.push_back (ASTnode (new ASTnil (args [0].itt->line)));
			break;
		case 3: {
			Position pos = args [1];
			while (Position exp = nextArg (pos)) {
				if (exp) {
					dimentions.push_back (parseExpression (exp, scope));
				} else {
					dimentions.push_back (ASTnode (new ASTnil (args [0].itt->line)));
				}
				pos += exp;
			}
			break;
		}
		default:
			return ASTnode (new ASToperatorError (0, "Array decelerations require 2 or 3 arguments"));
	}
	
	if (args.back ().itt->type != lexer::TOKEN::identifier) {
		return generateOperatorError (args.back (), "Identifier expected");
	}
	if (args.back () + 1) {
		return generateOperatorError (args.back () + 1, "Unexpected token: '" + (args.back () + 1).itt->iden + "'");
	}
	
	const string & iden = args.back ().itt->iden;
	ASTnode typeNode = parseExpression (args.front (), scope, *matches);
	
	if (typeid (*typeNode.get ()) != typeid (ASTtype)) {
		return generateError (args.front (), "Type expected");
	}
	
	ASTnode type (new ASTtype (args [0].itt->line, typeNode->isAlias (), typeNode->isConst (), getArrayType (scope, typeNode->getType (), dimentions.size ())));
	
	bool nil = false;
	for (ASTnode & node : dimentions) {
		if (node->isNil ()) {
			nil = true;
			break;
		}
	}
	
	size_t line = args [0].itt->line;
	ASTnode init = nullptr;
	if (!nil and !dimentions.empty ()) {
		std::list <ASTnode> args;
		args.push_back (dimentions [0]);
		for (size_t i=1; i<dimentions.size (); ++i) {
			args.push_back (ASTnode (new ASTnumbatInstr (line, "mul", {args.back (), dimentions [i]}, dimentions [0])));
		}
		args.push_front (ASTnode (new ASTallocate (line, args.back (), type->getType ())));
		init = ASTnode (new ASTtuple (line, args));
	}
	NumbatVariable * var = createVariable (scope, type, init, iden, false, false);
	if (!var) {
		return generateError (args.back (), "'" + iden + "' already declared in this scope");
	}
	return ASTnode (new ASTvariable (args [0].itt->line, var));
	
}

ASTnode parseAssignmentOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	//TODO: type inference
	//TODO: initialisation of variables
	if (args.size () != 2) return ASTnode (new ASToperatorError (0, "Assignment operators require exactly two arguments"));
	ASTnode rhs = parseExpression (args [1], scope, *matches);
	if (!rhs->isValid ()) return rhs;
	ASTnode lhs = parseExpression (args [0], scope, *matches);
	if (!lhs->isValid ()) return rhs;
	giveNode (scope, lhs);
	giveNode (scope, rhs);
	
	if (lhs->isList ()) {
		auto list = lhs->getList ();
		if (list.size () == 1) {
			lhs = list.front ();
		}
	}
	
	if (rhs->isList ()) {
		auto list = rhs->getList ();
		if (list.size () == 1) {
			rhs = list.front ();
		}
	}
	
	if (!lhs->isValid () or !rhs->isValid () or !lhs->getType () or !rhs->getType ()) {
		return ASTnode (new ASTtuple (args [0].itt->line, std::list <ASTnode> {lhs, rhs}));
	}
	
	return defAssign (scope, func, {lhs, rhs});
	
}

ASTnode parseBinary (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError (0, "Binary operators require exactly two arguments"));
	
	ASTnode lhs = parseExpression (args [0], scope, *matches);
	ASTnode rhs = parseExpression (args [1], scope, *matches);
	
	giveNode (scope, lhs);
	giveNode (scope, rhs);
	
	if (lhs->isList ()) {
		auto list = lhs->getList ();
		if (list.size () == 1) {
			lhs = list.front ();
		}
	}
	
	if (rhs->isList ()) {
		auto list = rhs->getList ();
		if (list.size () == 1) {
			rhs = list.front ();
		}
	}
	
	if (!lhs->isValid () or !rhs->isValid () or !lhs->getType () or !rhs->getType ()) {
		return ASTnode (new ASTtuple (0, std::list <ASTnode> {lhs, rhs}));
	}
	
	std::vector <FunctionDecleration *> candidates = lhs->getType ()->getMethods (func), rhsCand = rhs->getType ()->getMethods (func);
	std::copy (rhsCand.begin (), rhsCand.end (), std::back_inserter (candidates));
	auto callable = findBestMatch (std::vector <ASTnode> {lhs, rhs}, candidates);
	
	if (callable->isValid ()) {
		return makeFunctionCall (scope, callable);
	} else if (defImp) {
		return defImp (scope, func , std::vector <ASTnode> ({lhs, rhs}));
	} else {
		return ASTnode (new ASTerror (args [0].itt->line, "Something went wrong"));
	}
	
}

ASTnode parseBlockOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	switch (args.size ()) {
		case 0:
			return ASTnode (new ASTnil (0));
		case 1:
			return parseBody (args [0], scope);
		default:
			return ASTnode (new ASTerror (args [0].itt->line, "Something went wrong"));
	}
	
}

ASTnode parseCall (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	std::vector <ASTnode> params;
	ASTnode exp;
	std::vector <FunctionDecleration *> candidates;
	std::shared_ptr <ASTcallable> callable;
	switch (args.size ()) {
		case 2:
			params = parseArgs (args [1], scope);
		case 1:
			exp = parseExpression (args [0], scope, *matches);
			if (!exp->isValid ()) return exp;
			if (typeid (*exp.get ()) == typeid (ASTfunctionlist)) {
				candidates = static_cast <ASTfunctionlist *> (exp.get ())->getElements ();
			} else if (exp->getType ()) {
				candidates = exp->getType ()->getMethods (func);
			} else {
				return ASTnode (new ASTerror (args [0].itt->line, "Something went wrong"));
			}
			{
				auto callable = findBestMatch (params, candidates);
				if (callable->isValid () and !callable->getFunction ()->getType ().empty ()) {
					return makeFunctionCall (scope, callable);
				} else {
					return callable;
				}
			}
		default:
			return ASTnode (new ASTerror (0, "Something went wrong"));
	}
	
}

ASTnode parseComma (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError (0, "Comma operator requires exactly two arguments"));
	
	ASTnode lhs = parseExpression (args [0], scope, *matches);
	ASTnode rhs = parseExpression (args [1], scope, *matches);
	
	return ASTnode (new ASTtuple (args [0].end->line, std::list <ASTnode> {lhs, rhs}));
	
}

ASTnode parseIfStatment (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () < 2 or args.size () > 3) return ASTnode (new ASToperatorError (0, "If statements require a body"));
	NumbatScope * child = createChild (scope);
	ASTnode cond = parseExpression (args.front (), child);
	ASTnode body = parseExpression (args [1], child);
	if (args.size () == 3) {
		ASTnode alt = parseExpression (args.back (), child);
		return ASTnode (new ASTbranch (args [0].itt->line, cond, body, alt));
	} else {
		return ASTnode (new ASTbranch (args [0].itt->line, cond, body));
	}
	
}

ASTnode parseIndex (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	std::vector <ASTnode> params;
	ASTnode exp;
	std::vector <FunctionDecleration *> candidates;
	std::shared_ptr <ASTcallable> callable;
	switch (args.size ()) {
		case 2:
			params = parseArgs (args [1], scope);
		case 1:
			exp = parseExpression (args [0], scope, *matches);
			if (exp->getType ()) {
				if (exp->getType ()->isArray ()) {
					return ASTnode (new ASTgep (args [0].itt->line, exp, params[0]));
				} else {
					candidates = exp->getType ()->getMethods (func);
					callable = findBestMatch (params, candidates);
					if (callable->isValid ()) {
						return makeFunctionCall (scope, callable);
					} else {
						return ASTnode (new ASTerror (args [0].itt->line, "TODO: print candidates for functions in parseCall"));
					}
				}
			} else {
				return generateError (args [0], "Expected a variable");
			}
		default:
			return ASTnode (new ASTerror (0, "Something went wrong"));
	}
	
}

ASTnode parseRedirectOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError (0, "Redirect operator requires exactly two arguments"));
	size_t line = args.front ().itt->line;
	ASTnode rhs = parseExpression (args [1], scope, *matches);
	if (!rhs->isValid ()) return rhs;
	if (!rhs->getType ()) return ASTnode (new ASTerror (line, rhs->getIden () + " has no type"));
	ASTnode lhs = parseExpression (args [0], scope, *matches);
	if (!lhs->isValid ()) return lhs;
	if (!lhs->getType ()) return ASTnode (new ASTerror (line, lhs->getIden () + " has no type"));
	if (lhs->getType () != rhs->getType ()) return ASTnode (new ASTerror (line, "Can not redirect ref " + lhs->getType ()->getIden () + " to " + rhs->getType ()->getIden ()));
	return ASTnode (new ASTredirect (line, lhs, rhs));
	
}

ASTnode parseReferenceOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if  (args.size () != 2)return ASTnode (new ASToperatorError (0, "Reference operator requires exactly two arguments"));
	
	Position lhsPos = args [0];
	
	bool constTkn=false, ref=false;
	while (lhsPos.itt->type == lexer::TOKEN::typemodifier) {
		if (lhsPos.itt->iden == "ref") {
			ref = true;
		} else if (lhsPos.itt->iden == "const") {
			constTkn = true;
		}
		++lhsPos;
	}
	
	ASTnode lhs = parseExpression (lhsPos, scope, *matches);
	
	Position pos = args [1];
	ASTnode ret = resolveSymbol (scope, pos.itt->iden, lhs);
	
	if (typeid (*ret.get ()) == typeid (ASTtype)) {
		*static_cast <ASTtype *> (ret.get ()) = ASTtype (args [1].itt->line, ref, constTkn, ret->getType ());
	}
	
	if (++pos) {
		if (pos.itt->type == lexer::TOKEN::identifier) {
			ret = ASTnode (new ASTvariable (args [1].itt->line, createVariable (scope, ret, nullptr, pos.itt->iden, false, false)));
		} else {
			ret = generateOperatorError (pos, "Identifier expected");
		}
		++pos;
	}
	
	return ret;
	
}

ASTnode parseSubExpression (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	size_t line = args.front ().itt->line;
	switch (args.size ()) {
		case 0:
			return ASTnode (new ASTnil (line));
		case 1:
			return ASTnode (new ASTsubexp (line, parseExpression (args [0], scope)));
		default:
			return ASTnode (new ASTerror (line, "Something went wrong"));
	}
	
}

ASTnode parseUnary (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	size_t line = args.front ().itt->line;
	if (args.size () != 1) return ASTnode (new ASToperatorError (line, "Unary operators require exactly one argument"));
	ASTnode arg = parseExpression (args [0], scope, *matches);
	
	if (!arg->getType ()) return arg;
	
	std::vector <FunctionDecleration *> candidates = arg->getType ()->getMethods (func);
	auto callable = findBestMatch ({arg}, candidates);
	
	if (callable->isValid ()) {
		return makeFunctionCall (scope, callable);
	} else if (defImp) {
		return defImp (scope, func , {arg});
	} else {
		return ASTnode (new ASTerror (line, "Something went wrong"));
	}
	
}

ASTnode parseWhileLoop (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError (args.front ().itt->line, "While loops require a condition and a body"));
	NumbatScope * child = createChild (scope);
	ASTnode cond = parseExpression (args.front (), child);
	ASTnode body = parseExpression (args.back (), child);
	return ASTnode (new ASTwhileloop (args.front ().itt->line, cond, body));
	
}


};
};