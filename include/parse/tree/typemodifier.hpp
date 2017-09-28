#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeTypeModifier : public ParseTreeNode {
	
	public:
		
		virtual AST::TypePtr createASTtype (AST::Context & ctx);
		
		const string & getIden () const {return sym;}
		
		ParseTreeTypeModifier (PTNode type, numbat::lexer::position pos, string sym) : ParseTreeNode (pos), type (type), sym (sym) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode type;
		string sym;
		
};


}
