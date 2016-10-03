#pragma once


#include "base.hpp"

 


struct ResolveScope : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden);
		virtual const nir::Type * resolveType (nir::Scope * scope);
		
		const string & getIden () const {return iden;}
		
		ResolveScope (PTNode parent, const string & iden) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, parent->getPos ()), parent (parent), iden (iden) {}
		
		virtual ~ResolveScope () {delete parent;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode parent;
		string iden;
		
};
