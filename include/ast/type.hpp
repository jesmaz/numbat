#pragma once


#include <ast/node.hpp>
#include <cinttypes>
#include <map>
#include <set>
#include <utility/array.hpp>
#include <vector>


namespace AST {

class Type : public Node {
	
	public:
		
		string getName () const {return name;}
		virtual TypePtr getRegType () const {return nullptr;}
		
		const std::multimap <string, FuncPtr> & getMethods () const {return methods;}
		virtual void overloadFunc (const string & str, const FuncPtr & func) {methods.insert (std::make_pair (str, func));}
		
		Type (numbat::lexer::position pos, const numbat::File * file) : Node (pos, file) {}
		Type (numbat::lexer::position pos, const numbat::File * file, const string & name) : Node (pos, file), name (name) {}
		Type (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type) : Node (pos, file, type) {}
		
	protected:
	private:
		
		string name, fingerprint;
		std::multimap <string, FuncPtr> methods;
		
};


class Inferred : public Type {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Inferred (numbat::lexer::position pos, const numbat::File * file) : Type (pos, file) {}
		
	protected:
	private:
		
		
		
};

class Array : public Type {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		static TypePtr get (const TypePtr & base);
		
	protected:
	private:
		
		Array (numbat::lexer::position pos, const numbat::File * file, const TypePtr & base) : Type (pos, file), base (base) {}
		
		TypePtr base;
		
};

class ArrayInit : public Type {
	
	public:
		
		const NodePtr & getLength () const {return length;}
		const NodePtr & getVal () const {return val;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		ArrayInit (numbat::lexer::position pos, const numbat::File * file, const TypePtr & base, const NodePtr & length, const NodePtr & val) : Type (pos, file, Array::get (base)), base (base), length (length), val (val) {}
		
	protected:
	private:
		
		TypePtr base;
		NodePtr length, val;
		
};

class Const : public Type {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		TypePtr getRegType () const {return type;}
		virtual string toString (text::PrintMode mode) const;
		
		virtual void overloadFunc (const string & str, const FuncPtr & func) {type->overloadFunc (str, func); Type::overloadFunc (str, func);}
		
		static TypePtr get (const TypePtr & base);
		
	protected:
	private:
		
		Const (numbat::lexer::position pos, const numbat::File * file, TypePtr type) : Type (pos, file), type (type) {}
		
		TypePtr type;
		
};

class Ref : public Type {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		TypePtr getRegType () const {return type;}
		virtual string toString (text::PrintMode mode) const;
		
		virtual void overloadFunc (const string & str, const FuncPtr & func) {type->overloadFunc (str, func);}
		
		static TypePtr get (const TypePtr & base);
		
	protected:
	private:
		
		Ref (numbat::lexer::position pos, const numbat::File * file, TypePtr type) : Type (pos, file), type (type) {}
		
		TypePtr type;
		
};

class Interface : public Type {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		Interface (numbat::lexer::position pos, const numbat::File * file) : Type (pos, file) {}
		
	protected:
	private:
		
		std::map <string, std::set <std::shared_ptr <Function>>> methods;
		
};

class Numeric : public Type {
	
	public:
		
		enum ArithmaticType : char {ARBITRARY='a', DECINT='d', INT='i', FPINT='f', UINT='u', UNDETERMINED='n'};
		
		ArithmaticType getArith () const {return arith;}
		uint32_t getMinPrec () const {return minPrecision;}
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		static TypePtr get (ArithmaticType arith, uint32_t minPrecision);
		
	protected:
	private:
		
		Numeric (numbat::lexer::position pos, const numbat::File * file, ArithmaticType arith, uint32_t minPrecision) : Type (pos, file), arith (arith), minPrecision (minPrecision) {}
		
		ArithmaticType arith;
		uint32_t minPrecision;
		
};

class Struct : public Type {
	
	public:
		
		const BasicArray <NodePtr> & getMembers () const {return members;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		string toString (text::PrintMode mode) const;
		
		static TypePtr tuple (const BasicArray <TypePtr> & vals);
		
		Struct (numbat::lexer::position pos, const numbat::File * file, const string & name) : Type (pos, file, name) {}
		
	protected:
	private:
		
		friend parser::Struct;
		
		std::map <string, size_t> positionMap;
		BasicArray <NodePtr> members;
		
};

}
