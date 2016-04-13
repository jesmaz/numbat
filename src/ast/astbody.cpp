#include "../../include/ast/control/astbody.hpp"

namespace numbat {
namespace parser {


bool ASTbody::isParsed () const {

	bool b=true;
	for (const ASTnode & n : body) {
		if (n) {
			b &= n->isParsed ();
		} else {
			return false;
		}
	}
	return b;
	
}

bool ASTbody::isValid () const {

	bool b=true;
	for (const ASTnode & n : body) {
		if (n) {
			b &= n->isValid ();
		} else {
			return false;
		}
	}
	return b;
	
}

size_t ASTbody::calculateWeight () const {
	
	size_t sum=0;
	for (const ASTnode & n : body) {
		sum += n->calculateWeight ();
	}
	return sum;
	
}

string ASTbody::toString (const string & indent) const {
	
	string str;
	string inden = indent + '\t';
	for (ASTnode n : body) {
		if (n)
			str += n->toString (inden) + '\n';
		else
			str += "null\n";
	}
	return str;
	
}


}
}