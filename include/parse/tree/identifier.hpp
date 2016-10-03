#ifndef PARSETREEIDENTIFIER_HPP
#define PARSETREEIDENTIFIER_HPP


#include "base.hpp"


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden);
		virtual const nir::Type * resolveType (nir::Scope * scope);
		
		const string & getIden () const {return iden;}
		
		ParseTreeIdentifier (uint32_t line, uint32_t pos, const string & iden) : ParseTreeNode (line, pos), iden (iden) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string iden;
		
};


#endif /*PARSETREEIDENTIFIER_HPP*/