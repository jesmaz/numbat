#ifndef ASTMEMBER_HPP
#define ASTMEMBER_HPP

#include "../astbase.hpp"
#include "../type/numbattype.hpp"


namespace numbat {
namespace parser {


class ASTmember : public ASTbase {
	VISITABLE
	public:
		virtual ASTnode resolveSymbol (const string & iden) const {return type->resolveSymbol (iden);}
		virtual const ASTnode getArrayType () const {return type->getArrayType ();}
		virtual const ASTnode getASTType () const {return type;}
		virtual bool isAlias () const {return type->isAlias ();}
		virtual bool isArray () const {return type->isArray ();}
		virtual bool isConst () const {return type->isConst ();}
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return true;}
		virtual const NumbatType * getType () const {return type->getType ();}
		virtual size_t calculateWeight () const {return 1;}
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return iden;}
		virtual string toString (const string & indent = "") const {return indent + iden;}
		
		ASTmember (size_t lineNo) : ASTbase (lineNo) {}
		ASTmember (size_t lineNo, const string & iden, const ASTnode & type) : ASTbase (lineNo), iden (iden), type (type) {}
	private:
		string iden;
		ASTnode type;
};


}
}


#endif /*ASTMEMBER_HPP*/