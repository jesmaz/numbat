#include "../include/ast/astbody.hpp"

namespace numbat {
namespace parser {


ssize_t ASTbase::getLengthIndex () const {
	
	auto type = getType ();
	if (type) {
		return type->getLengthIndex ();
	}
	return -1;
	
}


}
}