#ifndef OPERATOR_HPP
#define OPERATOR_HPP


#include "base.hpp"

#include <vector>


class ParseTreeOperator : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const std::vector <ParseTreeNode *> & getArgs () const {return args;}
		
		virtual const nir::Instruction * build (nir::Scope * scope, ParseTreeNode::BuildMode mode);
		
		ParseTreeOperator (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeOperator (const string & iden, const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()), iden (iden), args (args) {}
		virtual ~ParseTreeOperator () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		std::vector <PTNode> args;
		
};


#endif /*OPERATOR_HPP*/