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


};
};