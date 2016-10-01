#ifndef LIST_HPP
#define LIST_HPP


#include "base.hpp"


class ParseTreeList : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual bool isList () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		virtual const BasicArray <ParseTreeNode *> releaseArgs () {auto a = args; args = BasicArray <PTNode> (); return a;}
		
		ParseTreeList (uint32_t line, uint32_t pos) : ParseTreeNode (ParseTreeNode::NodeType::LIST, line, pos) {}
		ParseTreeList (const BasicArray <PTNode> & args) : ParseTreeNode (ParseTreeNode::NodeType::LIST, args.front ()->getLine (), args.front ()->getPos ()), args (args) {}
		virtual ~ParseTreeList () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		BasicArray <PTNode> args;
		
};


#endif /*LIST_HPP*/
