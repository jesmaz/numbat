#ifndef ASTCONSTANTCSTRING_HPP
#define ASTCONSTANTCSTRING_HPP

#include "astconstant.hpp"


namespace numbat {
namespace parser {


class ASTconstantCString : public ASTconstant <string> {
	VISITABLE
	public:
		const string & getValue () const {return val;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return ASTconstant::toString (indent) + " \"" + val + "\"";}
		
		ASTconstantCString () {}
		ASTconstantCString (const ASTnode & type, const string & val) : ASTconstant (type), val (val) {}
	private:
		string val;
};


}
}


#endif /*ASTCONSTANTCSTRING_HPP*/