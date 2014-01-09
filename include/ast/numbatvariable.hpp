#ifndef NUMBATVARIABLE_HPP
#define NUMBATVARIABLE_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


struct NumbatVariable {
	public:
		bool isAlias () const {return type->isAlias ();}
		bool isConst () const {return type->isConst ();}
		shared_ptr <NumbatType> getType () const {return type->getType ();}
		const size_t getSize () const {return type->getSize ();}
		const string & getIden () const {return iden;}
		
		NumbatVariable () {}
		NumbatVariable (const ASTnode & type, const string & iden) : type (type), iden (iden) {}
	private:
		ASTnode type;
		string iden;
};


}
}


#endif /*ASTBASE_HPP*/