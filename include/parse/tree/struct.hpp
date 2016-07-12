#ifndef STRUCT_HPP
#define STRUCT_HPP


#include "base.hpp"

#include <vector>


struct Struct : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope, ParseTreeNode::BuildMode mode);
		
		void declare (nir::Scope * scope);
		
		Struct (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		Struct (uint32_t line, uint32_t pos, const string & iden, const std::vector <PTNode> & members) : ParseTreeNode (line, pos), iden (iden), members (members) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		std::vector <PTNode> tplate, tags, members;
		nir::Struct * type;
		numbat::parser::NumbatScope * sscope;
		
};


#endif /*STRUCT_HPP*/