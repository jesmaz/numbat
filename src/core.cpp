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
		//return ASTnode (new ASTerror ("Type mismatch: (" + lhs->toString () + ", " + rhs->toString () + ") (todo: sutible type conversions)"));
	}
	
	if (lhs->getType ()->isArray ()) {
		
		ASTnode length = ASTnode (new ASTstructIndex (0, rhs));
		ASTnode type = ASTnode (new ASTtype (false, false, length->getType ()));
		ASTnode index = ASTnode (new ASTvariable (std::shared_ptr <NumbatVariable> (new NumbatVariable (length->getASTType (), "index"))));
		
		std::vector <ASTnode> args (2);
		args [0] = index;
		args [1] = length;
		ASTnode cond = ASTnode (new ASTnumbatInstr ("cmplt", args, ASTnode (new ASTtype (false, false, ast->generateRawType ("raw 1", 1, std::set <string> ())))));
		
		args [0] = index;
		args [1] = ASTnode (new ASTconstantInt (type, 1));
		ASTnode incr = ASTnode (new ASTnumbatInstr ("add", args, index));
		
		args [1] = ASTnode (new ASTconstantInt (type, 0));
		ASTnode zero = ASTnode (new ASTnumbatInstr ("mov", args, index));
		
		ASTnode geplhs = ASTnode (new ASTgep (lhs, index));
		ASTnode geprhs = ASTnode (new ASTgep (rhs, index));
		args [0] = ast->createBinaryCall (func, geplhs, geprhs, end, defAssign);
		args [1] = incr;
		ASTnode body = ASTnode (new ASTbody (args));
		ASTnode loop = ASTnode (new ASTwhileloop (cond, body));
		
		args [0] = lhs;
		args [1] = ASTnode (new ASTallocate (length, lhs->getType ()));
		ASTnode alloc = ASTnode (new ASTnumbatInstr ("mov", args, lhs));
		
		args [0] = alloc;
		args [1] = zero;
		args.push_back (loop);
		return ASTnode (new ASTbody (args));
		
	} else if (lhs->getType ()->isRaw ()) {
		std::vector <ASTnode> args (2);
		args [0] = lhs;
		args [1] = trhs;
		return ASTnode (new ASTnumbatInstr ("mov", args, lhs));
	} else {
		std::vector <ASTnode> instr;
		return ASTnode (new ASTerror ("NYI"));
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
		
		string key = dataType->toString () + " []";
		shared_ptr <NumbatType> nbtype;
		auto arrType = ast->types.find (key);
		
		if (arrType == ast->types.end ()) {
			nbtype = ast->types [key] = shared_ptr <NumbatType> (new NumbatPointerType (key, dataType));
			ASTnode type = ASTnode (new ASTtype (false, false, ast->generateRawType ("raw 64", 64, std::set <string> ())));
			ASTnode param = ASTnode (new ASTparamater (shared_ptr <NumbatVariable> (new NumbatVariable (type, "length"))));
			nbtype->buildData (std::vector <ASTnode> (1, param));
		} else {
			nbtype = arrType->second;
		}
		
		ASTnode type (new ASTtype (dataType->isAlias (), dataType->isConst (), nbtype));
		ASTnode var (new ASTvariable (ast->variables [iden] = std::shared_ptr <NumbatVariable> (new NumbatVariable (type, iden))));
		ASTnode size;
		
		bool zero = false;
		for (const ASTnode & n : dimentions) {
			if (ASTconstantInt * ci = dynamic_cast <ASTconstantInt *> (n.get ())) {
				zero |= !ci->getValue ();
			}
		}
		
		if (zero) {
			return var;
		}
		
		if (dimentions.empty ()) {
			return var;
		} else if (dimentions.size () == 1) {
			size = dimentions [0];
		} else {
			size = ASTnode (new ASTnumbatInstr ("mul", dimentions, dimentions [0]));
		}
		
		ASTnode alloc (new ASTallocate (size, type->getType ()));
		std::vector <ASTnode> movvec (2);
		movvec [0] = var;
		movvec [1] = alloc;
		return ASTnode (new ASTnumbatInstr ("mov", movvec, var));
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
	ast->nextToken (oppLoc [1]);
	auto params = ast->parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
	ast->itt = oppLoc [1];
	ast->nextToken (end);
	
	if (ast->itt != end) {
		return ASTnode (new ASTerror ("Unexpected token: '" + ast->itt->iden + "'"));
	}
	
	shared_ptr <ASTfunctionlist> flist = std::dynamic_pointer_cast <ASTfunctionlist> (lhs);
	ASTnode ret;
	std::cerr << "'" << lhs->getIden () << "'" << std::endl;
	
	if (flist) {
		
		std::cerr << flist->getElements ().size () << " matches" << std::endl;
		for (auto & fdef : flist->getElements ()) {
			
			std::cerr << fdef->getIden () << " (" << fdef->getArgs ().size () << ")" << std::endl;
			if (fdef->getArgs ().size () == params.size ()) {
				
				std::vector <ASTnode> cast = ast->createStaticCast (params, fdef->getArgs ());
				
				bool success = true;
				for (auto & arg : cast) {
					success &= arg->isValid ();
				}
				
				//TODO: Prioritize function conversions
				if (success) {
					ret = ASTnode (new ASTcall (shared_ptr <ASTcallable> (new ASTfunctionPointer (fdef)), cast));
					break;
				}
				
			}
			
		}
		
		if (!ret) {
			ret = ASTnode (new ASTerror ("No sutible function found"));
			ast->error ("No sutible function found", end);
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
	std::cerr << args [0]->getType ()->toString ("====> ") << std::endl;
	
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

};
};