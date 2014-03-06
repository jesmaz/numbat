#ifndef MODULE_HPP
#define MODULE_HPP

#include "ast/functiondecleration.hpp"
#include "ast/numbattype.hpp"
#include "ast/operatordecleration.hpp"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace numbat {
namespace parser {

struct AbstractSyntaxTree;

using std::shared_ptr;
using std::string;

struct Module {
	
	public:
		
		const std::map <string, shared_ptr <NumbatType>> & getTypes () const {return types;}
		const std::map <string, shared_ptr <OperatorDecleration>> & getOperators () const {return operators;}
		const std::unordered_map <string, ASTnode(*)(AbstractSyntaxTree *, tkitt)> & getStatmentParsers () const {return statementParsers;}
		const std::multimap <string, shared_ptr <FunctionDecleration>> & getFunctions () const {return functions;}
		const std::set <shared_ptr <Module>> & getDependencies () const {return dependencies;}
		
		void insertType (const string & iden, const shared_ptr <NumbatType> & type) {types [iden] = type;}
		void insertOperator (const string & iden, const shared_ptr <OperatorDecleration> & opp) {operators [iden] = opp;}
		void insertStatmentParser (const string & iden, ASTnode(*parser)(AbstractSyntaxTree *, tkitt)) {statementParsers [iden] = parser;}
		void insertFunction (const string & iden, const shared_ptr <FunctionDecleration> & func) {functions.insert (std::make_pair (iden, func));}
		void insertDependency (const shared_ptr <Module> & module) {dependencies.insert (module);}
		
		static const shared_ptr <Module> createEmpty (const string & id);
		static const shared_ptr <Module> createFromFile (const string & file);
		static const shared_ptr <Module> createFromFile (const string & dir, const string & file);
		static const shared_ptr <Module> import (const string & file);
		static const shared_ptr <Module> importLocal (const string & dir, const string & file);
		
	protected:
	private:
		
		Module () {}
		Module (const std::map <string, shared_ptr <NumbatType>> & types, const std::multimap <string, shared_ptr <FunctionDecleration>> & functions, const std::map <string, shared_ptr <OperatorDecleration>> & operators, const std::set <shared_ptr <Module>> & dependencies, const std::unordered_map <string, ASTnode(*)(AbstractSyntaxTree *, tkitt)> & stmtParse) : types (types), functions (functions), operators (operators), dependencies (dependencies), statementParsers (stmtParse) {}
		
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		std::map <string, shared_ptr <OperatorDecleration>> operators;
		std::set <shared_ptr <Module>> dependencies;
		std::unordered_map <string, ASTnode(*)(AbstractSyntaxTree *, tkitt)> statementParsers;
		
		static std::map <string, shared_ptr <Module>> allModules;
		static std::set <string> includeDirs;
		
};


}
}

#endif /*MODULE_HPP*/