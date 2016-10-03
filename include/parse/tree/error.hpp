#ifndef PARSETREEERROR_HPP
#define PARSETREEERROR_HPP


#include "base.hpp"


class ParseTreeError : public ParseTreeNode {
	
	public:
		
		ParseTreeError (numbat::lexer::position pos, const string & err) : ParseTreeNode (pos), err (err) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string err;
		
};


#endif /*PARSETREEERROR_HPP*/