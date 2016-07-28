#pragma once

#include "forward.hpp"
#include "instruction.hpp"


namespace nir {


class Parameter : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {abort ();}
		
		Argument getDefVal () const {return defVal;}
		
		Parameter (Argument instruction, const string & iden) : Instruction ({instruction}, instruction.instr->getTypes (), {&(this->iden)}), defVal (instruction), iden (iden) {}
		Parameter (const Type * type, const string & iden) : Instruction ({}, {type}, {&(this->iden)}), iden (iden) {}
		
	protected:
	private:
		
		string strDump (text::PrintMode mode) const {return "NYI Parameter.strDump";}
		
		Argument defVal;
		string iden;
		
};


};