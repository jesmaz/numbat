#pragma once


#include <nir/instruction.hpp>


namespace nir {


class Composite : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {return new Composite (getType (), replacmentArgs, getIden ());}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const;
		
		Composite (const Type * type, std::vector <Argument> args, symbol iden) : Instruction (args, {type}, {iden}) {}
		
		friend Scope;
		
};


};
