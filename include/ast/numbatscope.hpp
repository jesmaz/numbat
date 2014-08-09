#ifndef NUMBATSCOPE_HPP
#define NUMBATSCOPE_HPP

#include "astbase.hpp"
#include "memory/numbatvariable.hpp"

#include <map>

namespace numbat {
namespace parser {


class NumbatScope : public ASTbase {
	VISITABLE
	public:
		
		friend ASTnode collectDestructors (const NumbatScope * scope);
		friend ASTnode resolveSymbol (const NumbatScope * scope, const string & iden, ASTnode parent=nullptr, bool cascade=true, bool local=true);
		friend NumbatScope * createChild (NumbatScope * scope);
		friend std::vector <FunctionDecleration *> findFunctions (const NumbatScope * scope, const string & iden);
		
		friend const NumbatType * getType (NumbatScope * scope, const string & iden);
		friend const NumbatVariable * getVariable (NumbatScope * scope, const string & iden);
		
	protected:
	private:
		
		NumbatScope () {}
		
		bool registerSymbol_ (const string & iden, FunctionDecleration * func);
		bool registerSymbol_ (const string & iden, NumbatType * type);
		bool registerSymbol_ (const string & iden, NumbatVariable * var);
		
		bool symbolRegisted (const string & iden);
		
		std::map <string, unique_ptr <NumbatType>> types;
		std::map <string, unique_ptr <NumbatVariable>> variables;
		std::multimap <string, unique_ptr <FunctionDecleration>> functions;
		
		std::set <unique_ptr <NumbatScope>> children;
		
		std::vector <unique_ptr <ASTbase>> body;
		
		NumbatScope * parent=nullptr;;
		
};


}
}

#endif /*NUMBATSCOPE_HPP*/