#ifndef PARSETREELITERAL_HPP
#define PARSETREELITERAL_HPP


#include "base.hpp"


class ParseTreeLiteral : public ParseTreeNode {
	
	public:
		
		ParseTreeLiteral (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeLiteral (uint32_t line, uint32_t pos, const string & literal) : ParseTreeNode (line, pos), literal (literal) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string literal;
		
};


#endif /*PARSETREELITERAL_HPP*/