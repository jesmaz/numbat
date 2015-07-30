#include "../../../include/parse/tree/operator.hpp"

string ParseTreeOperator::strDump (text::PrintMode mode) {
	if (iden.empty () and not args.empty ()) {
		string s = "(";
		for (size_t i=0; i+1<args.size (); ++i) s += args [i]->toString (mode) + " ";
		return s + args.back ()->toString (mode) + ")";
	} else if (not args.empty ()) {
		string s = "(";
		size_t i=0;
		for (char c : iden) {
			if (c == ' ') {
				if (isalnum (s.back ())) s += " ";
				assert (i < args.size ());
				s += args [i]->toString (mode) + " ";
				++i;
			} else {
				s.push_back (c);
				if (!isalnum (c)) s += " ";
			}
		}
		s.back () = ')';
		return s;
	} else {
		return "()";
	}
}