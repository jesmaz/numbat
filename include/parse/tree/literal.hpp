#pragma once


#include <parse/tree/base.hpp>
#include <token.hpp>


namespace parser {


class ParseTreeLiteral : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		
		ParseTreeLiteral (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeLiteral (numbat::lexer::position pos, const string & literal, numbat::lexer::TOKEN tokenType) : ParseTreeNode (pos), literal (literal), tokenType(tokenType) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string literal;
		numbat::lexer::TOKEN tokenType;
		
};


}
