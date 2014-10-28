#include "../../include/ast/asttuplecall.hpp"


namespace numbat {
namespace parser {


bool ASTtuplecall::isParsed () const {
	
	for (const shared_ptr <ASTcallable> & call : calls) {
		if (!call->isParsed ()) return false;
	}
	for (const ASTnode & node : lhsArgs) {
		if (!node->isParsed ()) return false;
	}
	for (const ASTnode & node : rhsArgs) {
		if (!node->isParsed ()) return false;
	}
	return true;
	
}


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

size_t ASTtuplecall::calculateWeight () const {
	
	size_t weight = 0;
	for (const shared_ptr <ASTcallable> & call : calls) {
		weight += call->calculateWeight ();
	}
	for (const ASTnode & node : lhsArgs) {
		weight += node->calculateWeight ();
	}
	for (const ASTnode & node : rhsArgs) {
		weight += node->calculateWeight ();
	}
	return weight;
	
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
