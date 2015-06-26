#include "../../../include/parse/tree/list.hpp"

string ParseTreeList::strDump (text::PrintMode mode) {
	if (args.size ()) {
		string s = "(";
		for (size_t i=0; i+1<args.size (); ++i) s += args [i]->toString (mode) + " , ";
		return s + args.back ()->toString (mode) + ")";
	} else {
		return "()";
	}
}