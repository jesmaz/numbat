#ifndef NUMBATSCOPE_HPP
#define NUMBATSCOPE_HPP

#include "astbase.hpp"
#include "asterror.hpp"
#include "astfunctionlist.hpp"
#include "memory/astmember.hpp"
#include "memory/astvariable.hpp"
#include "memory/numbatvariable.hpp"
#include "type/asttype.hpp"
#include "type/numbatpointertype.hpp"
#include "type/numbatrawtype.hpp"
#include "../context.hpp"

#include <map>

namespace numbat {
namespace parser {


class NumbatScope : public ASTbase {
	VISITABLE
	public:
		
		virtual ASTnode resolveSymbol (const string & iden) const;
		virtual bool isParsed () const;
		virtual bool isValid () const;
		virtual size_t calculateWeight () const;
		virtual string getIden () const {return "scope";}
		virtual string toString (const string & indent = "") const;
		
		friend ASTnode collectDestructors (const NumbatScope * scope);
		friend ASTnode resolveSymbol (const NumbatScope * scope, const string & iden, ASTnode parent=nullptr, bool cascade=true, bool local=true);
		friend NumbatScope * createChild (NumbatScope * scope);
		friend ParsingContext * getContext (NumbatScope * scope);
		friend std::vector <FunctionDecleration *> findFunctions (const NumbatScope * scope, const string & iden);
		
		friend FunctionDecleration * createFunctionDecleration (NumbatScope * scope, const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type, const std::set <string> metaTags);
		friend NumbatType * createRawType (NumbatScope * scope, const string & iden, size_t size, NumbatRawType::Type type);
		friend NumbatType * createStruct (NumbatScope * scope, const string & iden, const std::set <string> & meta);
		friend NumbatType * getArrayType (NumbatScope * scope, const NumbatType * type, size_t dimentions);
		friend NumbatVariable * createVariable (NumbatScope * scope, const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp);
		
		friend const NumbatType * getType (NumbatScope * scope, const string & iden);
		friend const NumbatVariable * getVariable (NumbatScope * scope, const string & iden);
		
		friend void addToBody (NumbatScope * scope, ASTnode n);
		
	protected:
		
		virtual NumbatVariable * createVariable (const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp);
		
		NumbatScope (ParsingContext * context) : context (context) {}
		
	private:
		
		bool registerSymbol (const string & iden, FunctionDecleration * func);
		bool registerSymbol (const string & iden, NumbatType * type);
		bool registerSymbol (const string & iden, NumbatVariable * var);
		
		bool symbolRegisted (const string & iden);
		
	protected:
		std::map <string, shared_ptr <NumbatType>> types;
		std::map <string, shared_ptr <NumbatVariable>> variables;
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		
	private:
		std::set <unique_ptr <NumbatScope>> children;
		
		std::vector <ASTnode> body;
		std::vector <std::map <void *, unique_ptr <NumbatPointerType>>> arrayTypes;
		
		NumbatScope * parent=nullptr;;
		ParsingContext * context;
		mutable int valid = -1;
		
};


ASTnode collectDestructors (const NumbatScope * scope);
ASTnode resolveSymbol (const NumbatScope * scope, const string & iden, ASTnode parent, bool cascade, bool local);
NumbatScope * createChild (NumbatScope * scope);
inline ParsingContext * getContext (NumbatScope * scope) {return scope->context;}
std::vector <FunctionDecleration *> findFunctions (const NumbatScope * scope, const string & iden);

FunctionDecleration * createFunctionDecleration (NumbatScope * scope, const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type, const std::set <string> metaTags);
NumbatType * createRawType (NumbatScope * scope, const string & iden, size_t size, NumbatRawType::Type type);
NumbatType * createStruct (NumbatScope * scope, const string & iden, const std::set <string> & meta);
NumbatType * getArrayType (NumbatScope * scope, const NumbatType * type, size_t dimentions);
inline NumbatVariable * createVariable (NumbatScope * scope, const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp) {return scope->createVariable (type, init, iden, global, temp);}

const NumbatType * getType (NumbatScope * scope, const string & iden);
const NumbatVariable * getVariable (NumbatScope * scope, const string & iden);

inline void addToBody (NumbatScope * scope, ASTnode n) {scope->body.push_back (n);}


}
}

#endif /*NUMBATSCOPE_HPP*/