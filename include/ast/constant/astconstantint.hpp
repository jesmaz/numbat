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
		virtual string toString (const string & indent = "") const {std::stringstream ss; ss << ASTconstant::toString (indent) << val; return ss.str ();}
		
		ASTconstantInt (size_t lineNo) : ASTconstant (lineNo) {}
		ASTconstantInt (size_t lineNo, const ASTnode & type, const size_t val) : ASTconstant (lineNo, type), val (val) {}
	private:
		size_t val;
};


}
}


#endif /*ASTCONSTANTINT_HPP*/