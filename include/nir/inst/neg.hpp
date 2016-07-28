#pragma once

#include <nir/instruction.hpp>


namespace nir {


class Neg : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return arg.instr->validate ();}
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 1); return new Neg (replacmentArgs [0], getIden ());}
		
		Argument getArg () const {return arg;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "-" + arg.toString (mode);}
		
		Neg (Argument arg, symbol iden) : Instruction ({arg}, arg.instr->getTypes (), {iden}), arg (arg) {}
		
		Argument arg;
		
		friend Scope;
		
};


};