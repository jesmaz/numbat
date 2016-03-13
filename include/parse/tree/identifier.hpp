#ifndef PARSETREEIDENTIFIER_HPP
#define PARSETREEIDENTIFIER_HPP


#include "base.hpp"


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		const nir::Type * buildType (nir::Scope * scope);
		const nir::Instruction * build (nir::Scope * scope, ParseTreeNode::BuildMode mode);
		numbat::parser::ASTnode build (numbat::parser::NumbatScope * scope);
		const string & getIden () const {return iden;}
		
		ParseTreeIdentifier (uint32_t line, uint32_t pos, const string & iden) : ParseTreeNode (line, pos), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		
};


#endif /*PARSETREEIDENTIFIER_HPP*/