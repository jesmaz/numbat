#pragma once

#include "../instruction.hpp"

namespace nir  {


class Put : public Instruction {
	CONST_VISITABLE
	public:
		
		Argument getDest () const {return dest;}
		Argument getSrc () const {return src;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "put." + getType ()->toString () + " " + src.instr->printIden () + " " + dest.instr->printIden ();}
		
		Put (const Type * type, Argument src, Argument dest, symbol iden) : Instruction ({type}, {iden}), src (src), dest (dest) {}
		
		Argument src, dest;
		
		friend Scope;
		
};


};