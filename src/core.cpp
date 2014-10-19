#include "../include/core.hpp"

namespace numbat {
namespace parser {


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
		return ASTnode (new ASTnumbatInstr (instr, std::vector <ASTnode> ({args [0], createStaticCast (args [1], args [0], 1)}), args [0]));
		
	} else if (args [1]->getType () and args [1]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [1]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (instr, std::vector <ASTnode> ({createStaticCast (args [0], args [1], 1), args [1]}), args [1]));
		
	} else {
		return ASTnode (new ASTerror ("No suitable function found"));
	}
	
}

ASTnode defCompare (NumbatScope * scope, const string & func, const std::vector< ASTnode > & args) {
	
	const NumbatType * nType = getType (scope, "bool");
	if (!nType) {
		return ASTnode (new ASTerror ("'bool' is not declared"));
	}
	ASTnode type (new ASTtype (false, true, nType));
	if (args [0]->getType () and args [0]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [0]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (instr, std::vector <ASTnode> ({args [0], createStaticCast (args [1], args [0], 1)}), type));
		
	} else if (args [1]->getType () and args [1]->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (args [1]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (instr, std::vector <ASTnode> ({createStaticCast (args [0], args [1], 1), args [1]}), type));
		
	} else {
		return ASTnode (new ASTerror ("No suitable function found"));
	}
	
}

ASTnode defConcat (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	return ASTnode (new ASTconcat (args [0], args [1]));
	
}

ASTnode defLogic (NumbatScope * scope, const string & func, const std::vector< ASTnode > & args) {
	
	return ASTnode (new ASTerror ("Logical operators are not yet implemented"));
	
}

ASTnode defNegation (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args) {
	
	if (args [0]->getType () and args [0]->getType ()->isRaw ()) {
		auto itt = instructions.find (func);
		string instr = (args [0]->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		return ASTnode (new ASTnumbatInstr (instr, std::vector <ASTnode> ({args [0]}), args [0]));
	} else {
		return ASTnode (new ASTerror ("No suitable function found"));
	}
	
}


ASTnode parseArrayDecleration (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	std::vector <ASTnode> dimentions;
	switch (args.size ()) {
		case 2:
			dimentions.push_back (ASTnode (new ASTnil ()));
			break;
		case 3: {
			Position pos = args [1];
			while (Position exp = nextArg (pos)) {
				if (exp) {
					dimentions.push_back (parseExpression (exp, scope));
				} else {
					dimentions.push_back (ASTnode (new ASTnil ()));
				}
				pos += exp;
			}
			break;
		}
		default:
			return ASTnode (new ASToperatorError ("Array decelerations require 2 or 3 arguments"));
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
	
	ASTnode type (new ASTtype (typeNode->isAlias (), typeNode->isConst (), getArrayType (scope, typeNode->getType (), dimentions.size ())));
	
	bool nil = false;
	for (ASTnode & node : dimentions) {
		if (node->isNil ()) {
			nil = true;
			break;
		}
	}
	
	ASTnode init = nullptr;
	if (!nil and !dimentions.empty ()) {
		std::list <ASTnode> args;
		args.push_back (dimentions [0]);
		for (size_t i=1; i<dimentions.size (); ++i) {
			args.push_back (ASTnode (new ASTnumbatInstr ("mul", {args.back (), dimentions [i]}, dimentions [0])));
		}
		args.push_front (ASTnode (new ASTallocate (args.back (), type->getType ())));
		init = ASTnode (new ASTtuple (args));
	}
	NumbatVariable * var = createVariable (scope, type, init, iden, false, false);
	if (!var) {
		return generateError (args.back (), "'" + iden + "' already declared in this scope");
	}
	return ASTnode (new ASTvariable (var));
	
}

ASTnode parseAssignmentOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	//TODO: type inference
	//TODO: initialisation of variables
	if (args.size () != 2) return ASTnode (new ASToperatorError ("Assignment operators require exactly two arguments"));
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
		std::cout << typeid (*lhs.get ()).name () << " " << lhs->getType () << " " << lhs->toString () << std::endl;
		std::cout << typeid (*rhs.get ()).name () << " " << rhs->getType () << " " << rhs->toString () << std::endl;
		return ASTnode (new ASTtuple (std::list <ASTnode> {lhs, rhs}));
	}
	
	std::vector <FunctionDecleration *> candidates = lhs->getType ()->getMethods (func);
	auto callable = findBestMatch (std::vector <ASTnode> {lhs, rhs}, candidates);
	if (callable->isValid ()) {
		return ASTnode (new ASTcall (callable, createStaticCast (std::vector <ASTnode> {lhs, rhs}, callable->getFunction ()->getType ())));
	} else {
		return ASTnode (new ASTmemcpy (lhs, createStaticCast (rhs, lhs)));
	}
	
}

