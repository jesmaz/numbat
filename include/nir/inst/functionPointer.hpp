#pragma once


#include "../instruction.hpp"
#include "../forward.hpp"
#include "../function.hpp"

#include <string>


namespace nir {

using std::string;

class FunctionPointer : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		const Function * getFunction () const {return func;}
		
	protected:
	private:
		
		FunctionPointer (const Function * func, symbol iden=nullptr) : Instruction ({}, {func->getType ()}, {iden}), func (func) {}
		
		const Function * func;
		
		friend Scope;
		
		virtual string strDump (text::PrintMode mode) const {return "function pointer";}
		
};


};