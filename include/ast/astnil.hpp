#ifndef ASTNIL_HPP
#define ASTNIL_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTnil : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return false;}
		virtual bool isConst () const {return false;}
		virtual bool isNil () const {return true;}
		virtual bool isValid () const {return true;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "nil";}
		virtual string toString (const string & indent = "") const {return indent + "nil";}
		
		ASTnil () {}
	private:
};


}
}


#endif /*ASTNIL_HPP*/