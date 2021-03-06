#pragma once


#include "base.hpp"

 


struct ParseTreeIfElse : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		ParseTreeIfElse (uint32_t line, uint32_t pos) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, line, pos), body (nullptr) {}
		ParseTreeIfElse (PTNode cond, PTNode body, PTNode alternate=nullptr) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, cond->getLine (), cond->getPos ()), cond (cond), body (body), alternate (alternate) {}
		
		virtual ~ParseTreeIfElse () {delete cond; delete body; delete alternate;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode cond, body, alternate;
		
};
