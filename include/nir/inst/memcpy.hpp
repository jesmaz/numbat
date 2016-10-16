#pragma once


#include <nir/function.hpp>
#include <nir/instruction.hpp>
#include <nir/parameter.hpp>


namespace nir {


class MemCpy : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {
			assert (replacmentArgs.size () == 3);
			return new MemCpy (replacmentArgs [0], replacmentArgs [1], replacmentArgs [2], func, getIden ());
		}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {
			string s = "memcpy " + getArguments () [0].toString (mode) + " " + getArguments () [1].toString (mode) + " " + getArguments () [2].toString (mode);
			if (func) {
				s += " " + func->getLabel ()->iden;
			}
			return s;
		}
		
		MemCpy (Argument src, Argument dest, Argument count, symbol iden) : Instruction ({src, dest, count}, {{src.type, iden}}), func (nullptr) {}
		MemCpy (Argument src, Argument dest, Argument count, const Function * func, symbol iden) : Instruction ({src, dest, count}, {{src.type, iden}}), func (func) {}
		
		const Function * func;
		
		friend Scope;
		
};


};

