#include <nir/scope.hpp>
#include <parse/tree/identifier.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeIdentifier::createAST (AST::Context & ctx) {
	
	auto var = ctx.resolve (iden);
	if (var) return var;
	//TODO: perform a spell check to offer suggested fixes
	report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Failed to resolve '" + iden + "'");
	return nullptr;
	
}

AST::TypePtr ParseTreeIdentifier::createASTtype (AST::Context & ctx) {
	
	auto type = ctx.resolveType (iden);
	if (type) return type;
	report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Failed to resolve '" + iden + "'");
	return nullptr;
	
}


const nir::Instruction * ParseTreeIdentifier::build (nir::Scope * scope) {
	return scope->resolve (iden, getPos ());
}

const nir::Instruction * ParseTreeIdentifier::buildAllocate (nir::Scope * scope, const string & iden) {
	auto * t = resolveType (scope);
	if (not t) return nullptr;
	return scope->allocateVariable (t, iden);
}

const nir::Type * ParseTreeIdentifier::resolveType (nir::Scope * scope) {
	return scope->resolveType (iden, getPos ());
}

string ParseTreeIdentifier::strDump (text::PrintMode mode) {
	return iden;
}


}
