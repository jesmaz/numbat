#pragma once

#include <nir/instruction.hpp>

namespace nir  {


class Ret : public Instruction {
	CONST_VISITABLE
	public:
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {return new Ret (replacmentArgs);}
		
		const BasicArray <Argument> & getArgs () const {return args;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "ret TODO: return values";}
		
		Ret (const BasicArray <Argument> & args) : Instruction ({args}, args.map <Result> ([](auto & t)->Result{return {t.instr->getType (), t.sym};})), args (args) {}
		
		BasicArray <Argument> args;
		
		friend Scope;
		
};


};
