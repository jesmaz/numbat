#include <nir/scope.hpp>
#include <parse/tree/identifier.hpp>

const nir::Instruction * ParseTreeIdentifier::build (nir::Scope * scope) {
	return scope->resolve (iden);
}

const nir::Instruction * ParseTreeIdentifier::buildAllocate (nir::Scope * scope, const string & iden) {
	auto * t = resolveType (scope);
	if (not t) return nullptr;
	return scope->allocateVariable (t, iden);
}

const nir::Type * ParseTreeIdentifier::resolveType (nir::Scope * scope) {
	return scope->resolveType (iden);
}

string ParseTreeIdentifier::strDump (text::PrintMode mode) {
	return iden;
}
