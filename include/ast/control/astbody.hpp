#ifndef ASTBODY_HPP
#define ASTBODY_HPP

#include "../astbase.hpp"


namespace numbat {
namespace parser {


class ASTbody : public ASTbase {
	VISITABLE
	public:
		const std::vector <ASTnode> & getBody () const {return body;}
		virtual bool isParsed () const;
		virtual bool isReturned () const {return body.empty () ? false : body.back ()->isReturned ();}
		virtual bool isValid () const;
		virtual const std::list <const ASTbase *> getErrors () const {std::list <const ASTbase *> err; for (const auto & e : body) {auto r = e->getErrors (); err.splice (err.end (), r);} return err;}
		virtual size_t calculateWeight () const;
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const;
		
		ASTbody (size_t lineNo) : ASTbase (lineNo) {}
		ASTbody (size_t lineNo, std::vector <ASTnode> body) : ASTbase (lineNo), body (body) {}
		ASTbody (size_t lineNo, const ASTnode & body) : ASTbase (lineNo), body (1, body) {}
	private:
		std::vector <ASTnode> body;
};


}
}


#endif /*ASTBODY_HPP*/