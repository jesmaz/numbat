#ifndef ASTRAWDATA_HPP
#define ASTRAWDATA_HPP

#include "../astbase.hpp"
#include "numbattype.hpp"

#include <set>


namespace numbat {
namespace parser {


class ASTrawdata : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return ref;}
		virtual bool isConst () const {return constType;}
		virtual bool isRaw () const {return true;}
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return true;}
		virtual const NumbatType * getType () const {return type;}
		virtual size_t calculateWeight () const {return 1;}
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTrawdata () {}
		ASTrawdata (const bool ref, const bool constType, const shared_ptr <NumbatType> & type, const std::set <string> & metaTags) : ref (ref), constType (constType), type (type.get ()), metaTags (metaTags) {}
		ASTrawdata (const bool ref, const bool constType, const NumbatType * type, const std::set <string> & metaTags) : ref (ref), constType (constType), type (type), metaTags (metaTags) {}
	private:
		bool ref, constType;
		const NumbatType * type;
		std::set <string> metaTags;
};


}
}


#endif /*ASTRAWDATA_HPP*/