#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class MetaTags : public ParseTreeNode {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx);
		AST::NodePtr createASTparam (AST::Context & ctx);
		
		virtual bool isAggregate () {return componant->isAggregate ();}
		virtual bool isList () {return componant->isList ();}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return componant->getArgs ();}
		virtual const BasicArray <ParseTreeNode *> releaseArgs () {return componant->releaseArgs ();}
		virtual const BasicArray <ParseTreeNode *> & getTags () const {return tags;}
		virtual const BasicArray <ParseTreeNode *> & getTemplate () const {return componant->getTemplate ();}
		virtual const string & getIden () const {return componant->getIden ();}
		virtual Function * asFunction () {return componant->asFunction ();}
		virtual Struct * asStruct () {return componant->asStruct ();}
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		virtual void declare (nir::Scope * scope);
		
		MetaTags (const BasicArray <PTNode> & tags, PTNode componant) : ParseTreeNode (componant->getType (), componant->getPos ()), tags (tags), componant (componant) {}
		virtual ~MetaTags () {delAll (tags); delete componant;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		BasicArray <PTNode> tags;
		PTNode componant;
		
};


}
