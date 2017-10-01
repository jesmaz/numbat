#pragma once


#include <ast/node.hpp>
#include <cinttypes>
#include <utility/literals.hpp>


namespace AST {


class Value : public Node {
	
	public:
		
		virtual bool isValue () const {return true;}
		
		
		Value (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type) : Node (pos, file, type) {}
		
	protected:
	private:
		
};

class Number : public Value {
	
	public:
		
		const std::shared_ptr <NumericLiteral> & getValue () const {return value;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Number (numbat::lexer::position pos, const numbat::File * file, const string & value);
		Number (numbat::lexer::position pos, const numbat::File * file, const std::shared_ptr <NumericLiteral> & value, const TypePtr & type) : Value (pos, file, type), value (value) {}
		
	protected:
	private:
		
		std::shared_ptr <NumericLiteral> value;
		
};


}
