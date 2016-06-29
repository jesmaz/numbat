#pragma once

#include "../instruction.hpp"

namespace nir  {


class Put : public Instruction {
	CONST_VISITABLE
	public:
		
		const Instruction * getDest () const {return dest;}
		const Instruction * getSrc () const {return src;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "put." + getType ()->toString () + " " + src->printIden () + " " + dest->printIden ();}
		
		Put (const Type * type, const Instruction * src, const Instruction * dest, symbol iden=nullptr) : Instruction ({type}, {iden}), src (src), dest (dest) {}
		
		const Instruction * src, * dest;
		
		friend Scope;
		
};


};