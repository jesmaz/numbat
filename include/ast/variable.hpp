#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>
#include <utility/literals.hpp>
#include <utility/literalstack.hpp>


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

class Value : public Node {
	
	public:
		
		enum class LOCATION {GLOBAL, LOCAL};
		
		Literal & getLiteral (LiteralStack & stack) const {
			switch (location) {
				case LOCATION::GLOBAL:
					return globalContex [stackIndex];
				case LOCATION::LOCAL:
					return stack [stackIndex];
			}
		}
		
		LOCATION getLocation () const {return location;}
		uint32_t getStackIndex () const {return stackIndex;}
		
		virtual bool isValue () const {return true;}
		virtual string toString (text::PrintMode mode) const;
		virtual void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		static ValPtr parseNumber (numbat::lexer::position pos, const numbat::File * file, const string & num);
		
		Value (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const Literal & lit) : Node (pos, file, type), stackIndex (globalContex.reserve ()), location (LOCATION::GLOBAL) {globalContex [stackIndex] = lit;}
		Value (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, uint32_t stackIndex, LOCATION location) : Node (pos, file, type), stackIndex (stackIndex), location (location) {}
		
		static LiteralStack globalContex;
		
	protected:
	private:
		
		uint32_t stackIndex;
		LOCATION location;
		
};

class Variable : public Value {
	
	public:
		
		const string & getIden () const {return identifier;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Variable (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, uint32_t stackIndex, LOCATION location, const string & iden);
		
		
	protected:
	private:
		
		string identifier;
		
};


}
