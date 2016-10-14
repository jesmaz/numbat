#pragma once

#include <cassert>
#include <nir/forward.hpp>
#include <nir/symbol.hpp>
#include <nir/type.hpp>
#include <utility/array.hpp>
#include <visitor.hpp>


namespace nir {

struct Argument {
	symbol sym=nullptr;
	const Type * type=nullptr;
	Argument ()=default;
	Argument (const Instruction * instr, size_t index=0);
	Argument (const Instruction * instr, symbol s);
	Argument (symbol sym, const Type * type) : sym (sym), type (type) {}
	string toString (text::PrintMode mode=text::PLAIN) const;
};

struct Result {
	const Type * type;
	symbol iden;
};

BasicArray <const Type *> argumentToType (const BasicArray <Argument> & args);
BasicArray <symbol> argumentToSymbol (const BasicArray <Argument> & args);

class Instruction : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual bool validate () const=0;
		
		const Type * getType () const {return results [0].type;}
		
		string printIden (text::PrintMode mode=text::PLAIN) const;
		string toString (text::PrintMode mode=text::PLAIN) const;
		symbol getIden () const {return results [0].iden;}
		
		const BasicArray <Argument> & getArguments () const {return arguments;}
		const BasicArray <Result> & getResults () const {return results;}
		
		virtual const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const=0;
		
		virtual const nir::Type * resolveType (const string & iden) const {return nullptr;}
		virtual const nir::Instruction * resolve (nir::Scope * scope, const string & iden) const {return nullptr;}
		
	protected:
		
		Instruction (const BasicArray <Argument> & arguments, const BasicArray <Result> & results) : arguments (arguments), results (results) {
			for (auto & a : arguments) {assert (a.type and a.sym or not a.type and not a.sym);}
		}
		
	private:
		
		virtual string strDump (text::PrintMode mode) const=0;
		
		const BasicArray <Argument> arguments;
		const BasicArray <Result> results;
		
};

};
