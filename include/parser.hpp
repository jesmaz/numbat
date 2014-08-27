#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"

namespace numbat {
namespace parser {


struct Position {
	tkitt itt, end;
	operator bool () const {return itt < end;}
	friend const bool operator < (const Position & lhs, const Position & rhs) {return lhs.itt < rhs.itt;}
	friend const bool operator > (const Position & lhs, const Position & rhs) {return lhs.itt > rhs.itt;}
	friend const bool operator <= (const Position & lhs, const Position & rhs) {return lhs.itt <= rhs.itt;}
	friend const bool operator >= (const Position & lhs, const Position & rhs) {return lhs.itt >= rhs.itt;}
	friend const bool operator != (const Position & lhs, const Position & rhs) {return lhs.itt != rhs.itt;}
	friend const bool operator == (const Position & lhs, const Position & rhs) {return lhs.itt == rhs.itt;}
	const Position & allign () {while ((itt->type == lexer::TOKEN::indent or itt->type == lexer::TOKEN::whitespace) and itt<end) ++itt; return *this;}
	template <typename T>
	const Position operator + (T num) const {return Position (itt + num, end).allign ();}
	const Position operator + (const Position & rhs) const {return Position (rhs.end + 1, end).allign ();}
	const Position operator ++ () {return Position (itt ++, end).allign ();}
	const Position & operator ++ (int) {++itt; return this->allign ();}
	const Position & operator += (const Position & rhs) {return *this = Position (rhs.end + 1, end).allign ();}
	Position (tkitt itt) : Position (itt, end) {}
	Position (tkitt itt, tkitt end) : itt (itt), end (end) {allign ();}
};

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