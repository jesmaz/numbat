#include <ast/memory.hpp>
#include <ast/variable.hpp>
#include <nir/scope.hpp>
#include <parse/tree/braceinitaliser.hpp>


namespace parser {


AST::NodePtr ParseTreeBraceInitaliser::createAST (AST::Context & ctx) {
	
	auto t = type->createASTtype (ctx);
	
	AST::VarPtr var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), t, 0, AST::Variable::LOCATION::LOCAL, "tmp");
	ctx.allocStack (var);
	auto params = args.map <AST::NodePtr> ([&](auto & a){return a->createAST (ctx);});
	return std::make_shared <AST::RawInit> (getPos (), ctx.getSourceFile (), var, params);
	
}

string ParseTreeBraceInitaliser::strDump (text::PrintMode mode) {
	string s = "(" + type->toString (mode) +  ": {";
	for (auto a : args) {
		s += a->toString (mode) + " ";
	}
	return s + "}";
}


}
