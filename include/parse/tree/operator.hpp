#ifndef OPERATOR_HPP
#define OPERATOR_HPP


#include "base.hpp"

#include <vector>


class ParseTreeOperator : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		
		ParseTreeOperator (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeOperator (const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()), args (args) {}
		virtual ~ParseTreeOperator () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> args;
		
};


#endif /*OPERATOR_HPP*/