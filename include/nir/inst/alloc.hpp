#pragma once

#include "../instruction.hpp"

namespace nir  {


class Alloc : public Instruction {
	CONST_VISITABLE
	public:
		
		const Instruction * getAmount () const {return amount;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "alloc." + getType ()->toString () + " " + amount->printIden ();}
		
		Alloc (const Type * type, const Instruction * amount, symbol iden=nullptr) : Instruction ({type}, {iden}), amount (amount) {}
		
		const Instruction * amount;
		
		friend Scope;
		
};


};