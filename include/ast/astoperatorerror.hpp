#ifndef ASTOPERATORERROR_HPP
#define ASTOPERATORERROR_HPP

#include "asterror.hpp"


namespace numbat {
namespace parser {


class ASToperatorError : public ASTerror {
	public:
		virtual bool isValid () const {return false;}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		
		ASToperatorError () {}
		ASToperatorError (const string & message) : ASTerror (message) {}
	private:
};


}
}


#endif /*ASTOPERATORERROR_HPP*/