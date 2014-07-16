#include "../include/ast/astbody.hpp"
#include "../include/ast/aststructindex.hpp"

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

ssize_t ASTbase::getLengthIndex () const {
	
	auto type = getType ();
	if (type) {
		return type->getLengthIndex ();
	}
	return -1;
	
}


}
}