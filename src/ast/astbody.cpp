#include "../include/ast/astbody.hpp"

namespace numbat {
namespace parser {


bool ASTbody::isValid () const {

	bool b=true;
	for (const ASTnode & n : body) {
		b &= n->isValid ();
	}
	return b;
	
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