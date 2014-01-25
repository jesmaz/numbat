#include "../include/ast/asttuple.hpp"

namespace numbat {
namespace parser {


bool ASTtuple::isValid () const {

	bool b=true;
	for (const ASTnode & n : elements) {
		b &= n->isValid ();
	}
	return b;
	
}

string ASTtuple::toString (const string & indent) const {
	
	string str;
	string inden = indent + '{';
	for (ASTnode n : elements) {
		if (n)
			str += n->toString (inden) + ", ";
		else
			str += "null, ";
	}
	return str + "}";
	
}


}
}