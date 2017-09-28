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

AST::NodePtr ParseTreeIdentifier::createASTmeta (AST::Context & ctx) {
	
	auto var = ctx.resolve ("@" + iden);
	if (var) return var;
	//TODO: perform a spell check to offer suggested fixes
	report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Failed to resolve '@" + iden + "'");
	return nullptr;
	
}

AST::TypePtr ParseTreeIdentifier::createASTtype (AST::Context & ctx) {
	
	auto type = ctx.resolveType (iden);
	if (type) return type;
	report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Failed to resolve '" + iden + "'");
	return nullptr;
	
}

string ParseTreeIdentifier::strDump (text::PrintMode mode) {
	return iden;
}


}
