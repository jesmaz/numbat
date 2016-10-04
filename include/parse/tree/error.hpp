#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeError : public ParseTreeNode {
	
	public:
		
		ParseTreeError (numbat::lexer::position pos, const string & err) : ParseTreeNode (pos), err (err) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string err;
		
};


}
