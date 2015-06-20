#ifndef PARSETREELITERAL_HPP
#define PARSETREELITERAL_HPP


#include "base.hpp"


class ParseTreeLiteral : public ParseTreeNode {
	
	public:
		
		ParseTreeLiteral (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		
	protected:
	private:
		
		virtual string strDump (ParseTreeNode::PrintMode mode) {return literal;}
		
		string literal;
		
};


#endif /*PARSETREELITERAL_HPP*/