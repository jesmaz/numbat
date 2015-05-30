#ifndef PARSETREE_HPP
#define PARSETREE_HPP


#include "tree/function.hpp"

#include <vector>


class ParseTree : public ParseTreeNode {
	
	public:
		
		ParseTree (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTree (const std::vector <PTNode> & body) : ParseTreeNode (body.front ()->getLine (), body.front ()->getPos ()), body (body) {}
		
	protected:
	private:
		
		virtual string strDump () {string s = "{"; for (size_t i=0; i+1<body.size (); ++i) s += body [i]->toString () + "; "; return s + body.back ()->toString () + ";}";}
		
		std::vector <PTNode> body;
		std::vector <Function *> functions;
		
};


#endif /*PARSERTREE_HPP*/