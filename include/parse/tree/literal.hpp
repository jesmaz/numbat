#ifndef PARSETREELITERAL_HPP
#define PARSETREELITERAL_HPP


#include "base.hpp"
#include "../../token.hpp"


class ParseTreeLiteral : public ParseTreeNode {
	
	public:
		
		virtual numbat::parser::ASTnode build (numbat::parser::NumbatScope * scope);
		
		ParseTreeLiteral (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeLiteral (uint32_t line, uint32_t pos, const string & literal, numbat::lexer::TOKEN tokenType) : ParseTreeNode (line, pos), literal (literal), tokenType(tokenType) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string literal;
		numbat::lexer::TOKEN tokenType;
		
};


#endif /*PARSETREELITERAL_HPP*/