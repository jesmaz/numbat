#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		AST::NodePtr createASTmeta (AST::Context & ctx);
		AST::TypePtr createASTtype (AST::Context & ctx);
		
		const string & getIden () const {return iden;}
		
		ParseTreeIdentifier (numbat::lexer::position pos, const string & iden) : ParseTreeNode (pos), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		
};


}
