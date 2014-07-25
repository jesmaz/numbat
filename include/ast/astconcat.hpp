#ifndef ASTCONCAT_HPP
#define ASTCONCAT_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTconcat : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getLhs () const {return lhs;}
		const ASTnode & getRhs () const {return rhs;}
		virtual bool isAlias () const {return false;}
		virtual bool isConst () const {return true;}
		virtual bool isValid () const {return true;}
		virtual size_t calculateWeight () const {return lhs->calculateWeight () + rhs->calculateWeight ();}
		virtual size_t getBitSize () const {return lhs->getBitSize ();}
		virtual string getIden () const {return " + ";}
		virtual string toString (const string & indent = "") const {return indent + lhs->toString () + " + " + rhs->toString ();}
		
		ASTconcat () {}
		ASTconcat (const ASTnode & lhs, const ASTnode & rhs) : lhs (lhs), rhs (rhs) {}
	private:
		ASTnode lhs, rhs;
};


}
}


#endif /*ASTCONCAT_HPP*/