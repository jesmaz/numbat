#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeIndex : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		AST::TypePtr createASTtype (AST::Context & ctx);
		
		ParseTreeIndex (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeIndex (PTNode index, const BasicArray <PTNode> & args) : ParseTreeNode (index->getPos ()), index (index), args (args) {}
		virtual ~ParseTreeIndex () {delAll (args); delete index;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode index;
		BasicArray <PTNode> args;
		
};


}
