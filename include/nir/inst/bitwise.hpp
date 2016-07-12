#pragma once


#include <nir/instruction.hpp>


namespace nir {


class BitAnd : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {
			return "bitand." + getType ()->toString (mode) + " " + lhs.toString (mode) + " " + rhs.toString (mode);
		}
		
		BitAnd (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({type}, {iden}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};

class BitOr : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {
			return "bitor." + getType ()->toString (mode) + " " + lhs.toString (mode) + " " + rhs.toString (mode);
		}
		
		BitOr (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({type}, {iden}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};

class BitXor : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		Argument getLhs () const {return lhs;}
		Argument getRhs () const {return rhs;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const {
			return "bitxor." + getType ()->toString (mode) + " " + lhs.toString (mode) + " " + rhs.toString (mode);
		}
		
		BitXor (const Type * type, Argument lhs, Argument rhs, symbol iden) : Instruction ({type}, {iden}), lhs (lhs), rhs (rhs) {}
		
		Argument lhs, rhs;
		
		friend Scope;
		
};


};