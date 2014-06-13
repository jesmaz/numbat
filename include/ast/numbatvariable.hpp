#ifndef NUMBATVARIABLE_HPP
#define NUMBATVARIABLE_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


struct NumbatVariable {
	public:
		const ASTnode & getASTType () const {return type;}
		bool isAlias () const {return type->isAlias ();}
		bool isArray () const {return type->isArray ();}
		bool isConst () const {return type->isConst ();}
		bool isGlobal () const {return global;}
		shared_ptr <NumbatType> getType () const {return type->getType ();}
		const size_t getSize () const {return type->getBitSize ();}
		const string & getIden () const {return iden;}
		
		NumbatVariable () {}
		NumbatVariable (const ASTnode & type, const string & iden, bool global=false) : type (type), iden (iden), global (global) {}
	private:
		ASTnode type;
		string iden;
		bool global;
};


}
}


#endif /*ASTBASE_HPP*/