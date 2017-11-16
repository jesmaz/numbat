#include <ast/call.hpp>
#include <ast/type.hpp>
#include <ast/variable.hpp>
#include <nir/scope.hpp>
#include <parse/tree/literal.hpp>
#include <string>


namespace parser {


AST::NodePtr ParseTreeLiteral::createAST (AST::Context & ctx) {
	
	switch (tokenType) {
		case numbat::lexer::TOKEN::chararrayliteral: {
			auto data = BasicArray <Literal> (literal.begin (), literal.end ()).map <Literal> ([](auto & n){
				return Literal (n);
			});
			return std::make_shared <AST::StaticValue> (
				getPos (),
				ctx.getSourceFile (),
				AST::Array::get (AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 8)),
				data
			);
		}
		
		case numbat::lexer::TOKEN::numericliteral: {
			return AST::StaticValue::parseNumber (getPos (), ctx.getSourceFile (), literal);
		}
		
		case numbat::lexer::TOKEN::stringliteral: {
			auto data = BasicArray <Literal> (literal.begin (), literal.end ()).map <Literal> ([](auto & n){
				return Literal (n);
			});
			auto arg = std::make_shared <AST::StaticValue> (
				getPos (),
				ctx.getSourceFile (),
				AST::Array::get (AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 8)),
				data
			);
			auto var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), AST::Const::get (ctx.resolveType ("string")), AST::Variable::globalContex.reserve (), AST::Variable::LOCATION::GLOBAL, "");
			return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> {arg});
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
