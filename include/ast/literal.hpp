#pragma once


#include <ast/node.hpp>
#include <cinttypes>


namespace AST {


class Value : public Node {
	
	public:
		
		virtual bool isValue () const {return true;}
		
		
		Value (numbat::lexer::position pos, const TypePtr & type) : Node (pos, type) {}
		
	protected:
	private:
		
};

class Number : public Value {
	
	public:
		
		const string & getValue () const {return value;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Number (numbat::lexer::position pos, const string & value);
		Number (numbat::lexer::position pos, const string & value, const TypePtr & type) : Value (pos, type), value (value) {}
		
	protected:
	private:
		
		string value;
		
};


}
