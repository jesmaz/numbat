#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct Struct : public ParseTreeNode {
	
	public:
		
		AST::TypePtr createType (AST::Context & ctx);
		AST::NodePtr createAST (AST::Context & ctx);
		
		const string & getIden () const {return iden;}
		
		Struct * asStruct () {return this;}
		
		Struct (numbat::lexer::position pos) : ParseTreeNode (ParseTreeNode::NodeType::STRUCT, pos) {}
		Struct (numbat::lexer::position pos, const string & iden, const BasicArray <PTNode> & members) : ParseTreeNode (ParseTreeNode::NodeType::STRUCT, pos), iden (iden), members (members) {}
		
		virtual ~Struct () {delAll (tplate); delAll (tags); delAll (members);}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		BasicArray <PTNode> tplate, tags, members;
		std::shared_ptr <AST::Struct> stype=nullptr;
		
};


}
