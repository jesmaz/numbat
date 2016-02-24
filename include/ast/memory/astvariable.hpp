#ifndef ASTVARIABLE_HPP
#define ASTVARIABLE_HPP

#include "../astbase.hpp"
#include "../type/numbattype.hpp"
#include "numbatvariable.hpp"


namespace numbat {
namespace parser {


class ASTvariable : public ASTbase {
	VISITABLE
	public:
		const NumbatVariable * getVariable () {return variable;}
		virtual ASTnode resolveSymbol (const string & iden) const {return variable->getASTType ()->resolveSymbol (iden);}
		virtual const ASTnode getArrayType () const {return variable->getASTType ()->getArrayType ();}
		virtual const ASTnode getASTType () const {return variable->getASTType ();}
		virtual bool isAlias () const {return variable->isAlias ();}
		virtual bool isArray () const {return variable->isArray ();}
		virtual bool isConst () const {return variable->isConst ();}
		virtual bool isGlobal () const {return variable->isGlobal ();}
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return variable->getType () and variable->getType ()->isValid () and (!variable->getInit () or variable->getInit ()->isValid ());}
		virtual const NumbatType * getType () const {return variable->getType ();}
		virtual size_t calculateWeight () const {return 1;}
		virtual size_t getBitSize () const {return variable->getSize ();}
		virtual string getIden () const {return variable->getIden ();}
		virtual string toString (const string & indent = "") const {if (variable->isTemp ()) {return indent + "temp (" + variable->getInit ()->toString () + ")";} else {return indent + variable->getIden ();}}
		
		ASTvariable (size_t lineNo) : ASTbase (lineNo), variable (nullptr) {}
		ASTvariable (size_t lineNo, const std::shared_ptr <NumbatVariable> & variable) : ASTbase (lineNo), variable (variable.get ()) {}
		ASTvariable (size_t lineNo, const NumbatVariable * variable) : ASTbase (lineNo), variable (variable) {}
	private:
		const NumbatVariable * variable;
};


}
}


#endif /*ASTVARIABLE_HPP*/