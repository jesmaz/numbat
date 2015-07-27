#ifndef PARSETREEIDENTIFIER_HPP
#define PARSETREEIDENTIFIER_HPP


#include "base.hpp"


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		virtual const string & getIden () const {return iden;}
		
		virtual numbat::parser::ASTnode build (numbat::parser::NumbatScope * scope);
		
		ParseTreeIdentifier (uint32_t line, uint32_t pos, const string & iden) : ParseTreeNode (line, pos), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		
};


#endif /*PARSETREEIDENTIFIER_HPP*/