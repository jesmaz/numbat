#pragma once

#include <nir/instruction.hpp>

namespace nir  {


class Ret : public Instruction {
	CONST_VISITABLE
	public:
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {return new Ret (replacmentArgs);}
		
		const std::vector <Argument> & getArgs () const {return args;}
		
		virtual bool validate () const {return true;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {return "ret TODO: return values";}
		
		Ret (const std::vector <Argument> & args) : Instruction ({args}, argumentToType (args), argumentToSymbol (args)), args (args) {}
		
		std::vector <Argument> args;
		
		friend Scope;
		
};


};