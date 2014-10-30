#ifndef ASTBRANCH_HPP
#define ASTBRANCH_HPP

#include "../astbase.hpp"
#include "astbody.hpp"


namespace numbat {
namespace parser {


class ASTbranch : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getCond () const {return cond;}
		const ASTnode & getBody () const {return body;}
		const ASTnode & getAlt () const {return alt;}
		virtual bool isParsed () const {return cond->isParsed () and body->isParsed () and alt->isParsed ();}
		virtual bool isReturned () const {return body->isReturned () and alt->isReturned ();}
		virtual bool isValid () const {return cond->isValid () and body->isValid () and alt->isValid ();}
		virtual size_t calculateWeight () const {return cond->calculateWeight () + std::max (body->calculateWeight (), alt->calculateWeight ());}
		virtual string getIden () const {return "branch";}
		virtual string toString (const string & indent = "") const {return indent + "branch (" + cond->toString () + ") {" + body->toString (indent + '\t') + "} {" + alt->toString (indent + '\t') + "\n}";}
		
		ASTbranch (size_t lineNo) : ASTbase (lineNo) {}
		ASTbranch (size_t lineNo, const ASTnode & cond, const ASTnode & body) : ASTbranch (lineNo, cond, body, ASTnode (new ASTbody (lineNo))) {}
		ASTbranch (size_t lineNo, const ASTnode & cond, const ASTnode & body, const ASTnode & alt) : ASTbase (lineNo), cond (cond), body (body), alt (alt) {}
	private:
		ASTnode cond, body, alt;
};


}
}


#endif /*ASTBRANCH_HPP*/