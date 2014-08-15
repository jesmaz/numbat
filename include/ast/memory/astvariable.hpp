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
		const std::shared_ptr <NumbatVariable> & getVariable () {return variable;}
		virtual const ASTnode getArrayType () const {return variable->getASTType ()->getArrayType ();}
		virtual const ASTnode getASTType () const {return variable->getASTType ();}
		virtual bool isAlias () const {return variable->isAlias ();}
		virtual bool isArray () const {return variable->isArray ();}
		virtual bool isConst () const {return variable->isConst ();}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return variable->getType ();}
		virtual size_t calculateWeight () const {return 1;}
		virtual size_t getBitSize () const {return variable->getSize ();}
		virtual string getIden () const {return variable->getIden ();}
		virtual string toString (const string & indent = "") const {return indent + variable->getIden ();}
		
		ASTvariable () {}
		ASTvariable (const std::shared_ptr <NumbatVariable> & variable) : variable (variable) {}
	private:
		std::shared_ptr <NumbatVariable> variable;
};


}
}


#endif /*ASTVARIABLE_HPP*/