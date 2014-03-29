#include "../include/core.hpp"

namespace numbat {
namespace parser {


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
	tkitt colon;
	ASTnode node = nullptr;
	if ((colon = ast->findToken (":", end)) != end) {
		ast->nextToken (colon);
		if (ast->itt != colon) {
			node = ast->parseStatment (colon);
		}
		if (!node) {
			//TODO: handle a lack of a condition (infinite loop?)
			return node;
		}
		if (node->isCallable ()) {
			shared_ptr <ASTcallable> call = std::dynamic_pointer_cast <ASTcallable> (node);
			node = ASTnode (new ASTcallindex (call, 0));
		}
		ast->nextToken (end);
		node = ASTnode (new ASTwhileloop (node, ast->parseBody (end)));
	} else {
		ast->itt = end;
	}
	return node;
}


ASTnode parseElementReferenceOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	ASTnode lhs = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	ASTnode ret = ast->resolveSymbol (ast->itt->iden, lhs);
	ast->nextToken (end);
	return ret;
	
}

ASTnode parseGenericBinary (AbstractSyntaxTree * ast, const string & func, const std::vector< tkitt > & oppLoc, std::list< OperatorDecleration::OperatorMatch > & matches, tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> lhsMatches;
	splitListAboutTkn (lhsMatches, matches, oppLoc [0]);
	ASTnode lhs = ast->parseExpression (lhsMatches, oppLoc [0]);
	ast->itt = oppLoc [0];
	ast->nextToken (end);
	ASTnode rhs = ast->parseExpression (matches, end);
	return ast->createBinaryCall (func, lhs, rhs, end);
	
}

ASTnode parseGenericIndexCall (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) {
	
	std::vector <ASTnode> args (1, ast->parseExpression (matches, oppLoc [0]));
	ast->itt = oppLoc [0];
	ast->nextToken (oppLoc [1]);
	auto params = ast->parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
	ast->itt = oppLoc [1];
	args.insert (args.end (), params.begin (), params.end ());
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
	return ASTnode (new ASTnumbatInstr ("redir", args));
	
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