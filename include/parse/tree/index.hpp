#ifndef PARSETREEINDEX_HPP
#define PARSETREEINDEX_HPP


#include "base.hpp"


class ParseTreeIndex : public ParseTreeNode {
	
	public:
		
		ParseTreeIndex (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeIndex (PTNode index) : ParseTreeNode (index->getLine (), index->getPos ()), index (index) {}
		virtual ~ParseTreeIndex () {delete index;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode index;
		
};


#endif /*PARSETREEINDEX_HPP*/