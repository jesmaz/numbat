#include "../../../include/parse/tree/literal.hpp"

string ParseTreeLiteral::strDump (text::PrintMode mode) {
	return mode & text::COLOUR ? text::yel + literal + text::reset : literal;
}