#ifndef ASTTYPE_HPP
#define ASTTYPE_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTtype : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return ref;}
		virtual bool isArray () const {return type->isArray ();}
		virtual bool isConst () const {return constType;}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTtype () {}
		ASTtype (const bool ref, const bool constType, const shared_ptr <NumbatType> & type) : ref (ref), constType (constType), type (type) {}
	private:
		bool ref, constType;
		shared_ptr <NumbatType> type;
};


}
}


#endif /*ASTTYPE_HPP*/