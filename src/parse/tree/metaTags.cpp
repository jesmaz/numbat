#include <parse/tree/metaTags.hpp>

const nir::Instruction * MetaTags::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return componant->build (scope, mode);
}

void MetaTags::declare (nir::Scope * scope) {
	componant->declare (scope);
}