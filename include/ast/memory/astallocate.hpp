#ifndef ASTALLOCATE_HPP
#define ASTALLOCATE_HPP

#include "../astbase.hpp"
#include "../type/numbattype.hpp"


namespace numbat {
namespace parser {


class ASTallocate : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getAmount () const {return amount;}
		virtual bool isAlias () const {return true;}
		virtual bool isConst () const {return true;}
		virtual bool isParsed () const {return amount->isParsed ();}
		virtual bool isValid () const {return amount->isValid ();}
		virtual const NumbatType * getType () const {return type;}
		virtual size_t calculateWeight () const {return 32;}//This is honestly just an arbitrary number.
		virtual size_t getBitSize () const {return 64;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			std::ostringstream ss; ss << indent << "allocate (" << amount->toString () << ") bytes as " << type->getIden ();
			return ss.str ();
		}
		
		ASTallocate (size_t lineNo) : ASTbase (lineNo) {}
		ASTallocate (size_t lineNo, const ASTnode & amount, const shared_ptr <NumbatType> & type) : ASTbase (lineNo), amount (amount), type (type.get ()) {}
		ASTallocate (size_t lineNo, const ASTnode & amount, const NumbatType * type) : ASTbase (lineNo), amount (amount), type (type) {}
	private:
		ASTnode amount;
		const NumbatType * type;
};


}
}


#endif /*ASTALLOCATE_HPP*/