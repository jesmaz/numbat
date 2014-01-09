#include "../include/ast/functiondecleration.hpp"

namespace numbat {
namespace parser {

string FunctionDecleration::toString (const string & indent) {
	string str = "(";
	for (ASTnode n : parameters) {
		str += n->toString (indent) + ", ";
	}
	str += "): {";
	for (ASTnode n : type) {
		str += n->toString (indent) + ", ";
	}
	str += "}";
	if (body)
		return str + "\n" + body->toString (indent + "\t") + "\n";
	else
		return str + "\n";
}

}
}