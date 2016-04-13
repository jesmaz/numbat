#include "../../include/ast/type/numbatpointertype.hpp"

namespace numbat {
namespace parser {


const bool NumbatPointerType::isValid () const {
	return NumbatType::isValid () and dataType->isValid ();
}

string NumbatPointerType::convertDataToString (const void * data, size_t & offset) const {
	string ret = "{" + NumbatType::convertDataToString (data, offset) + ", ";
	return ret + dataType->getType ()->convertDataToString (data, offset) + " [] }";
}

string NumbatPointerType::toString (const string & indent) const {
	return indent + "{" + NumbatType::toString () + ", " + dataType->getType ()->toString () + "}";
}

}
}