#pragma once


#include "../instruction.hpp"


namespace nir {


class Div : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const;
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 2); return new Div (getType (), replacmentArgs [0], replacmentArgs [1], getIden ());}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const;
		
		Div (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({lhs, rhs}, {type}, {iden}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};


};