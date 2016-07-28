#pragma once

#include "../function.hpp"
#include "../instruction.hpp"

namespace nir  {


class DirectCall : public Instruction {
	CONST_VISITABLE
	public:
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {return new DirectCall (func, replacmentArgs, getIdens ());}
		
		const Function * getFunc () const {return func;}
		const std::vector <Argument> & getArgs () const {return args;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "call TODO: function names and args";}
		
		DirectCall (const Function * func, const std::vector <Argument> & args, const std::vector <symbol> & idens) : Instruction (args, func->getRetTypes (), idens), func (func), args (args) {}
		
		const Function * func;
		std::vector <Argument> args;
		
		friend Scope;
		
};


};