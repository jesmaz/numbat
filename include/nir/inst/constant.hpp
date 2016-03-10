#pragma once


#include "../instruction.hpp"
#include "../forward.hpp"

#include <string>


namespace nir {

using std::string;

class Constant : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const string & getVal () const {return val;}
		
	protected:
	private:
		
		Constant (const Type * type, const string & val, symbol iden=nullptr) : Instruction (type, iden), val (val) {}
		
		string val;
		
		friend Scope;
		
		virtual string strDump (text::PrintMode mode) const {return val + "(" + getType ()->toString (mode) + ")";}
		
};


};