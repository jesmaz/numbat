#include <ast/sequence.hpp>
#include <parse/tree/return.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeReturn::createAST (AST::Context & ctx) {
	return std::make_shared <AST::Return> (getPos (), ctx.getSourceFile (), arg->createAST (ctx));
}

string ParseTreeReturn::strDump (text::PrintMode mode) {
	
	return "return " + arg->toString (mode);
	
}


}

