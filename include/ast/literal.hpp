#pragma once


#include <ast/node.hpp>
#include <cinttypes>
#include <utility/literals.hpp>


namespace AST {


class Value : public Node {
	
	public:
		
		const Literal & getLiteral () const {return literal;}
		
		virtual bool isValue () const {return true;}
		virtual string toString (text::PrintMode mode) const;
		virtual void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		static ValPtr parseNumber (numbat::lexer::position pos, const numbat::File * file, const string & num);
		
		Value (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const Literal & lit) : Node (pos, file, type), literal (lit) {}
		
	protected:
	private:
		
		Literal literal;
		
};


}
