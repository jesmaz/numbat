#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct ResolveScope : ParseTreeNode {
	
	public:
		
		const string & getIden () const {return iden;}
		
		ResolveScope (PTNode parent, const string & iden) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, parent->getPos ()), parent (parent), iden (iden) {}
		
		virtual ~ResolveScope () {delete parent;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode parent;
		string iden;
		
};


}
