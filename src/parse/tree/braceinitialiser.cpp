#include <ast/memory.hpp>
#include <ast/variable.hpp>
#include <nir/scope.hpp>
#include <parse/tree/braceinitaliser.hpp>


namespace parser {


AST::NodePtr ParseTreeBraceInitaliser::createAST (AST::Context & ctx) {
	
	auto t = type->createASTtype (ctx);
	
	AST::VarPtr var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), t, ctx.allocStack (), AST::Variable::LOCATION::LOCAL, "tmp");
	return std::make_shared <AST::RawInit> (getPos (), ctx.getSourceFile (), var, body->createAST (ctx));
	
}

string ParseTreeBraceInitaliser::strDump (text::PrintMode mode) {
	return "(" + type->toString (mode) +  ": " + body->toString (mode) + ")";
}


}
