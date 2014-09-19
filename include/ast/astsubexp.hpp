#ifndef ASTSUBEXP_HPP
#define ASTSUBEXP_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTsubexp : public ASTbase {
	public:
		virtual ASTnode resolveSymbol (const string & iden) const {return exp->resolveSymbol(iden);}
		virtual const ASTnode getArrayType () const {return exp->getArrayType();}
		virtual const ASTnode getASTType () const {return exp->getASTType();}
		virtual bool isAlias () const {return exp->isAlias();}
		virtual bool isArray () const {return exp->isArray();}
		virtual bool isCallable () const {return exp->isCallable();}
		virtual bool isConst () const {return exp->isConst();}
		virtual bool isList () const {return exp->isList();}
		virtual bool isNil () const {return exp->isNil();}
		virtual bool isRaw () const {return exp->isRaw();}
		virtual bool isValid () const {return exp->isValid();}
		virtual const NumbatType * getType () const {return exp->getType();}
		virtual size_t calculateWeight () const {return exp->calculateWeight();}
		//DEPRECATED
		virtual size_t getBitSize () const {return exp->getBitSize();}
		virtual const std::list <ASTnode> getList () const {return exp->getList();}
		virtual string getIden () const {return exp->getIden();}
		virtual string toString (const string & indent = "") const {return exp->toString(indent);}
		virtual ssize_t getLengthIndex () const {return exp->getLengthIndex();}
		ASTsubexp () {}
		ASTsubexp (ASTnode exp) : exp (exp) {}
	protected:
	private:
		ASTnode exp;
};


}
}


#endif /*ASTSUBEXP_HPP*/