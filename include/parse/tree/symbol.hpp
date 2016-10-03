#ifndef PARSETREESYMBOL_HPP
#define PARSETREESYMBOL_HPP


#include "base.hpp"


class ParseTreeSymbol : public ParseTreeNode {
	
	public:
		
		ParseTreeSymbol (numbat::lexer::position pos, string sym) : ParseTreeNode (ParseTreeNode::NodeType::SYMBOL, pos), sym (sym) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string sym;
		
};


#endif /*PARSETREESYMBOL_HPP*/
