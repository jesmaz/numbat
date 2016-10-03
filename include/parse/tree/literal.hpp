#ifndef PARSETREELITERAL_HPP
#define PARSETREELITERAL_HPP


#include "base.hpp"
#include "../../token.hpp"


class ParseTreeLiteral : public ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		ParseTreeLiteral (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeLiteral (numbat::lexer::position pos, const string & literal, numbat::lexer::TOKEN tokenType) : ParseTreeNode (pos), literal (literal), tokenType(tokenType) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string literal;
		numbat::lexer::TOKEN tokenType;
		
};


#endif /*PARSETREELITERAL_HPP*/