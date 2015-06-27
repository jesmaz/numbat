#include "../../../include/parse/tree/keyword.hpp"

string ParseTreeKeyword::strDump (text::PrintMode mode) {
	return mode&text::COLOUR ? text::blu + sym + text::reset : sym;
}