#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeList : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual bool isList () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		virtual const BasicArray <ParseTreeNode *> releaseArgs () {auto a = args; args = BasicArray <PTNode> (); return a;}
		
		ParseTreeList (numbat::lexer::position pos) : ParseTreeNode (ParseTreeNode::NodeType::LIST, pos) {}
		ParseTreeList (const BasicArray <PTNode> & args) : ParseTreeNode (ParseTreeNode::NodeType::LIST, args.front ()->getPos ()), args (args) {}
		virtual ~ParseTreeList () {delAll (args);}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		BasicArray <PTNode> args;
		
};


}
