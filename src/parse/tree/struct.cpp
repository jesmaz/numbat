#include "../../../include/parse/tree/struct.hpp"

string Struct::strDump (text::PrintMode mode) {
	return "";
}

void Struct::declare (numbat::parser::NumbatScope * scope) {
	
	numbat::parser::createStruct (scope, getIden (), {});
	sscope = numbat::parser::createChild (scope);
	
}