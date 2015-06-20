#ifndef STRUCT_HPP
#define STRUCT_HPP


#include "base.hpp"

#include <vector>


struct Struct : ParseTreeNode {
	
	public:
		
		Struct (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		Struct (const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) {return "";}
		
		std::vector <PTNode> tplate, tags, type;
		
};


#endif /*STRUCT_HPP*/