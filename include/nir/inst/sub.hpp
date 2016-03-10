#ifndef NIR_INST_SUB
#define NIR_INST_SUB


#include "../instruction.hpp"


namespace nir {


class Sub : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const;
		
		const Instruction * getLhs () const {return lhs;}
		const Instruction * getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const;
		
		Sub (const Type * type, const Instruction * lhs, const Instruction * rhs, symbol iden=nullptr) : Instruction (type, iden), lhs (lhs), rhs (rhs) {}
		
		const Instruction * lhs, * rhs;
		
		friend Scope;
		
};


};


#endif/*NIR_INST_ADD*/