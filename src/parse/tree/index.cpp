#include "../../../include/parse/tree/index.hpp"

string ParseTreeIndex::strDump (text::PrintMode mode) {
	string s = text::PrintMode::COLOUR & mode ? text::red + "[" + text::reset : "[";
	return s + (index ? index->toString (mode) : "") + (text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]");
}