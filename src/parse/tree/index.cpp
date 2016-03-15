#include "../../../include/parse/tree/index.hpp"

string ParseTreeIndex::strDump (text::PrintMode mode) {
	string s = "(" + index->toString (mode) + (text::PrintMode::COLOUR & mode ? text::red + " [" + text::reset : " [");
	if (!args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1; i<args.size (); ++i) s += args [i]->toString (mode);
	}
	return s + (text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]") + ")";
}