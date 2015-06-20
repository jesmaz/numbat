#ifndef PARSETREEIDENTIFIER_HPP
#define PARSETREEIDENTIFIER_HPP


#include "base.hpp"


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		ParseTreeIdentifier (uint32_t line, uint32_t pos, const string & iden) : ParseTreeNode (line, pos), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (ParseTreeNode::PrintMode mode) {return iden;}
		
		string iden;
		
};


#endif /*PARSETREEIDENTIFIER_HPP*/