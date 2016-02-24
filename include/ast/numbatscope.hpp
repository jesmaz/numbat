#ifndef NUMBATSCOPE_HPP
#define NUMBATSCOPE_HPP

#include "astbase.hpp"
#include "asterror.hpp"
#include "astfunctionlist.hpp"
#include "memory/astmember.hpp"
#include "memory/astvariable.hpp"
#include "memory/numbatvariable.hpp"
#include "type/asttype.hpp"
#include "type/numbatenumtype.hpp"
#include "type/numbatpointertype.hpp"
#include "type/numbatrawtype.hpp"
#include "../context.hpp"

#include <map>

namespace numbat {
namespace parser {


class AbstractSyntaxTree;

class NumbatScope : public ASTbase {
	VISITABLE
	public:
		
		const std::map <string, shared_ptr <NumbatVariable>> & getVariables () const {return variables;}
		const std::multimap <string, shared_ptr <FunctionDecleration>> & getFunctions () const {return functions;}
		const std::set <NumbatEnumType *> & getEnums () const {return enums;}
		const std::vector <ASTnode> & getBody () const {return body;}
		
		virtual AbstractSyntaxTree * getAST () {return parent->getAST();}
		virtual const ASTnode getASTType () const {return body.empty () ? nullptr : body.back ()->getASTType ();}
		virtual ASTnode resolveSymbol (const string & iden) const;
		virtual bool isParsed () const;
		virtual bool isReturned () const {return body.empty () ? false : body.back ()->isReturned ();}
		virtual bool isValid () const;
		virtual const NumbatType * getType () const {return body.empty () ? nullptr : body.back ()->getType ();}
		virtual const std::list <const ASTbase *> getErrors () const;
		virtual size_t calculateWeight () const;
		virtual string getIden () const {return "scope";}
		virtual string toString (const string & indent = "") const;
		
		friend ASTnode collectDestructors (const NumbatScope * scope);
		friend ASTnode resolveSymbol (const NumbatScope * scope, const string & iden, ASTnode parent=nullptr, bool cascade=true, bool local=true);
		friend const NumbatScope * getParent (const NumbatScope * scope);
		friend NumbatScope * createChild (NumbatScope * scope);
		friend ParsingContext * getContext (const NumbatScope * scope);
		friend std::vector <FunctionDecleration *> findFunctions (const NumbatScope * scope, const string & iden);
		
		friend FunctionDecleration * createFunctionDecleration (NumbatScope * scope, const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type, const std::set <string> metaTags, const NumbatScope * fScope);
		friend FunctionDecleration * getFunction (NumbatScope * scope);
		friend NumbatEnumType * createEnum (NumbatScope * scope, const string & iden, const NumbatType * baseType, const std::set <string> & meta);
		friend NumbatType * createRawType (NumbatScope * scope, const string & iden, size_t size, NumbatRawType::Type type);
		friend NumbatType * createStruct (NumbatScope * scope, const string & iden, const std::set <string> & meta);
		friend NumbatType * getArrayType (NumbatScope * scope, const NumbatType * type, size_t dimentions);
		friend NumbatVariable * createVariable (NumbatScope * scope, const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp);
		
		friend const NumbatType * getType (NumbatScope * scope, const string & iden);
		friend const NumbatVariable * getVariable (NumbatScope * scope, const string & iden);
		
		friend void addToBody (NumbatScope * scope, ASTnode n);
		friend void giveNode (NumbatScope * scope, ASTnode n);
		friend void setFunction (NumbatScope * scope, FunctionDecleration * funcDec);
		
	protected:
		
		virtual const NumbatType * getType (const string & iden);
		
		virtual NumbatVariable * createVariable (const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp);
		
		NumbatScope (size_t lineNo, ParsingContext * context) : ASTbase (lineNo), context (context) {}
		
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
		std::set <ASTnode> nodes;
		std::set <NumbatEnumType *> enums;
		std::set <unique_ptr <NumbatScope>> children;
		
		std::vector <ASTnode> body;
		std::vector <std::map <void *, unique_ptr <NumbatPointerType>>> arrayTypes;
		
		NumbatScope * parent=nullptr;
		FunctionDecleration * func=nullptr;
		ParsingContext * context;
		mutable int valid = -1;
		
};


ASTnode collectDestructors (const NumbatScope * scope);
ASTnode resolveSymbol (const NumbatScope * scope, const string & iden, ASTnode parent, bool cascade, bool local);
inline const NumbatScope * getParent (const NumbatScope * scope) {return scope->parent;}
NumbatScope * createChild (NumbatScope * scope);
inline ParsingContext * getContext (const NumbatScope * scope) {return scope->context;}
std::vector <FunctionDecleration *> findFunctions (const NumbatScope * scope, const string & iden);

FunctionDecleration * createFunctionDecleration (NumbatScope * scope, const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type, const std::set <string> metaTags, const NumbatScope * fScope);
inline FunctionDecleration * getFunction (NumbatScope * scope) {return scope->func;}
NumbatEnumType * createEnum (NumbatScope * scope, const string & iden, const NumbatType * baseType, const std::set <string> & meta);
NumbatType * createRawType (NumbatScope * scope, const string & iden, size_t size, NumbatRawType::Type type);
NumbatType * createStruct (NumbatScope * scope, const string & iden, const std::set <string> & meta);
NumbatType * getArrayType (NumbatScope * scope, const NumbatType * type, size_t dimentions);
inline NumbatVariable * createVariable (NumbatScope * scope, const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp) {return scope->createVariable (type, init, iden, global, temp);}

const NumbatType * getType (NumbatScope * scope, const string & iden);
const NumbatVariable * getVariable (NumbatScope * scope, const string & iden);

inline void addToBody (NumbatScope * scope, ASTnode n) {scope->body.push_back (n);}
inline void giveNode (NumbatScope * scope, ASTnode n) {scope->nodes.insert (n);}
inline void setFunction (NumbatScope * scope, FunctionDecleration * funcDec) {scope->func = funcDec;}


}
}

#endif /*NUMBATSCOPE_HPP*/