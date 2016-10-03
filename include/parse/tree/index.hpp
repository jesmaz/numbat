#pragma once


#include <parse/tree/base.hpp>


class ParseTreeIndex : public ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden);
		virtual const nir::Type * resolveType (nir::Scope * scope);
		
		ParseTreeIndex (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeIndex (PTNode index, const BasicArray <PTNode> & args) : ParseTreeNode (index->getPos ()), index (index), args (args) {}
		virtual ~ParseTreeIndex () {delAll (args); delete index;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode index;
		BasicArray <PTNode> args;
		
};
