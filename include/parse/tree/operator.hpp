#ifndef OPERATOR_HPP
#define OPERATOR_HPP


#include "base.hpp"

#include <vector>


class ParseTreeOperator : public ParseTreeNode {
	
	public:
		
		ParseTreeOperator (const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()), args (args) {}
		
	protected:
	private:
		
		virtual string strDump () {string s = "("; for (size_t i=0; i+1<args.size (); ++i) s += args [i]->toString () + " "; return s + args.back ()->toString () + ")";}
		
		std::vector <PTNode> args;
		
};


#endif /*OPERATOR_HPP*/