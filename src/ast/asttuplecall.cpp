#include "../../include/ast/asttuplecall.hpp"


namespace numbat {
namespace parser {


bool ASTtuplecall::isValid () const {
	
	for (const shared_ptr <ASTcallable> & call : calls) {
		if (!call->isValid ()) return false;
	}
	for (const ASTnode & node : lhsArgs) {
		if (!node->isValid ()) return false;
	}
	for (const ASTnode & node : rhsArgs) {
		if (!node->isValid ()) return false;
	}
	return true;
	
}

string ASTtuplecall::toString (const string & indent) const {
	
	string ret = indent + '(';
	for (const ASTnode & node : lhsArgs) {
		ret += node->toString () + ", ";
	}
	ret += iden + " ";
	for (const ASTnode & node : rhsArgs) {
		ret += node->toString () + ", ";
	}
	return ret + ')';
	
}


}
}
