#ifndef ASTBRANCH_HPP
#define ASTBRANCH_HPP

#include "astbase.hpp"
#include "astbody.hpp"


namespace numbat {
namespace parser {


class ASTbranch : public ASTbase {
	VISITABLE
	public:
		virtual bool isValid () const {return true;}
		virtual string getIden () const {return "branch";}
		virtual string toString (const string & indent = "") const {return indent + "branch (" + cond->toString () + ") {" + body->toString (indent + '\t') + "} {" + alt->toString (indent + '\t') + "\n}";}
		
		ASTbranch () {}
		ASTbranch (const ASTnode & cond, const ASTnode & body) : ASTbranch (cond, body, ASTnode (new ASTbody ())) {}
		ASTbranch (const ASTnode & cond, const ASTnode & body, const ASTnode & alt) : cond (cond), body (body), alt (alt) {}
	private:
		ASTnode cond, body, alt;
};


}
}


#endif /*ASTBRANCH_HPP*/