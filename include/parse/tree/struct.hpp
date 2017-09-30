#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct Struct : ParseTreeNode {
	
	public:
		
		AST::TypePtr createType (AST::Context & ctx);
		AST::NodePtr createAST (AST::Context & ctx);
		
		Struct (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		Struct (numbat::lexer::position pos, const string & iden, const BasicArray <PTNode> & members) : ParseTreeNode (pos), iden (iden), members (members) {}
		
		virtual ~Struct () {delAll (tplate); delAll (tags); delAll (members);}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		BasicArray <PTNode> tplate, tags, members;
		std::shared_ptr <AST::Struct> stype=nullptr;
		
};


}
