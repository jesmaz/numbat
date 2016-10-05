#pragma once


#include <nir/instruction.hpp>


namespace nir {


class Equal : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 2); return new Equal (getType (), replacmentArgs [0], replacmentArgs [1], getIden ());}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "equal." + getType ()->toString (mode) + " " + lhs.toString (mode) + " " + rhs.toString (mode);}
		
		Equal (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({lhs, rhs}, {{type, iden}}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};


};
