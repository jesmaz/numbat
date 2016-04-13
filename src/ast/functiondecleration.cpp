#include "../../include/ast/functiondecleration.hpp"

namespace numbat {
namespace parser {

string FunctionDecleration::toString (const string & indent) {
	string str = "(";
	for (ASTnode n : parameters) {
		str += n->toString () + ", ";
	}
	str += ") -> (";
	for (ASTnode n : type) {
		str += n->toString () + ", ";
	}
	str += ")";
	if (body)
		return str + body->toString (indent) + "\n";
	else
		return str + "\n";
}

}
}