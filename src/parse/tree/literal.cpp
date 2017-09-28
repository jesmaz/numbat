#include <ast/literal.hpp>
#include <nir/scope.hpp>
#include <parse/tree/literal.hpp>
#include <string>


namespace parser {


AST::NodePtr ParseTreeLiteral::createAST (AST::Context & ctx) {
	
	switch (tokenType) {
		case numbat::lexer::TOKEN::chararrayliteral: {
			//TODO: Character array literals
			return nullptr;
		}
		
		case numbat::lexer::TOKEN::numericliteral: {
			return std::make_shared <AST::Number> (getPos (), ctx.getSourceFile (), literal);
		}
		
		case numbat::lexer::TOKEN::stringliteral: {
			//TODO: String literals
			return nullptr;
		}
		
		default: {
			//TODO: Handle invalid
			return nullptr;
		}
	}
	
}

string ParseTreeLiteral::strDump (text::PrintMode mode) {
	return mode & text::COLOUR ? text::yel + literal + text::reset : literal;
}


}
