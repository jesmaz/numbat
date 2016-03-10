#pragma once

#include "../function.hpp"
#include "../instruction.hpp"

namespace nir  {


class DirectCall : public Instruction {
	CONST_VISITABLE
	public:
		
		const Function * getFunc () const {return func;}
		const std::vector <const Instruction *> & getArgs () const {return args;}
		const std::vector <symbol> & getIdens () const {return idens;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "call TODO: function names and args";}
		
		DirectCall (const Function * func, const std::vector <const Instruction *> & args, const std::vector <symbol> & idens) : Instruction (func->getType (), nullptr), func (func), args (args), idens (idens) {}
		
		const Function * func;
		std::vector <const Instruction *> args;
		std::vector <symbol> idens;
		
		friend Scope;
		
};


};