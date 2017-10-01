#include <ast/literal.hpp>
#include <ast/type.hpp>
#include <nir/scope.hpp>
#include <parse/tree/literal.hpp>
#include <string>


namespace parser {


AST::NodePtr ParseTreeLiteral::createAST (AST::Context & ctx) {
	
	switch (tokenType) {
		case numbat::lexer::TOKEN::chararrayliteral: {
			auto data = BasicArray <uint8_t> (literal.begin (), literal.end ()).map <Literal> ([](auto & n){
				return std::make_shared <NumericLiteralTemplate <uint8_t>> (n);
			});
			return std::make_shared <AST::ArrayVal> (
				getPos (),
				ctx.getSourceFile (),
				std::make_shared <ArrayLiteral> (data),
				AST::Array::get (AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 8))
			);
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
