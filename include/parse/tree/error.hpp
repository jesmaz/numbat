#ifndef PARSETREEERROR_HPP
#define PARSETREEERROR_HPP


#include "base.hpp"


class ParseTreeError : public ParseTreeNode {
	
	public:
		
		ParseTreeError (uint32_t line, uint32_t pos, const string & err) : ParseTreeNode (line, pos), err (err) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string err;
		
};


#endif /*PARSETREEERROR_HPP*/