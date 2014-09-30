#ifndef ASTTYPE_HPP
#define ASTTYPE_HPP

#include "../astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTtype : public ASTbase {
	VISITABLE
	public:
		virtual const ASTnode getArrayType () const {return type->getArrayType ();}
		virtual bool isAlias () const {return ref;}
		virtual bool isArray () const {return type->isArray ();}
		virtual bool isConst () const {return constType;}
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return true;}
		virtual const NumbatType * getType () const {return type;}
		virtual size_t calculateWeight () const {return 1;}
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTtype () {}
		ASTtype (const bool ref, const bool constType, const shared_ptr <NumbatType> & type) : ref (ref), constType (constType), type (type.get ()) {}
		ASTtype (const bool ref, const bool constType, const NumbatType * type) : ref (ref), constType (constType), type (type) {}
	private:
		bool ref, constType;
		const NumbatType * type;
};


}
}


#endif /*ASTTYPE_HPP*/