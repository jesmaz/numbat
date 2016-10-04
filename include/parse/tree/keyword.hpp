#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeKeyword : public ParseTreeNode {
	
	public:
		
		const string & getIden () const {return sym;}
		
		ParseTreeKeyword (numbat::lexer::position pos, string sym) : ParseTreeNode (ParseTreeNode::NodeType::KEYWORD, pos), sym (sym) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string sym;
		
};


}
