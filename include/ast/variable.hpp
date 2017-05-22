#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>


namespace AST {

class Variable : public Node {
	
	public:
		
		const string & getIden () const {return identifier;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Variable (numbat::lexer::position pos, const string & iden, const TypePtr & type) : Node (pos, type), identifier (iden) {}
		
	protected:
	private:
		
		string identifier;
		
};


}
