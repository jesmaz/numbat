#pragma once


#include <parse/tree/base.hpp>
#include <parse/tree/variable.hpp>


namespace parser {


class ParseTreeBraceInitaliser : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		
		ParseTreeBraceInitaliser (numbat::lexer::position pos, PTNode type, PTNode body) : ParseTreeNode (pos), type (type), body (body) {}
		
		virtual ~ParseTreeBraceInitaliser () {delete type; delete body;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode type, body;
		
};


}

