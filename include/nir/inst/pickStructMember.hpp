#pragma once


#include <nir/instruction.hpp>


namespace nir {


class PickStructMember : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 1); return new PickStructMember (getType (), replacmentArgs [0], index, memIden, getIden ());}
		
		Argument getParent () const {return parent;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return parent.toString (mode) + "." + memIden;}
		
		PickStructMember (const Type * type, Argument parent, size_t index, const string & memIden, symbol iden) : Instruction ({parent}, {{type, iden}}), parent (parent), index (index), memIden (memIden) {}
		
		Argument parent;
		size_t index;
		string memIden;
		
		friend Scope;
		
};


};
