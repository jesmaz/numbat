#include "../../../include/parse/tree/identifier.hpp"

numbat::parser::ASTnode ParseTreeIdentifier::build (numbat::parser::NumbatScope * scope) {
	return numbat::parser::resolveSymbol (scope, iden);
}

string ParseTreeIdentifier::strDump (text::PrintMode mode) {
	return iden;
}