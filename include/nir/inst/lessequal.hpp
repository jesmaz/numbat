#pragma once


#include <nir/instruction.hpp>


namespace nir {


class LessEqual : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 2); return new LessEqual (getType (), replacmentArgs [0], replacmentArgs [1], getIden ());}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "lessequal." + getType ()->toString (mode) + " " + lhs.toString (mode) + " " + rhs.toString (mode);}
		
		LessEqual (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({lhs, rhs}, {type}, {iden}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};


};
