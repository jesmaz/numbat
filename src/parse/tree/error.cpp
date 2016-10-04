#include <parse/tree/error.hpp>


namespace parser {


string ParseTreeError::strDump (text::PrintMode mode) {
	string s = "error:" + std::to_string (getPos ().line) + ":" + std::to_string (getPos ().col) + ": " + err;
	if (mode & text::COLOUR) {
		return text::red + s + text::reset;
	} else {
		return s;
	}
}


}
