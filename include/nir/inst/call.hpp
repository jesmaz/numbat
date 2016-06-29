#pragma once

#include "../function.hpp"
#include "../instruction.hpp"

namespace nir  {


class DirectCall : public Instruction {
	CONST_VISITABLE
	public:
		
		const Function * getFunc () const {return func;}
		const std::vector <const Instruction *> & getArgs () const {return args;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "call TODO: function names and args";}
		
		DirectCall (const Function * func, const std::vector <const Instruction *> & args, const std::vector <symbol> & idens) : Instruction (func->getRetTypes (), idens), func (func), args (args) {}
		
		const Function * func;
		std::vector <const Instruction *> args;
		
		friend Scope;
		
};


};