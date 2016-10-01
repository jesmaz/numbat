#ifndef STRUCT_HPP
#define STRUCT_HPP


#include "base.hpp"

 


struct Struct : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		void declare (nir::Scope * scope);
		
		Struct (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		Struct (uint32_t line, uint32_t pos, const string & iden, const BasicArray <PTNode> & members) : ParseTreeNode (line, pos), iden (iden), members (members) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		BasicArray <PTNode> tplate, tags, members;
		nir::Struct * type=nullptr;
		
};


#endif /*STRUCT_HPP*/
