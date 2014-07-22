#include "../include/core.hpp"

namespace numbat {
namespace parser {


const std::map <string, string> instructions = [] {
	std::map <string, string> mp;
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


ASTnode defArithmetic (AbstractSyntaxTree * ast, const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end) {
	
	if (lhs->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (lhs->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		
		ASTnode trhs = rhs;
		if (lhs->getType () != rhs->getType ()) {
			trhs = ast->createStaticCast (std::vector <ASTnode> (1, rhs), std::vector <ASTnode> (1, lhs), 1) [0];
		}
		std::vector <ASTnode> args (2);
		args [0] = lhs;
		args [1] = trhs;
		return ASTnode (new ASTnumbatInstr (instr, args, lhs));
		
	} else {
		return ASTnode (new ASTerror ("No suitable function found"));
	}
	
}

ASTnode defAssign (AbstractSyntaxTree * ast, const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end) {
	
	ASTnode trhs = rhs;
	if (lhs->getType () != rhs->getType ()) {
		trhs = ast->createStaticCast (std::vector <ASTnode> (1, rhs), std::vector <ASTnode> (1, lhs), 1) [0];
	}
	
	if (lhs->getType ()->isArray () and rhs->getType ()->isArray ()) {
		
		//TODO: Complex types, memory allocation
		std::vector <ASTnode> args (2);
		ASTnode compare = defCompare (ast, " < ", ASTbase::getLength (lhs), ASTbase::getLength (rhs), end);
		args [0] = lhs;
		args [1] = ASTnode (new ASTallocate (ASTbase::getLength (rhs), rhs->getType ()));
		ASTnode mov (new ASTnumbatInstr ("mov", args, lhs));
		args [0] = ASTbase::getLength (lhs);
		args [1] = ASTbase::getLength (rhs);
		ASTnode smv (new ASTnumbatInstr ("mov", args, ASTbase::getLength (rhs)));
		args [0] = mov;
		args [1] = smv;
		ASTnode branch (new ASTbranch (compare, ASTnode (new ASTbody (args))));
		args [0] = branch;
		args [1] = ASTnode (new ASTmemcpy (lhs, trhs));
		return ASTnode (new ASTbody (args));
		
	} else {
		//TODO: Complex types
		return ASTnode (new ASTmemcpy (lhs, trhs));
	}
	
}

ASTnode defCompare (AbstractSyntaxTree * ast, const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end) {
	
	if (lhs->getType ()->isRaw ()) {
		
		auto itt = instructions.find (func);
		string instr = (lhs->getType ()->isFloat () ? "f" : "") + (itt != instructions.end () ? itt->second : "");
		
		ASTnode trhs = rhs;
		if (lhs->getType () != rhs->getType ()) {
			trhs = ast->createStaticCast (std::vector <ASTnode> (1, rhs), std::vector <ASTnode> (1, lhs), 1) [0];
		}
		std::vector <ASTnode> args (2);
		args [0] = lhs;
		args [1] = trhs;
		auto typ = ast->types.find ("bool");
		return ASTnode (new ASTnumbatInstr (instr, args, ASTnode (new ASTtype (false, true, typ->second))));
		
	} else {
		return ASTnode (new ASTerror ("No suitable function found"));
	}
	
}


ASTnode parseExpression (AbstractSyntaxTree * ast, tkitt end) {
	tkitt scolon;
	ASTnode exp = nullptr;
	if ((scolon = ast->findToken (";", end)) != end) {
		exp = ast->parseStatment (scolon);
		ast->eatSemicolon (end);
	} else {
		ast->itt = end;
	}
	return exp;
}

ASTnode parseWhileLoop (AbstractSyntaxTree * ast, tkitt end) {
	ast->nextToken (end);  // Eat 'while' token
	ASTnode node = nullptr;
	if (ast->itt->iden == "(") {
		ast->nextToken (end);
		tkitt cend = ast->findToken (")", end);
		node = ast->parseStatment (cend);
		ast->itt = cend;
		ast->nextToken (end);
	} else {
		return ASTnode (new ASTerror ("Expected '('"));
	}
	node = ASTnode (new ASTwhileloop (node, ast->parseBody (end)));
	return node;
}


ASTnode parseArithmeticOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	return parseBinary (ast, func, oppLoc, matches, end, defArithmetic);
}

ASTnode parseArrayDecleration (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	ASTnode dataType = ast->parseExpression (oppLoc [0]);
	if (!dataType->isValid ()) {
		ast->error (dataType->toString (), end);
		return dataType;
	}
	
	ast->itt = oppLoc [0];
	ast->nextToken (oppLoc [1]);
	auto dimentions = ast->parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
	
	if (1 < dimentions.size ()) {
		return ASTnode (new ASTerror ("Multidimensional arrays are not yet supported"));
	}
	
	ast->itt = oppLoc [1];
	ast->nextToken (end);
	const string & iden = ast->itt->iden;
	ast->nextToken (end);
	
	if (ast->itt != end) {
		return ASTnode (new ASTerror ("Unexpected token: '" + ast->itt->iden + "'"));
	} else {
		
		ASTnode type = ast->createArrayType (dataType, dimentions.size ());
		ASTnode var (new ASTvariable (ast->variables [iden] = std::shared_ptr <NumbatVariable> (new NumbatVariable (type, iden))));
		ASTnode size;
		
		bool zero = false;
		for (const ASTnode & n : dimentions) {
			if (ASTconstantInt * ci = dynamic_cast <ASTconstantInt *> (n.get ())) {
				zero |= !ci->getValue ();
			}
		}
		
		std::vector <ASTnode> bodyvec (2);
		
		if (dimentions.empty ()) {
			return var;
		} else if (dimentions.size () == 1) {
			size = dimentions [0];
		} else {
			size = ASTnode (new ASTnumbatInstr ("mul", dimentions, dimentions [0]));
		}
		
		if (zero) {
			return var;
		} else {
			
			ASTnode alloc (new ASTallocate (size, type->getType ()));
			std::vector <ASTnode> movvec (2);
			movvec [0] = var;
			movvec [1] = alloc;
			
			bodyvec [0] = ASTnode (new ASTnumbatInstr ("mov", movvec, var));
			
		}
		
		std::vector <ASTnode> movvec (2);
		movvec [0] = ASTnode (new ASTstructIndex (0, var));;
		movvec [1] = size;
		bodyvec [1] = ASTnode (new ASTnumbatInstr ("mov", movvec, size));
		
		return ASTnode (new ASTbody (bodyvec));
		
	}
}

ASTnode parseAssignmentOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	return parseBinary (ast, func, oppLoc, matches, end, defAssign);
	
}

ASTnode parseBinary (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end, defBinaryImp defImpl) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	ASTnode lhs = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	ASTnode rhs = ast->parseExpression (matches, end);
	return ast->createBinaryCall (func, lhs, rhs, end, defImpl);
	
}

ASTnode parseComparisonOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	return parseBinary (ast, func, oppLoc, matches, end, defCompare);
}

