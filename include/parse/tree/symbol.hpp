#ifndef PARSETREESYMBOL_HPP
#define PARSETREESYMBOL_HPP


#include "base.hpp"


class ParseTreeSymbol : public ParseTreeNode {
	
	public:
		
		ParseTreeSymbol (uint32_t line, uint32_t pos, string sym) : ParseTreeNode (line, pos), sym (sym) {}
		
	protected:
	private:
		
		virtual string strDump () {return sym;}
		
		string sym;
		
};


#endif /*PARSETREESYMBOL_HPP*/