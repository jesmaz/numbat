#include <ast/flowcontrol.hpp>
#include <nir/scope.hpp>
#include <parse/tree/ifelse.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeIfElse::createAST (AST::Context & ctx) {
	if (alternate) {
		return std::make_shared <AST::Unresolved_IfElse> (getPos (), ctx.getSourceFile (), cond->createAST (ctx), body->createAST (ctx), alternate->createAST (ctx));
	} else {
		return std::make_shared <AST::Unresolved_IfElse> (getPos (), ctx.getSourceFile (), cond->createAST (ctx), body->createAST (ctx), nullptr);
	}
}

string ParseTreeIfElse::strDump (text::PrintMode mode) {
	
	string s = "if (" + cond->toString (mode) + ") " + body->toString (mode);
	if (alternate) {
		s += " else " + alternate->toString (mode);
	}
	return s;
	
}


}
