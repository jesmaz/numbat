#ifndef ASTCONSTANT_HPP
#define ASTCONSTANT_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


template <typename T>
class ASTconstant : public ASTbase {
	public:
		virtual bool isAlias () const {return type->isAlias ();}
		virtual bool isConst () const {return true;}
		virtual bool isValid () const {return type->isValid ();}
		virtual const T & getValue () const=0;
		virtual shared_ptr <NumbatType> getType () const {return type->getType ();}
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + type->getIden ();}
		
		ASTconstant () {}
		ASTconstant (const ASTnode & type) : type (type) {}
	private:
		ASTnode type;
};


}
}


#endif /*ASTCONSTANT_HPP*/