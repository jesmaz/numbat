#include "../include/ast/control/astbody.hpp"
#include "../include/ast/memory/aststructindex.hpp"

namespace numbat {
namespace parser {


const ASTnode ASTbase::getLength (const ASTnode & exp) {
	
	ASTnode val = nullptr;
	ssize_t index = exp->getLengthIndex ();
	if (0 <= index) {
		val = ASTnode (new ASTstructIndex (index, exp));
	}
	return val;
	
}

const ASTnode ASTbase::getArrayType () const {
	
	auto type = getType ();
	ASTnode t = nullptr;
	if (type) {
		t = type->getArrayType ();
	}
	return t;
	
}

ssize_t ASTbase::getLengthIndex () const {
	
	auto type = getType ();
	if (type) {
		return type->getLengthIndex ();
	}
	return -1;
	
}


}
}