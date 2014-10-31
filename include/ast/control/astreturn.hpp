#ifndef ASTRETURN_HPP
#define ASTRETURN_HPP

#include "../astbase.hpp"
#include "../type/numbattype.hpp"


namespace numbat {
namespace parser {


class ASTreturn : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getExpr () const {return expr;}
		virtual const ASTnode getASTType () const {return expr->getASTType ();}
		virtual bool isAlias () const {return expr->isAlias ();}
		virtual bool isConst () const {return expr->isConst ();}
		virtual bool isParsed () const {return expr->isParsed ();}
		virtual bool isReturned () const {return true;}
		virtual bool isValid () const {return expr->isValid ();}
		virtual const NumbatType * getType () const {return expr->getType ();}
		virtual size_t calculateWeight () const {return expr->calculateWeight ();}
		virtual size_t getBitSize () const {return expr->getBitSize ();}
		virtual string getIden () const {return expr->getIden ();}
		virtual string toString (const string & indent = "") const {return indent + "return " + expr->toString ();}
		
		ASTreturn (size_t lineNo) : ASTbase (lineNo) {}
		ASTreturn (size_t lineNo, const ASTnode expr) : ASTbase (lineNo), expr (expr) {}
	private:
		ASTnode expr;
};

class ASTreturnvoid : public ASTbase {
	VISITABLE
	public:
		virtual bool isParsed () const {return true;}
		virtual bool isReturned () const {return true;}
		virtual bool isValid () const {return true;}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + "return void";}
		
		ASTreturnvoid (size_t lineNo) : ASTbase (lineNo) {}
	private:
};


}
}


#endif /*ASTRETURN_HPP*/