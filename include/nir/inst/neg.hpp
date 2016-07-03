#pragma once

#include <nir/instruction.hpp>


namespace nir {


class Neg : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return arg.instr->validate ();}
		
		Argument getArg () const {return arg;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "-" + arg.toString (mode);}
		
		Neg (Argument arg, symbol iden=nullptr) : Instruction (arg.instr->getTypes (), {iden}), arg (arg) {}
		
		Argument arg;
		
		friend Scope;
		
};


};