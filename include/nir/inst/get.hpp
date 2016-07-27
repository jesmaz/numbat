#pragma once

#include "../instruction.hpp"

namespace nir  {


class Get : public Instruction {
	CONST_VISITABLE
	public:
		
		Argument getSrc () const {return src;}
		
		virtual bool validate () const {return true;}
		
		Get (const Type * type, Argument src, symbol iden) : Instruction ({src}, {type}, {iden}), src (src) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "get." + getType ()->toString () + " " + src.toString ();}
		
		Argument src;
		
		friend Scope;
		
};


};