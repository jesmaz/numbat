#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct ParseTreeIfElse : ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		ParseTreeIfElse (numbat::lexer::position pos) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, pos), body (nullptr) {}
		ParseTreeIfElse (PTNode cond, PTNode body, PTNode alternate=nullptr) : ParseTreeNode (ParseTreeNode::NodeType::EXPRESSION, cond->getPos ()), cond (cond), body (body), alternate (alternate) {}
		
		virtual ~ParseTreeIfElse () {delete cond; delete body; delete alternate;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode cond, body, alternate;
		
};


}
