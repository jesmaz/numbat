#pragma once

#include "../instruction.hpp"

namespace nir  {


class Get : public Instruction {
	CONST_VISITABLE
	public:
		
		const Instruction * getSrc () const {return src;}
		
		virtual bool validate () const {return true;}
		
		Get (const Type * type, const Instruction * src, symbol iden=nullptr) : Instruction ({type}, {iden}), src (src) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "get." + getType ()->toString () + " " + src->printIden ();}
		
		const Instruction * src;
		
		friend Scope;
		
};


};