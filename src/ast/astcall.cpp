#include "../include/ast/astcall.hpp"

namespace numbat {
namespace parser {


bool ASTcall::isValid () const {
	
	bool b=callee->isValid ();
	for (const ASTnode & n : args) {
		b &= n->isValid ();
	}
	return b;
	
}

string ASTcall::toString (const string & indent) const {
	
	string ret = indent + callee->toString () + "(";
	for (const ASTnode & arg : args) {
		ret += arg->toString () + ", ";
	}
	return ret + ")";
	
}


}
}