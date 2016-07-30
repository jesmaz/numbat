#include "../../../include/nir/scope.hpp"
#include "../../../include/parse/tree/identifier.hpp"

const nir::Type * ParseTreeIdentifier::buildType (nir::Scope * scope) {
	return scope->resolveType (iden);
}

const nir::Instruction * ParseTreeIdentifier::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	if (mode == ParseTreeNode::BuildMode::NORMAL) {
		return scope->resolve (iden);
	} else {
		const nir::Type * type = scope->resolveType (iden);
		return scope->createParameter (type);
	}
	
}

string ParseTreeIdentifier::strDump (text::PrintMode mode) {
	return iden;
}