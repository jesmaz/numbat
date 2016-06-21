#pragma once


#include "base.hpp"

#include <vector>


struct ResolveScope : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope, BuildMode mode);
		const string & getIden () const {return iden;}
		
		ResolveScope (PTNode parent, const string & iden) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, parent->getLine (), parent->getPos ()), parent (parent), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode parent;
		string iden;
		
};
