#ifndef NIR_INST_ADD
#define NIR_INST_ADD


#include "../instruction.hpp"


namespace nir {


class Add : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const;
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {assert (replacmentArgs.size () == 2); return new Add (getType (), replacmentArgs [0], replacmentArgs [1], getIden ());}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const;
		
		Add (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({lhs, rhs}, {{type, iden}}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};


};


#endif/*NIR_INST_ADD*/
