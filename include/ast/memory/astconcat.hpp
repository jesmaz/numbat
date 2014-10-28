#ifndef ASTCONCAT_HPP
#define ASTCONCAT_HPP

#include "../astbase.hpp"


namespace numbat {
namespace parser {


class ASTconcat : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getLhs () const {return lhs;}
		const ASTnode & getRhs () const {return rhs;}
		const shared_ptr <ASTcallable> & getConv () const {return conv;}
		virtual bool isAlias () const {return false;}
		virtual bool isConst () const {return true;}
		virtual bool isParsed () const {return lhs->isParsed () and rhs->isParsed () and (!conv or conv->isParsed ());}
		virtual bool isValid () const {return lhs->isValid () and rhs->isValid () and (!conv or conv->isValid ());}
		virtual const NumbatType * getType () const {return lhs->getType ();}
		virtual size_t calculateWeight () const {return lhs->calculateWeight () + rhs->calculateWeight () + (conv ? conv->calculateWeight () : 0);}
		virtual size_t getBitSize () const {return lhs->getBitSize ();}
		virtual string getIden () const {return " + ";}
		virtual string toString (const string & indent = "") const {return indent + lhs->toString () + " ~ " + rhs->toString ();}
		
		ASTconcat () {}
		ASTconcat (const ASTnode & lhs, const ASTnode & rhs, const shared_ptr <ASTcallable> & conv = nullptr) : lhs (lhs), rhs (rhs), conv (conv) {}
	private:
		ASTnode lhs, rhs;
		shared_ptr <ASTcallable> conv;
};


}
}


#endif /*ASTCONCAT_HPP*/