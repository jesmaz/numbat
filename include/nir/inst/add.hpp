#ifndef NIR_INST_ADD
#define NIR_INST_ADD


#include "../instruction.hpp"


namespace nir {


class Add : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const;
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const;
		
		Add (const Type * type, Argument lhs, Argument rhs, symbol iden=nullptr) : Instruction ({type}, {iden}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};


};


#endif/*NIR_INST_ADD*/