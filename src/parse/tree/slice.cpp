#include "../../../include/parse/tree/slice.hpp"

string ParseTreeSlice::strDump (text::PrintMode mode) {
	if (args.size ()) {
		string s = text::PrintMode::COLOUR & mode ? text::red + "[" + text::reset : "[";
		string del = " ";//text::PrintMode::COLOUR & mode ? text::yel + " : " + text::reset : " : ";
		for (size_t i=0; i+1<args.size (); ++i) s += args [i]->toString (mode) + del;
		return s + args.back ()->toString (mode) + (text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]");
	} else {
		return text::PrintMode::COLOUR & mode ? "[]" : "[]";
	}
}