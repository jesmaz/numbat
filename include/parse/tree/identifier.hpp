#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		AST::TypePtr createASTtype (AST::Context & ctx);
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden);
		virtual const nir::Type * resolveType (nir::Scope * scope);
		
		const string & getIden () const {return iden;}
		
		ParseTreeIdentifier (numbat::lexer::position pos, const string & iden) : ParseTreeNode (pos), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		
};


}
