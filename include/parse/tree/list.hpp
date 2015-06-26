#ifndef LIST_HPP
#define LIST_HPP


#include "base.hpp"


class ParseTreeList : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const std::vector <ParseTreeNode *> & getArgs () const {return args;}
		virtual const std::vector <ParseTreeNode *> & releaseArgs () const {auto a = args; args = std::vector <PTNode> (); return a;}
		
		ParseTreeList (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos, ParseTreeNode::NodeType::LIST) {}
		ParseTreeList (const std::vector <PTNode> & args) : ParseTreeList (args.front ()->getLine (), args.front ()->getPos ()), args (args) {}
		virtual ~ParseTreeList () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> args;
		
};


#endif /*LIST_HPP*/