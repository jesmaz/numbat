#pragma once


#include <parse/tree/base.hpp>
#include <parse/tree/variable.hpp>


namespace parser {


class ParseTreeBraceInitaliser : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		
		ParseTreeBraceInitaliser (numbat::lexer::position pos, PTNode type, const BasicArray <PTNode> & args) : ParseTreeNode (pos), type (type), args (args) {}
		
		virtual ~ParseTreeBraceInitaliser () {delete type; delAll (args);}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode type;
		BasicArray <PTNode> args;
		
};


}

