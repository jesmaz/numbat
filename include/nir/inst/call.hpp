#pragma once

#include "../function.hpp"
#include "../instruction.hpp"

namespace nir  {


class DirectCall : public Instruction {
	CONST_VISITABLE
	public:
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {return new DirectCall (func, replacmentArgs, getResults ().map <symbol> ([](auto & t){return t.iden;}));}
		
		const Function * getFunc () const {return func;}
		const BasicArray <Argument> & getArgs () const {return args;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "call TODO: function names and args";}
		
		DirectCall (const Function * func, const BasicArray <Argument> & args, const BasicArray <symbol> & idens) : Instruction (args, combine <Result> (func->getRetTypes (), idens)), func (func), args (args) {}
		
		const Function * func;
		BasicArray <Argument> args;
		
		friend Scope;
		
};


};
