#ifndef ASTCONSTANTFPINT_HPP
#define ASTCONSTANTFPINT_HPP

#include "astconstant.hpp"


namespace numbat {
namespace parser {


class ASTconstantFPInt : public ASTconstant <string> {
	VISITABLE
	public:
		const string & getValue () const {return val;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {std::stringstream ss; ss << ASTconstant::toString (indent) << val; return ss.str ();}
		
		ASTconstantFPInt (size_t lineNo) : ASTconstant (lineNo) {}
		ASTconstantFPInt (size_t lineNo, const ASTnode & type, const string & str) : ASTconstant (lineNo, type), val (str) {}
	private:
		string val;
};


}
}


#endif /*ASTCONSTANTFPINT_HPP*/