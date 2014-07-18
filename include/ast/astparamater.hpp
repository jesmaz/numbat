#ifndef ASTPARAMATER_HPP
#define ASTPARAMATER_HPP

#include "astbase.hpp"
#include "numbattype.hpp"
#include "numbatvariable.hpp"


namespace numbat {
namespace parser {


class ASTparamater : public ASTbase {
	VISITABLE
	public:
		const std::shared_ptr <NumbatVariable> & getVariable () {return variable;}
		virtual const ASTnode getArrayType () const {return variable->getASTType ()->getArrayType ();}
		virtual const ASTnode getASTType () const {return variable->getASTType ();}
		virtual bool isAlias () const {return variable->isAlias ();}
		virtual bool isConst () const {return variable->isConst ();}
		virtual bool isValid () const {return variable != nullptr;}
		virtual shared_ptr <NumbatType> getType () const {return variable->getType ();}
		virtual size_t getBitSize () const {return variable->getSize ();}
		virtual string getIden () const {return variable->getIden ();}
		virtual string toString (const string & indent = "") const {
			return variable->getType ()->toString (indent) + " " + variable->getIden ();
		}
		
		ASTparamater () {}
		ASTparamater (const std::shared_ptr <NumbatVariable> & variable) : variable (variable) {}
	private:
		std::shared_ptr <NumbatVariable> variable;
};


}
}


#endif /*ASTPARAMATER_HPP*/