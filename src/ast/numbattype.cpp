#include "../include/ast/numbattype.hpp"

namespace numbat {
namespace parser {


const size_t NumbatType::getSize () const {
	size_t size=0;
	for (const ASTnode & mem : members) {
		size += mem->getSize ();
	}
	return size;
}

string NumbatType::convertDataToString (const void * data, size_t & offset) const {
	string ret = "{";
	for (const ASTnode & member : members) {
		ret += member->getType ()->convertDataToString (data, offset) + ", ";
	}
	return ret + "}";
}

string NumbatType::toString (const string & indent) const {
	std::ostringstream ss;
	ss << "'" << iden << "' {";
	for (ASTnode n : members) ss << n->toString ("") << ", ";
	ss << "}";
	return ss.str ();
}

int NumbatType::findMember (const std::string & iden) {
	for (int i=0, l=members.size (); i<l; ++i) {
		if (members [i]->getIden () == iden) {
			return i;
		}
	}
	return -1;
}

}
}