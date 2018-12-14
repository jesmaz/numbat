#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct ParseTreeLoop : ParseTreeNode {
	
public:
	
	AST::NodePtr createAST (AST::Context & ctx);
	
	ParseTreeLoop (PTNode init, PTNode cond, PTNode step, PTNode body) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, cond->getPos ()), init (init), cond (cond), step (step), body (body) {}
	
	virtual ~ParseTreeLoop () {delete init; delete cond; delete step; delete body;}
	
protected:
private:
	
	virtual string strDump (text::PrintMode mode);
	
	PTNode init, cond, step, body;
	
};


}

