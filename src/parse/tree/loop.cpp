#include <ast/flowcontrol.hpp>
#include <nir/scope.hpp>
#include <parse/tree/loop.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeLoop::createAST (AST::Context & ctx) {
	auto context = std::make_unique <AST::Context> (ctx);
	AST::NodePtr i, c, s, b;
	if (init) {
		i = init->createAST (*context);
	}
	c = cond->createAST (*context);
	if (step) {
		s = step->createAST (*context);
	}
	b = body->createAST (*context);
	return std::make_shared <AST::Unresolved_Loop> (getPos (), ctx.getSourceFile (), i, c, s, b);
}

string ParseTreeLoop::strDump (text::PrintMode mode) {
	
	string s = "loop (";
	if (init) {
		s += init->toString (mode);
	}
	s += "; " + cond->toString (mode) + "; ";
	if (step) {
		s += step->toString (mode);
	}
	s += ") " + body->toString (mode);
	return s;
	
}


}

