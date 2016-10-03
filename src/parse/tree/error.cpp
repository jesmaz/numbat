#include "../../../include/parse/tree/error.hpp"

string ParseTreeError::strDump (text::PrintMode mode) {
	string s = "error:" + std::to_string (getLine ()) + ":" + std::to_string (getPos ()) + ": " + err;
	if (mode & text::COLOUR) {
		return text::red + s + text::reset;
	} else {
		return s;
	}
}