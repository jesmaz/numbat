#include <ast/memory.hpp>


namespace AST {


string RawInit::toString (text::PrintMode mode) const {
	if (args.empty ()) {
		return var->toString (mode) + ": {}";
	} else {
		string s = var->toString (mode) + ": {";
		for (auto & a : args) {
			s += a->toString (mode) + ", ";
		}
		s.pop_back ();
		s.back () = '}';
		return s;
	}
}


}