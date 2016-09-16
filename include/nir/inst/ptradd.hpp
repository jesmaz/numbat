#pragma once


#include <nir/instruction.hpp>


namespace nir {


class PtrAdd : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {
			if (param) {
				return new PtrAdd (getType (), replacmentArgs [0], replacmentArgs [1], getIden ());
			} else {
				return new PtrAdd (getType (), replacmentArgs [0], param, getIden ());
			}
		}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {
			string s = "ptradd." + getType ()->toString (mode) + " " + getArguments () [0].toString (mode) + " ";
			if (param) {
				s += *param->getIden ();
			} else {
				s += getArguments () [1].toString (mode);
			}
			return s;
		}
		
		PtrAdd (const Type * type, Argument src, Argument offset, symbol iden) : Instruction ({src, offset}, {type}, {iden}), param (nullptr) {}
		PtrAdd (const Type * type, Argument src, const Parameter * param, symbol iden) : Instruction ({src}, {type}, {iden}), param (param) {}
		
		const Parameter * param;
		
		friend Scope;
		
};


};

