#ifndef ASTBODY_HPP
#define ASTBODY_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTbody : public ASTbase {
	VISITABLE
	public:
		const std::vector <ASTnode> & getBody () const {return body;}
		virtual bool isValid () const;
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const;
		
		ASTbody () {}
		ASTbody (std::vector <ASTnode> body) : body (body) {}
	private:
		std::vector <ASTnode> body;
};


}
}


#endif /*ASTBODY_HPP*/