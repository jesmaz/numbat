#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct ParseTreeReturn : ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		
		ParseTreeReturn (numbat::lexer::position pos, PTNode arg) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, pos), arg (arg) {}
		
		virtual ~ParseTreeReturn () {delete arg;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode arg;
		
};


}

