#pragma once

#include "forward.hpp"
#include "instruction.hpp"


namespace nir {


class Parameter : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const Instruction * getDefVal () const {return defVal;}
		
		Parameter (const Instruction * instruction, const string & iden) : Instruction (instruction->getTypes (), {&(this->iden)}), defVal (instruction), iden (iden) {}
		Parameter (const Type * type, const string & iden) : Instruction ({type}, {&(this->iden)}), iden (iden) {}
		
	protected:
	private:
		
		string strDump (text::PrintMode mode) const {return "NYI Parameter.strDump";}
		
		const Instruction * defVal;
		string iden;
		
};


};