#include "../../include/ast/type/asttuple.hpp"

namespace numbat {
namespace parser {


bool ASTtuple::isParsed () const {

	bool b=true;
	for (const ASTnode & n : elements) {
		b &= n->isParsed ();
	}
	return b;
	
}

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
	
	string str = indent + '{';
	for (ASTnode n : elements) {
		if (n)
			str += n->toString () + ", ";
		else
			str += "null, ";
	}
	return str + "}";
	
}


}
}