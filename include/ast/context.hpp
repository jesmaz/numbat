#pragma once


#include <ast/function.hpp>
#include <ast/node.hpp>
#include <ast/type.hpp>
#include <forward.hpp>
#include <map>


namespace AST {


enum class FUNC_MATCH {EXACT, IMPLICIT, POLYMORPHIC, MIS_MATCH};


FUNC_MATCH rateMatch (const Function & func, const BasicArray <TypePtr> & args);
FUNC_MATCH rateMatch (const Function & func, const BasicArray <TypePtr> & args, const std::map <string, TypePtr> & params);


struct Context {
	
	public:
		
		struct function_t {
			std::map <FuncPtr, const Context *> funcs;
			const Context * owner;
		};
		
		struct type_t {
			TypePtr type;
			const Context * owner;
		};
		
		struct variable_t {
			NodePtr var;
			const Context * owner;
		};
		
		static const std::set <FuncPtr> & getAllFunctions () {return allFunctions;}
		
		const numbat::File * getSourceFile () const {return sourceFile;}
		
		Context * getRootContext () const {return rootContex;}
		
		NodePtr resolve (const string & str) const;
		TypePtr resolveType (const string & str) const;
		
		void func (const string & str, FuncPtr func);
		void import (const string & str, Context * ctx);
		void type (const string & str, TypePtr t);
		void var (const string & str, NodePtr v);
		
		std::shared_ptr <function_t> reserveFunc (const string & str);
		std::shared_ptr <type_t> reserveType (const string & str);
		
		Context (const numbat::File * sourceFile);
		             
		Context (const Context & other) : imports (other.imports),
		             functions (other.functions),
		             types (other.types),
		             variables (other.variables),
		             sourceFile (other.sourceFile),
		             ownImp (false),
		             ownFunc (false),
		             ownType (false),
		             ownVar (false),
		             rootContex (this) {}
		             
		Context (const Context & other, const numbat::File * source) : Context (other) {sourceFile = source;}
		
	protected:
	private:
		
		const Context & operator = (const Context &)=delete;
		
		static std::set <FuncPtr> allFunctions;
		std::shared_ptr <std::map <string, Context *>> imports;
		std::shared_ptr <std::map <string, std::shared_ptr <function_t>>> functions;
		std::shared_ptr <std::map <string, std::shared_ptr <type_t>>> types;
		std::shared_ptr <std::map <string, std::shared_ptr <variable_t>>> variables;
		const numbat::File * sourceFile=nullptr;
		bool ownImp, ownFunc, ownType, ownVar;
		
		Context * rootContex=nullptr;
		
};

class Function_Set : public Node {
	
	public:
		
		const Context::function_t & getSet () const {return *set;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Function_Set (numbat::lexer::position pos, const numbat::File * file, const std::shared_ptr <Context::function_t> & set) : Node (pos, file), set (set) {}
		
	protected:
	private:
		
		std::shared_ptr <Context::function_t> set;
		
};

}
