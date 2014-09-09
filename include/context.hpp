#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "ast/operatordecleration.hpp"

#include <map>
#include <unordered_map>

namespace numbat {
namespace parser {


class NumbatScope;
class Position;

struct ParsingContext {
	
	typedef ASTnode(*ExpressionParser)(Position pos, NumbatScope * scope);
	string path, file;
	
	std::map <string, shared_ptr <OperatorDecleration>> operators;
	std::map <string, string> blocks;
	
	std::multimap <string, shared_ptr <OperatorDecleration>> operatorsByFirstToken;
	
	std::unordered_map <string, ExpressionParser> expressionParsers;
};


};
};


#endif /*CONTEXT_HPP*/