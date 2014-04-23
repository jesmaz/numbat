#include "../include/ast/numbatpointertype.hpp"

namespace numbat {
namespace parser {


string NumbatPointerType::convertDataToString (const void * data, size_t & offset) const {
	string ret = "{" + NumbatType::convertDataToString (data, offset) + ", ";
	return ret + dataType->getType ()->convertDataToString (data, offset) + " [] }";
}

string NumbatPointerType::toString (const string & indent) const {
	return "{" + NumbatType::toString () + ", " + dataType->getType ()->toString () + "}";
}

}
}