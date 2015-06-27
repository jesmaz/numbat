#ifndef PARSETREEKWYWORD_HPP
#define PARSETREEKWYWORD_HPP


#include "base.hpp"


class ParseTreeKeyword : public ParseTreeNode {
	
	public:
		
		ParseTreeKeyword (uint32_t line, uint32_t pos, string sym) : ParseTreeNode (ParseTreeNode::NodeType::KEYWORD, line, pos), sym (sym) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string sym;
		
};


#endif /*PARSETREEKWYWORD_HPP*/