ASTnode parseElementReferenceOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	bool constTkn=false, ref=false;
	while (ast->itt->type == lexer::TOKEN::typemodifier) {
		if (ast->itt->iden == "ref") {
			ref = true;
		} else if (ast->itt->iden == "const") {
			constTkn = true;
		}
		ast->nextToken (end);
	}
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	ASTnode lhs = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	ASTnode ret = ast->resolveSymbol (ast->itt->iden, lhs);
	if (!ret->isValid ()) {
		ast->error (ret->toString (), end);
	}
	ast->nextToken (end);
	
	if (ASTtype * type = dynamic_cast <ASTtype *> (ret.get ())) {
		*type = ASTtype (ref, constTkn, type->getType ());
	}
	
	if (ast->itt != end) {
		if (ast->itt->type == lexer::TOKEN::identifier) {
			ret = ASTnode (new ASTvariable (ast->variables [ast->itt->iden] = std::shared_ptr <NumbatVariable> (new NumbatVariable (ret, ast->itt->iden))));
		} else {
			ret = ASTnode (new ASTerror ("Identifier expected"));
		}
		ast->nextToken (end);
	}
	
	return ret;
	
}

ASTnode parseFunctionCall (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	ASTnode lhs = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	std::vector <ASTnode> params;
	if (oppLoc.size () == 2) {
		ast->nextToken (oppLoc [1]);
		params = ast->parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
		ast->itt = oppLoc [1];
	} else {
		ast->nextToken (end);
	}
	ast->nextToken (end);
	
	if (ast->itt != end) {
		return ASTnode (new ASTerror ("Unexpected token: '" + ast->itt->iden + "'"));
	}
	
	shared_ptr <ASTfunctionlist> flist = std::dynamic_pointer_cast <ASTfunctionlist> (lhs);
	ASTnode ret;
	
	if (flist) {
		
		ret = findBestMatch (ast, params, flist->getElements ());
		
		if (!ret->isValid ()) {
			ast->error ("No suitable function found", end);
		}
		
	} else if (ASTtype * type = dynamic_cast <ASTtype *> (lhs.get ())) {
		
		ret = findBestMatch (ast, params, type->getType ()->getConstructors ());
		if (!ret->isValid ()) {
			ast->error ("No suitable constructor found", end);
		}
		
	} else {
		//function object
		//TODO: function objects
		ret = ASTnode (new ASTerror ("Function objects are not yet implemented"));
		ast->error ("Function objects are not yet implemented", end);
	}
	
	return ret;
	
}

