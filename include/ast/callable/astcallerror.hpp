#ifndef ASTCALLERROR_HPP
#define ASTCALLERROR_HPP

#include "astcallable.hpp"


namespace numbat {
namespace parser {

class ASTcallerror : public ASTcallable {
	public:
		virtual bool isValid () const {return false;}
		virtual const FunctionDecleration * getFunction () const {return nullptr;}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + message;}
		
		ASTcallerror () {}
		ASTcallerror (const string & message) : message (message) {}
	private:
		string message;
};


}
}


#endif /*ASTCALLERROR_HPP*/