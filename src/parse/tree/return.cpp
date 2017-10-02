#include <parse/tree/return.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeReturn::createAST (AST::Context & ctx) {
	abort ();
}

string ParseTreeReturn::strDump (text::PrintMode mode) {
	
	return "return " + arg->toString (mode);
	
}


}

