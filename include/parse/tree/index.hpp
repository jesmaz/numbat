#pragma once


#include <parse/tree/base.hpp>


class ParseTreeIndex : public ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden);
		virtual const nir::Type * resolveType (nir::Scope * scope);
		
		ParseTreeIndex (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeIndex (PTNode index, const std::vector <PTNode> & args) : ParseTreeNode (index->getLine (), index->getPos ()), index (index), args (args) {}
		virtual ~ParseTreeIndex () {delete index;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode index;
		std::vector <PTNode> args;
		
};