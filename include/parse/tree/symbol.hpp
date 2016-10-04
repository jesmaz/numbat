#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeSymbol : public ParseTreeNode {
	
	public:
		
		ParseTreeSymbol (numbat::lexer::position pos, string sym) : ParseTreeNode (ParseTreeNode::NodeType::SYMBOL, pos), sym (sym) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string sym;
		
};


}
