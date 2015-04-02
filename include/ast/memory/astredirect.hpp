#ifndef ASTREDIRECT_HPP
#define ASTREDIRECT_HPP

#include "../astbase.hpp"


namespace numbat {
namespace parser {


class ASTredirect : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getLhs () const {return lhs;}
		const ASTnode & getRhs () const {return rhs;}
		virtual const ASTnode getASTType () const {return lhs->getASTType ();}
		virtual bool isAlias () const {return lhs->isAlias ();}
		virtual bool isConst () const {return lhs->isConst ();}
		virtual bool isParsed () const {return lhs->isParsed ();}
		virtual bool isReturned () const {return true;}
		virtual bool isValid () const {return lhs->isValid () and rhs->isValid ();}
		virtual const NumbatType * getType () const {return lhs->getType ();}
		virtual size_t calculateWeight () const {return rhs->calculateWeight ();}
		virtual size_t getBitSize () const {return lhs->getBitSize ();}
		virtual string getIden () const {return lhs->getIden ();}
		virtual string toString (const string & indent = "") const {return indent + lhs->toString () + " => " + rhs->toString ();}
		
		ASTredirect (size_t lineNo) : ASTbase (lineNo) {}
		ASTredirect (size_t lineNo, const ASTnode lhs, const ASTnode rhs) : ASTbase (lineNo), lhs (lhs), rhs (rhs) {}
	private:
		ASTnode lhs, rhs;
};


}
}


#endif /*ASTREDIRECT_HPP*/