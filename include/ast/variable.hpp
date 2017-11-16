#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>
#include <utility/literals.hpp>
#include <utility/literalstack.hpp>


namespace AST {


class Value : public Node {
	
	public:
		
		virtual Literal & getLiteral (LiteralStack & stack) const=0;
		
		virtual bool isValue () const {return true;}
		
		Value (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type) : Node (pos, file, type) {}
		
	protected:
	private:
		
};

class StaticIndex : public Value {
	
	public:
		
		Literal & getLiteral (LiteralStack & stack) const {
			if (parent->isValue ()) {
				return static_cast <Value*> (parent.get ())->getLiteral (stack) [index];
			} else {
				static Literal nilLit;
				nilLit = Literal ();
				return nilLit;
			}
		}
			
		const NodePtr & getParent () const {return parent;}
		size_t getIndex () const {return index;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		StaticIndex (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & parent, size_t index) : Value (pos, file, type), parent (parent), index (index) {assert (parent);}
		
	protected:
	private:
		
		NodePtr parent;
		size_t index;
		
};

class StaticValue : public Value {
	
	public:
		
		Literal & getLiteral (LiteralStack & stack) const {return literal;}
		
		virtual bool isValue () const {return true;}
		virtual string toString (text::PrintMode mode) const {return literal.toString (mode);}
		virtual void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		static ValPtr parseNumber (numbat::lexer::position pos, const numbat::File * file, const string & num);
		
		StaticValue (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const Literal & lit) : Value (pos, file, type), literal (lit) {}
		
	protected:
	private:
		
		mutable Literal literal;
		
};

class Variable : public Value {
	
	public:
		
		enum class LOCATION {GLOBAL, LOCAL};
		
		Literal & getLiteral (LiteralStack & stack) const {
			switch (location) {
				case LOCATION::GLOBAL:
					return globalContex [stackIndex];
				case LOCATION::LOCAL:
					return stack [stackIndex];
			}
			abort ();
		}
		
		LOCATION getLocation () const {return location;}
		uint32_t getStackIndex () const {return stackIndex;}
		
		const string & getIden () const {return identifier;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Variable (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, uint32_t stackIndex, LOCATION location, const string & iden);
		
		static LiteralStack globalContex;
		
		
	protected:
	private:
		
		uint32_t stackIndex;
		LOCATION location;
		string identifier;
		
};


}
