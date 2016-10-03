#pragma once

#include <cassert>
#include <nir/forward.hpp>
#include <nir/type.hpp>
#include <utility/array.hpp>
 
#include <visitor.hpp>


namespace nir {

struct Argument {
	const Instruction * instr=nullptr;
	symbol sym=nullptr;
	string toString (text::PrintMode mode=text::PLAIN) const;
};

BasicArray <const Type *> argumentToType (const BasicArray <Argument> & args);
BasicArray <symbol> argumentToSymbol (const BasicArray <Argument> & args);

class Instruction : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual bool validate () const=0;
		
		const Type * getType () const {return types [0];}
		
		string printIden (text::PrintMode mode=text::PLAIN) const;
		string toString (text::PrintMode mode=text::PLAIN) const;
		symbol getIden () const {return idens [0];}
		
		const BasicArray <Argument> & getArguments () const {return arguments;}
		const BasicArray <const Type *> & getTypes () const {return types;}
		const BasicArray <symbol> & getIdens () const {return idens;}
		
		virtual const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const=0;
		
		virtual const nir::Type * resolveType (const string & iden) const {return nullptr;}
		virtual const nir::Instruction * resolve (nir::Scope * scope, const string & iden) const {return nullptr;}
		
	protected:
		
		Instruction (const BasicArray <Argument> & arguments, const BasicArray <const Type *> & types, const BasicArray <symbol> & idens) : arguments (arguments), types (types), idens (idens) {assert (types.size () == idens.size ());}
		
	private:
		
		virtual string strDump (text::PrintMode mode) const=0;
		
		BasicArray <Argument> arguments;
		BasicArray <const Type *> types;
		BasicArray <symbol> idens;
		
};

};
