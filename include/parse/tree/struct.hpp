#ifndef STRUCT_HPP
#define STRUCT_HPP


#include "base.hpp"

#include <vector>


struct Struct : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope, ParseTreeNode::BuildMode mode);
		
		void declare (nir::Scope * scope);
		
		Struct (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		Struct (const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> tplate, tags, members;
		nir::Struct * type;
		numbat::parser::NumbatScope * sscope;
		
};


#endif /*STRUCT_HPP*/