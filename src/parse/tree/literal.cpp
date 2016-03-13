#include "../../../include/ast/constant/astconstantcstring.hpp"
#include "../../../include/ast/constant/astconstantint.hpp"
#include "../../../include/ast/constant/astconstantfpint.hpp"
#include "../../../include/nir/scope.hpp"
#include "../../../include/parse/tree/literal.hpp"

#include <string>

const nir::Instruction * ParseTreeLiteral::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	switch (tokenType) {
		case numbat::lexer::TOKEN::chararrayliteral: {
			//TODO: Character array literals
			return nullptr;
		}
			
		case numbat::lexer::TOKEN::numericliteral: {
			const nir::Type * t;
			if (literal.back () == 'f') {
				t = scope->resolveType ("float");
			} else if (literal.back () == 'h') {
				t = scope->resolveType ("half");
			} else if (literal.back () == 'q') {
				t = scope->resolveType ("quad");
			} else if (literal.find ('.') != string::npos) {
				t = scope->resolveType ("double");
			} else {
				t = scope->resolveType ("uint64");
			}
			return scope->createConstant (t, literal);
		}
			
		case numbat::lexer::TOKEN::stringliteral: {
			//TODO: String literals
			return nullptr;
		}
		
		default:
			//TODO: Handle invalid
			return nullptr;
	}
	
}

string ParseTreeLiteral::strDump (text::PrintMode mode) {
	return mode & text::COLOUR ? text::yel + literal + text::reset : literal;
}