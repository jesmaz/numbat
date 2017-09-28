#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeVariable : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		AST::NodePtr createASTparam (AST::Context & ctx);
		
		PTNode releaseIden () {auto e = iden; iden = 0; return e;}
		PTNode releaseInst () {auto e = inst; inst = 0; return e;}
		PTNode releaseVType () {auto e = vType; vType = 0; return e;}
		
		ParseTreeVariable (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeVariable (PTNode vType, PTNode iden) : ParseTreeNode (iden->getPos ()), vType (vType), iden (iden), inst (nullptr) {}
		ParseTreeVariable (PTNode vType, PTNode iden, PTNode inst) : ParseTreeNode (iden->getPos ()), vType (vType), iden (iden), inst (inst) {}
		
		virtual ~ParseTreeVariable () {delete vType; delete iden; delete inst;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode vType=nullptr, iden=nullptr, inst=nullptr;
		
};


}
