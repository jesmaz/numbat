#pragma once


#include <nir/instruction.hpp>


namespace nir {


class Reinterpret : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Reinterpret * recreate (const std::vector <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 1); return new Reinterpret (getType (), replacmentArgs [0], getIden ());}
		
		Argument getArg () const {return arg;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "reinterpret." + getType ()->toString (mode) + " " + arg.toString (mode);}
		
		Reinterpret (const Type * type, Argument arg, symbol iden) : Instruction ({arg}, {type}, {iden}), arg (arg) {}
		
		Argument arg;
		
		friend Scope;
		
};


};
