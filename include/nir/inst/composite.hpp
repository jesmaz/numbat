#pragma once


#include <nir/instruction.hpp>


namespace nir {


class Composite : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {return new Composite (getType (), replacmentArgs, getIden ());}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {
			string s = "composite";
			for (const Type * t : getTypes ()) {
				s += "." + t->toString (mode);
			}
			for (Argument arg : getArguments ()) {
				s += " " + arg.toString (mode);
			}
			return s;
		}
		
		Composite (const Type * type, const BasicArray <Argument> & args, symbol iden) : Instruction (args, {type}, {iden}) {}
		
		friend Scope;
		
};


};
