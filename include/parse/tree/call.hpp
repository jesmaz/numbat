#pragma once


#include "base.hpp"


namespace parser {


class ParseTreeCall : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		
		virtual bool isAggregate () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		ParseTreeCall (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeCall (const PTNode & iden, const BasicArray <PTNode> & args) : ParseTreeNode (iden->getPos ()), iden (iden), args (args) {}
		virtual ~ParseTreeCall () {delAll (args); delete iden;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode iden;
		BasicArray <PTNode> args;
		
};


}
