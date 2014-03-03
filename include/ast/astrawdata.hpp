#ifndef ASTRAWDATA_HPP
#define ASTRAWDATA_HPP

#include "astbase.hpp"
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
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getSize () const {return type->getSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTrawdata () {}
		ASTrawdata (const bool ref, const bool constType, const shared_ptr <NumbatType> & type, const std::set <string> & metaTags) : ref (ref), constType (constType), type (type), metaTags (metaTags) {}
	private:
		bool ref, constType;
		shared_ptr <NumbatType> type;
		std::set <string> metaTags;
};


}
}


#endif /*ASTRAWDATA_HPP*/