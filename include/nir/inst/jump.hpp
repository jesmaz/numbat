#pragma once

#include <nir/block.hpp>
#include <nir/instruction.hpp>

namespace nir  {


class Jump : public Instruction {
	CONST_VISITABLE
	public:
		
		const Instruction * getCondition () const {return condition;}
		const Block * getBlock () const {return block;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "Jump TODO: destination & condition";}
		
		Jump (const Instruction * condition, const Block * block) : Instruction ({nullptr}, {nullptr}), condition (condition), block (block) {}
		
		const Instruction * condition;
		const Block * block;
		
		friend Scope;
		
};


};