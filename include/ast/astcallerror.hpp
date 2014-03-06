#ifndef ASTCALLERROR_HPP
#define ASTCALLERROR_HPP

#include "astcallable.hpp"


namespace numbat {
namespace parser {

class ASTcallerror : public ASTcallable {
	public:
		virtual bool isValid () const {return false;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const {return func;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + message;}
		
		ASTcallerror () {}
		ASTcallerror (const string & message) : message (message) {}
	private:
		string message;
		shared_ptr <FunctionDecleration> func = nullptr;
};


}
}


#endif /*ASTCALLERROR_HPP*/