#include "../../../include/nir/module.hpp"
#include "../../../include/nir/scope.hpp"
#include "../../../include/nir/type/struct.hpp"
#include "../../../include/parse/tree/struct.hpp"


const nir::Instruction * Struct::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	
	
}

void Struct::declare (nir::Scope * scope) {
	
	//type = scope->registerStruct (getIden ());
	
}

string Struct::strDump (text::PrintMode mode) {
	return "";
}