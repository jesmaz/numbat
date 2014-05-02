#ifndef ASTALLOCATE_HPP
#define ASTALLOCATE_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTallocate : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getAmount () const {return amount;}
		virtual bool isAlias () const {return true;}
		virtual bool isConst () const {return true;}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getBitSize () const {return 64;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			std::ostringstream ss; ss << indent << "allocate (" << amount->toString () << ") bytes as " << type->getIden ();
			return ss.str ();
		}
		
		ASTallocate () {}
		ASTallocate (const ASTnode & amount, const shared_ptr <NumbatType> & type) : amount (amount), type (type) {}
	private:
		ASTnode amount;
		shared_ptr <NumbatType> type;
};


}
}


#endif /*ASTALLOCATE_HPP*/