ASTnode parseBinary (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError ("Binary operators require exactly two arguments"));
	
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
		if (lhs->getType ()) std::cout << lhs->getType ()->toString () << std::endl;
		if (rhs->getType ()) std::cout << rhs->getType ()->toString () << std::endl;
		std::cout << typeid (*lhs.get ()).name () << " " << lhs->toString () << std::endl;
		std::cout << typeid (*rhs.get ()).name () << " " << rhs->toString () << std::endl;
		std::cout << std::boolalpha << lhs->isValid () << std::endl;
		std::cout << std::boolalpha << rhs->isValid () << std::endl;
		std::cout << func << std::endl;
		if (ASTcallindex * index = dynamic_cast <ASTcallindex *> (lhs.get ())) {
			std::cout << typeid (*(index->getCall ())).name () << " " << index->getCall ()->toString () << std::endl;
			if (ASTcall * call = dynamic_cast <ASTcall *> (index->getCall ())) {
				std::cout << typeid (*(call->getCallee ().get ())).name () << " " << call->getCallee ()->toString () << std::endl;
			}
		}
		if (ASTcallindex * index = dynamic_cast <ASTcallindex *> (rhs.get ())) {
			std::cout << typeid (*(index->getCall ())).name () << index->getCall ()->isValid () << std::endl;
		}
		return ASTnode (new ASTtuple (std::list <ASTnode> {lhs, rhs}));
	}
	
	std::vector <FunctionDecleration *> candidates = lhs->getType ()->getMethods (func), rhsCand = rhs->getType ()->getMethods (func);
	std::copy (rhsCand.begin (), rhsCand.end (), std::back_inserter (candidates));
	auto callable = findBestMatch (std::vector <ASTnode> {lhs, rhs}, candidates);
	
	std::cout << callable->toString () <<std::endl;
	if (callable->isValid ()) {
		giveNode (scope, callable);
		return callable->getList ().front ();
	} else if (defImp) {
		return defImp (scope, func , std::vector <ASTnode> ({lhs, rhs}));
	} else {
		return ASTnode (new ASTerror ("Something went wrong"));
	}
	
}

ASTnode parseBlockOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	switch (args.size ()) {
		case 0:
			return ASTnode (new ASTnil);
		case 1:
			std::cout << '\n';
			for (tkitt itt = args[0].itt; itt != args[0].end; ++itt)
				std::cout << itt->iden;
			std::cout << std::endl;
			return parseBody (args [0], scope);
		default:
			return ASTnode (new ASTerror ("Something went wrong"));
	}
	
}

ASTnode parseCall (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	std::cout << "CALL: ";
	for (auto arg : args) {
		for (auto itt = arg.itt; itt != arg.end; ++itt) {
			std::cout << "'" << itt->iden << "'";
		}
	}
	std::cout << std::endl;
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
				return ASTnode (new ASTerror ("Something went wrong"));
			}
			return findBestMatch (params, candidates);
		default:
			return ASTnode (new ASTerror ("Something went wrong"));
	}
	
}

ASTnode parseComma (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError ("Comma operator requires exactly two arguments"));
	
	ASTnode lhs = parseExpression (args [0], scope, *matches);
	ASTnode rhs = parseExpression (args [1], scope, *matches);
	
	return ASTnode (new ASTtuple (std::list <ASTnode> {lhs, rhs}));
	
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
					return ASTnode (new ASTgep (exp, params[0]));
				} else {
					candidates = exp->getType ()->getMethods (func);
					callable = findBestMatch (params, candidates);
					if (callable->isValid ()) {
						return ASTnode (new ASTcall (callable, createStaticCast (params, callable->getFunction ()->getType ())));
					} else {
						return ASTnode (new ASTerror ("TODO: print candidates for functions in parseCall"));
					}
				}
			} else {
				return generateError (args [0], "Expected a variable");
			}
		default:
			return ASTnode (new ASTerror ("Something went wrong"));
	}
	
}

ASTnode parseRedirectOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	return ASTnode (new ASTerror ("Redirection is not yet implemented"));
	
}

ASTnode parseReferenceOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if  (args.size () != 2)return ASTnode (new ASToperatorError ("Reference operator requires exactly two arguments"));
	
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
		*static_cast <ASTtype *> (ret.get ()) = ASTtype (ref, constTkn, ret->getType ());
	}
	
	if (++pos) {
		if (pos.itt->type == lexer::TOKEN::identifier) {
			ret = ASTnode (new ASTvariable (createVariable (scope, ret, nullptr, pos.itt->iden, false, false)));
		} else {
			ret = generateOperatorError (pos, "Identifier expected");
		}
		++pos;
	}
	
	return ret;
	
}

ASTnode parseSubExpression (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	switch (args.size ()) {
		case 0:
			return ASTnode (new ASTnil);
		case 1:
			return ASTnode (new ASTsubexp (parseExpression (args [0], scope)));
		default:
			return ASTnode (new ASTerror ("Something went wrong"));
	}
	
}

ASTnode parseUnary (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 1) return ASTnode (new ASToperatorError ("Unary operators require exactly one argument"));
	ASTnode arg = parseExpression (args [0], scope, *matches);
	
	if (!arg->getType ()) return arg;
	
	std::vector <FunctionDecleration *> candidates = arg->getType ()->getMethods (func);
	auto callable = findBestMatch ({arg}, candidates);
	
	if (callable->isValid ()) {
		return ASTnode (new ASTcall (callable, createStaticCast ({arg}, callable->getFunction ()->getType ())));
	} else if (defImp) {
		return defImp (scope, func , {arg});
	} else {
		return ASTnode (new ASTerror ("Something went wrong"));
	}
	
}

ASTnode parseWhileLoop (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp) {
	
	if (args.size () != 2) return ASTnode (new ASToperatorError ("While loops require a condition and a body"));
	NumbatScope * child = createChild (scope);
	ASTnode cond = parseExpression (args.front (), child);
	ASTnode body = parseExpression (args.back (), child);
	return ASTnode (new ASTwhileloop (cond, body));
	
}


};
};