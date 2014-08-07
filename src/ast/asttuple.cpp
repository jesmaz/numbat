#include "../include/ast/type/asttuple.hpp"

namespace numbat {
namespace parser {


bool ASTtuple::isValid () const {

	bool b=true;
	for (const ASTnode & n : elements) {
		b &= n->isValid ();
	}
	return b;
	
}

size_t ASTtuple::calculateWeight () const {
	
	size_t weight = 0;
	for (const ASTnode & n : elements) {
		weight += n->calculateWeight( );
	}
	return weight;
	
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