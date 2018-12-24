#include <ast/variable.hpp>
#include <parse/tree/identifier.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeIdentifier::createAST (AST::Context & ctx) {
	
	auto var = ctx.resolve (iden);
	if (var) {
		if (typeid (*var) == typeid (AST::Variable)) {
			return std::make_shared <AST::VariableRef> (getPos (), ctx.getSourceFile (), std::static_pointer_cast <AST::Variable> (var));
		}
		return var;
	}
	//TODO: perform a spell check to offer suggested fixes
	report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Failed to resolve '" + iden + "'");
	return nullptr;
	
}

AST::NodePtr ParseTreeIdentifier::createASTmeta (AST::Context & ctx) {
	
	auto var = ctx.resolve ("@" + iden);
	if (var) {
		if (typeid (*var) == typeid (AST::Variable)) {
			return std::make_shared <AST::VariableRef> (getPos (), ctx.getSourceFile (), std::static_pointer_cast <AST::Variable> (var));
		}
		return var;
	}
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
