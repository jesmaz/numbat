#ifndef ASTTYPE_HPP
#define ASTTYPE_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTtype : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return alias;}
		virtual bool isConst () const {return constType;}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getSize () const {return type->getSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTtype () {}
		ASTtype (const bool alias, const bool constType, const shared_ptr <NumbatType> & type) : alias (alias), constType (constType), type (type) {}
	private:
		bool alias, constType;
		shared_ptr <NumbatType> type;
};


}
}


#endif /*ASTTYPE_HPP*/