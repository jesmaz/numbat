#pragma once

#include "../instruction.hpp"

namespace nir  {


class Alloc : public Instruction {
	CONST_VISITABLE
	public:
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 1); return new Alloc (getType (), replacmentArgs [0], getIden ());}
		
		Argument getAmount () const {return amount;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "alloc." + getType ()->toString () + " " + amount.toString (mode);}
		
		Alloc (const Type * type, Argument amount, symbol iden) : Instruction ({amount}, {{type, iden}}), amount (amount) {}
		
		Argument amount;
		
		friend Scope;
		
};


};
