#ifndef ASTIDENTIFIER_HPP
#define ASTIDENTIFIER_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {

class ASTidentifier : public ASTbase {
	VISITABLE
	public:
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return true;}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return iden;}
		virtual string toString (const string & indent = "") const {return indent + iden;}
		
		ASTidentifier () {}
		ASTidentifier (const string & iden) : iden (iden) {}
	private:
		string iden;
};


}
}


#endif /*ASTIDENTIFIER_HPP*/