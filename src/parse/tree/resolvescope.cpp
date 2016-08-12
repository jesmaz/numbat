#include <nir/instruction.hpp>
#include <nir/scope.hpp>
#include <parse/tree/resolvescope.hpp>


const nir::Instruction * ResolveScope::build (nir::Scope * scope) {
	const nir::Instruction * p = parent->build (scope);
	return scope->resolve ({p, p->getIden ()}, iden);
}

const nir::Instruction * ResolveScope::buildAllocate (nir::Scope * scope, const string & iden) {
	const nir::Type * type = resolveType (scope);
	return scope->allocateVariable (type, iden);
}

const nir::Type * ResolveScope::resolveType (nir::Scope * scope) {
	const nir::Instruction * p = parent->build (scope);
	return scope->resolveType ({p, p->getIden ()}, iden);
}


string ResolveScope::strDump (text::PrintMode mode) {
	
	return parent->toString (mode) + "." + iden;
	
}