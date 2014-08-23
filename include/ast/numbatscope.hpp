#ifndef NUMBATSCOPE_HPP
#define NUMBATSCOPE_HPP

#include "astbase.hpp"
#include "asterror.hpp"
#include "astfunctionlist.hpp"
#include "memory/astvariable.hpp"
#include "memory/numbatvariable.hpp"
#include "type/asttype.hpp"

#include <map>

namespace numbat {
namespace parser {


class NumbatScope : public ASTbase {
	VISITABLE
	public:
		
		virtual ASTnode resolveSymbol (const string & iden) const;
		virtual bool isValid () const;
		virtual size_t calculateWeight () const;
		virtual string getIden () const {return "scope";}
		virtual string toString (const string & indent = "") const;
		
		friend ASTnode collectDestructors (const NumbatScope * scope);
		friend ASTnode resolveSymbol (const NumbatScope * scope, const string & iden, ASTnode parent=nullptr, bool cascade=true, bool local=true);
		friend NumbatScope * createChild (NumbatScope * scope);
		friend std::vector <FunctionDecleration *> findFunctions (const NumbatScope * scope, const string & iden);
		
		friend const NumbatType * getType (NumbatScope * scope, const string & iden);
		friend const NumbatVariable * getVariable (NumbatScope * scope, const string & iden);
		
	protected:
		
		virtual NumbatVariable * createVariable (const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp);
		
		NumbatScope () {}
		
	private:
		
		bool registerSymbol (const string & iden, FunctionDecleration * func);
		bool registerSymbol (const string & iden, NumbatType * type);
		bool registerSymbol (const string & iden, NumbatVariable * var);
		
		bool symbolRegisted (const string & iden);
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::map <string, shared_ptr <NumbatVariable>> variables;
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		
		std::set <unique_ptr <NumbatScope>> children;
		
		std::vector <ASTnode> body;
		
		NumbatScope * parent=nullptr;;
		mutable int valid = -1;
		
};


}
}

#endif /*NUMBATSCOPE_HPP*/