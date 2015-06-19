#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include "base.hpp"

#include <vector>


struct ParseTree;

struct Function : ParseTreeNode {
	
	public:
		
		Function (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		Function (const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()) {}
		
	protected:
	private:
		
		virtual string strDump () {return "";}
		
		std::vector <PTNode> tplate, tags, params, type;
		string iden;
		ParseTree * body;
		
};


#endif /*FUNCTION_HPP*/