ASTnode parseGenericArray (AbstractSyntaxTree * ast, const string & func, const std::vector< tkitt > & oppLoc, std::list< OperatorDecleration::OperatorMatch > & matches, tkitt end) {
	
	ast->itt = oppLoc [0];
	ast->nextToken (oppLoc [1]);
	auto args = ast->parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
	return ast->createCallNode (ast->findFunction (func, args), args);
	
}

ASTnode parseGenericBinary (AbstractSyntaxTree * ast, const string & func, const std::vector< tkitt > & oppLoc, std::list< OperatorDecleration::OperatorMatch > & matches, tkitt end) {
	
	return parseBinary (ast, func, oppLoc, matches, end, nullptr);
	
}

ASTnode parseGenericIndexCall (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	std::vector <ASTnode> args (1, ast->parseExpression (lhsMatches, oppLoc [0]));
	ast->itt = oppLoc [0];
	ast->nextToken (oppLoc [1]);
	auto params = ast->parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
	ast->itt = oppLoc [1];
	ast->nextToken (end);
	
	if (!args [0]->getType ()) {
		return ASTnode (new ASTerror ("Invalid type"));
	}
	
	if (args [0]->isArray ()) {
		return ASTnode (new ASTgep (args [0], params [0]));
	}
	
	args.insert (args.end (), params.begin (), params.end ());
	if (ast->itt != end) {
		return ASTnode (new ASTerror ("Unexpected token: '" + ast->itt->iden + "'"));
	} else {
		return ast->createCallNode (ast->findFunction (func, args), args);
	}
	
}

ASTnode parseGenericTernary (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end){
	
	std::vector <ASTnode> args (3);
	std::list <OperatorDecleration::OperatorMatch> lhs, mid;
	splitListAboutTkn (lhs, matches, oppLoc [0]);
	args [0] = ast->parseExpression (lhs, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	splitListAboutTkn (mid, matches, oppLoc [1]);
	args [1] = ast->parseExpression (mid, oppLoc [1]);
	ast->itt = oppLoc [1];
	ast->nextToken (end);
	args [2] = ast->parseExpression (matches, end);
	return ast->createCallNode (ast->findFunction (func, args), args);
	
}

ASTnode parseGenericUnaryPrefix (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	std::vector <ASTnode> args (1, ast->parseExpression (matches, end));
	return ast->createCallNode (ast->findFunction (func, args), args);
	
}

ASTnode parseGenericUnaryPostfix (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	std::vector <ASTnode> args (1, ast->parseExpression (matches, oppLoc [0]));
	return ast->createCallNode (ast->findFunction (func, args), args);
	
}

ASTnode parseRedirectOperator (AbstractSyntaxTree * ast, const string & func, const std::vector< tkitt > & oppLoc, std::list< OperatorDecleration::OperatorMatch > & matches, tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	std::vector <ASTnode> args (2);
	args [0] = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	args [1] = ast->parseExpression (matches, end);
	return ASTnode (new ASTnumbatInstr ("redir", args, args [0]));
	
}

ASTnode parseTupleOperator (AbstractSyntaxTree * ast, const string & func, const std::vector< tkitt > & oppLoc, std::list< OperatorDecleration::OperatorMatch > & matches, tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	ASTnode lhs = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	ASTnode rhs = ast->parseExpression (matches, end); 
	return ast->createTuple (lhs, rhs);
	
}


shared_ptr <ASTcallable> findBestMatch (AbstractSyntaxTree * ast, const std::vector <ASTnode> & args, const std::vector <shared_ptr <FunctionDecleration>> & candidates, int maxDepth) {
	
	shared_ptr <FunctionDecleration> func = nullptr;
	std::vector <ASTnode> params (args);
	size_t weight = __UINT64_MAX__ - 1;
	for (auto & fdef : candidates) {
		if (fdef->getArgs ().size () == args.size ()) {
			std::vector <ASTnode> cast = ast->createStaticCast (args, fdef->getArgs (), maxDepth);
			size_t w = 0;
			for (const ASTnode & node : cast) {
				if (node->isValid ()) {
					w += node->calculateWeight ();
				} else {
					w = __UINT64_MAX__;
					break;
				}
			}
			if (w < weight) {
				weight = w;
				func = fdef;
				params = cast;
			}	
		}
	}
	if (func) {
		return shared_ptr <ASTcallable> (new ASTcall (shared_ptr <ASTcallable> (new ASTfunctionPointer (func)), params));
	} else {
		return shared_ptr <ASTcallable> (new ASTcallerror ("No match for function"));
	}
	
}

};
};