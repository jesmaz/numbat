#include <ast/sequence.hpp>


namespace AST {

string Return::toString (text::PrintMode mode) const {
	return "(return " + retVal->toString (mode) + ")";
}

string Sequence::toString (text::PrintMode mode) const {
	
	string s = "{";
	for (auto & n : nodes) {
		s += n->toString (mode) + "; ";
	}
	s.back () = '}';
	return s;
	
}


}
