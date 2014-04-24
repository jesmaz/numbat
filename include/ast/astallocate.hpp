#ifndef ASTALLOCATE_HPP
#define ASTALLOCATE_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTallocate : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return true;}
		virtual bool isConst () const {return true;}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getSize () const {return 64;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			std::ostringstream ss; ss << indent << "allcate " << amount << " bytes as " << type->getIden ();
			return ss.str ();
		}
		
		ASTallocate () {}
		ASTallocate (size_t amount, const shared_ptr <NumbatType> & type) : amount (amount), type (type) {}
	private:
		size_t amount=0;
		shared_ptr <NumbatType> type;
};


}
}


#endif /*ASTALLOCATE_HPP*/