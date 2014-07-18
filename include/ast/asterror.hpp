#ifndef ASTERROR_HPP
#define ASTERROR_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTerror : public ASTbase {
	public:
		virtual bool isValid () const {return false;}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + message;}
		
		ASTerror () {}
		ASTerror (const string & message) : message (message) {}
	private:
		string message;
};


}
}


#endif /*ASTERROR_HPP*/