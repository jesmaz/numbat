#ifndef CONTEXT_HPP
#define CONTEXT_HPP


#include "position.hpp"

namespace numbat {
namespace parser {


struct ParsingContext {
	
	typedef ASTnode(*parseStatment)(ParsingContext & context, Position pos, NumbatScope * scope);
	string path, file;
	
	std::map <string, shared_ptr <OperatorDecleration>> operators;
	std::map <string, string> blocks;
	
	std::multimap <string, shared_ptr <OperatorDecleration>> operatorsByFirstToken;
	
	//std::set <shared_ptr <OperatorDecleration>, std::greater <shared_ptr <OperatorDecleration>>> precidenceOrderedOperators;
	
	std::unordered_map <string, parseStatment> statementParsers;
	
	//std::unordered_set <string> parenOpperators, oppTokens, ternaryStart;
};


};
};


#endif /*CONTEXT_HPP*/