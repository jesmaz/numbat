#pragma once

#include <nir/instruction.hpp>


namespace nir {


class Neg : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return arg->validate ();}
		
		const Instruction * getArg () const {return arg;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "-" + arg->toString (mode);}
		
		Neg (const Instruction * arg, symbol iden=nullptr) : Instruction (arg->getTypes (), {iden}), arg (arg) {}
		
		const Instruction * arg;
		
		friend Scope;
		
};


};