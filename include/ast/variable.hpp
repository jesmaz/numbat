#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>


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
		
		bool isValue () const {return currentValue != nullptr;}
		const string & getIden () const {return identifier;}
		ValPtr & getCurrentValue () {return currentValue;}
		ValPtr getCurrentValue () const {return currentValue;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type);
		Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type, const ValPtr & currentVal);
		
	protected:
	private:
		
		string identifier;
		ValPtr currentValue;
		
};


}
