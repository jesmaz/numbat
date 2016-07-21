#include <nir/instruction.hpp>
#include <nir/scope.hpp>
#include <parse/tree/resolvescope.hpp>


const nir::Instruction * ResolveScope::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	const nir::Instruction * p = parent->build (scope, mode);
	if (mode == ParseTreeNode::BuildMode::NORMAL) {
		return scope->resolve ({p, p->getIden ()}, iden);
	} else {
		return scope->createParameter (scope->resolveType ({p, p->getIden ()}, iden));
	}
	
}

string ResolveScope::strDump (text::PrintMode mode) {
	
	return parent->toString (mode) + "." + iden;
	
}