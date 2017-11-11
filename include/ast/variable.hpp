#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>
#include <utility/literals.hpp>


namespace AST {


class StaticIndex : public Node {
	
	public:
		
		const NodePtr & getParent () const {return parent;}
		size_t getIndex () const {return index;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		StaticIndex (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & parent, size_t index) : Node (pos, file, type), parent (parent), index (index) {}
		
	protected:
	private:
		
		NodePtr parent;
		size_t index;
		
};

class Variable : public Node {
	
	public:
		
		bool isValue () const {return not currentValue.isNil ();}
		const string & getIden () const {return identifier;}
		Literal & getCurrentValue () {return currentValue;}
		Literal getCurrentValue () const {return currentValue;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type);
		Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type, const Literal & currentVal);
		
	protected:
	private:
		
		string identifier;
		Literal currentValue;
		
};


}
