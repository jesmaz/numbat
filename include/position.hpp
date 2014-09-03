#ifndef POSITION_HPP
#define POSITION_HPP

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
	const Position & operator ++ () {++itt; return this->allign ();}
	const Position operator ++ (int) {tkitt tmp = itt; ++itt; allign (); return Position (tmp, end);}
	const Position & operator += (const Position & rhs) {return *this = Position (rhs.end + 1, end).allign ();}
	Position (tkitt itt) : Position (itt, end) {}
	Position (tkitt itt, tkitt end) : itt (itt), end (end) {allign ();}
};

Position nextBody (const Position & pos);
Position nextExpression (const Position & pos);
Position nextIden (const Position & pos);
Position nextGroup (const Position & pos, const string & beg, const string & end);
Position skipBody (const Position & pos);

tkitt findToken (tkitt begin, tkitt end, lexer::TOKEN token, const string & str="");


};
};


#endif /*POSITION_HPP*/