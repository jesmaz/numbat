#ifndef PARSETREEIDENTIFIER_HPP
#define PARSETREEIDENTIFIER_HPP


#include "base.hpp"


class ParseTreeIdentifier : public ParseTreeNode {
	
	public:
		
		ParseTreeIdentifier (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		
	protected:
	private:
		
		virtual string strDump () {return "identifier";}
		
		string iden;
		
};


#endif /*PARSETREEIDENTIFIER_HPP*/