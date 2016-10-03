#ifndef STRUCT_HPP
#define STRUCT_HPP


#include "base.hpp"

 


struct Struct : ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		void declare (nir::Scope * scope);
		
		Struct (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		Struct (numbat::lexer::position pos, const string & iden, const BasicArray <PTNode> & members) : ParseTreeNode (pos), iden (iden), members (members) {}
		
		virtual ~Struct () {delAll (tplate); delAll (tags); delAll (members);}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		BasicArray <PTNode> tplate, tags, members;
		nir::Struct * type=nullptr;
		
};


#endif /*STRUCT_HPP*/
