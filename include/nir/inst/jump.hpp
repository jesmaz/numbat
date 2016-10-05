#pragma once

#include <nir/block.hpp>
#include <nir/instruction.hpp>

namespace nir  {


class Jump : public Instruction {
	CONST_VISITABLE
	public:
		
		Argument getCondition () const {return condition;}
		const Block * getBlock () const {return block;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 1); return new Jump (replacmentArgs [0], block);}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const;
		
		Jump (Argument condition, const Block * block) : Instruction ({condition}, {{nullptr, nullptr}}), condition (condition), block (block) {}
		
		Argument condition;
		const Block * block;
		
		friend Scope;
		
};


};
