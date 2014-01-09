#ifndef ASTCONSTANTINT_HPP
#define ASTCONSTANTINT_HPP

#include "astconstant.hpp"


namespace numbat {
namespace parser {


class ASTconstantInt : public ASTconstant <size_t> {
	VISITABLE
	public:
		const size_t & getValue () const {return val;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return ASTconstant::toString (indent) + " 0";}
		
		ASTconstantInt () {}
		ASTconstantInt (const ASTnode & type, const size_t val) : ASTconstant (type), val (val) {}
	private:
		size_t val;
};


}
}


#endif /*ASTCONSTANTINT_HPP*/