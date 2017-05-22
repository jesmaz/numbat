#pragma once


#include <ast/node.hpp>
#include <cinttypes>


namespace AST {


class Number : public Node {
	
	public:
		
		const string & getValue () const {return value;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Number (numbat::lexer::position pos, const string & value);
		
	protected:
	private:
		
		string value;
		
};


}
