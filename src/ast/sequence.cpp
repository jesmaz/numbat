#include <ast/sequence.hpp>


namespace AST {


string Sequence::toString (text::PrintMode mode) const {
	
	string s = "{";
	for (auto & n : nodes) {
		s += n->toString (mode) + "; ";
	}
	s.back () = '}';
	return s;
	
}


}
