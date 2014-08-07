#ifndef NUMBATVARIABLE_HPP
#define NUMBATVARIABLE_HPP

#include "../astbase.hpp"
#include "../type/numbattype.hpp"


namespace numbat {
namespace parser {


struct NumbatVariable {
	public:
		const ASTnode & getASTType () const {return type;}
		const ASTnode & getInit () const {return init;}
		bool isAlias () const {return type->isAlias ();}
		bool isArray () const {return type->isArray ();}
		bool isConst () const {return type->isConst ();}
		bool isGlobal () const {return global;}
		bool isGlobal (bool glo) {return global = glo;}
		bool isTemp () const {return temp;}
		shared_ptr <NumbatType> getType () const {return type->getType ();}
		const size_t getSize () const {return type->getBitSize ();}
		const string & getIden () const {return iden;}
		
		NumbatVariable () {}
		NumbatVariable (const ASTnode & type, const string & iden, bool global=false, bool temp=false) : type (type), init (nullptr), iden (iden), global (global), temp (temp) {}
		NumbatVariable (const ASTnode & type, const ASTnode & init, const string & iden, bool global=false, bool temp=false) : type (type), init (init), iden (iden), global (global), temp (temp) {}
	private:
		ASTnode type, init;
		string iden;
		bool global, temp;
};


}
}


#endif /*ASTBASE_HPP*/