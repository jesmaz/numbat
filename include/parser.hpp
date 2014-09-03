#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "position.hpp"

namespace numbat {
namespace parser {


struct ParsingContext {
	string path, file;
	
	std::map <string, shared_ptr <OperatorDecleration>> operators;
	
	std::multimap <string, shared_ptr <OperatorDecleration>> operatorsByFirstToken;
	
	std::set <shared_ptr <OperatorDecleration>, std::greater <shared_ptr <OperatorDecleration>>> precidenceOrderedOperators;
	
	std::unordered_map <string, ASTnode(*)(AbstractSyntaxTree *, tkitt)> statementParsers;
	
	std::unordered_set <string> parenOpperators, oppTokens, ternaryStart;
};

void parseBody (Parser parser, NumbatScope * body);


};
};

#endif