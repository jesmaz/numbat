#ifndef MODULE_HPP
#define MODULE_HPP

#include "ast/functiondecleration.hpp"
#include "ast/numbattype.hpp"
#include "ast/operatordecleration.hpp"

#include <map>
#include <memory>
#include <set>
#include <string>

namespace numbat {
namespace parser {


using std::shared_ptr;
using std::string;

struct Module {
	
	public:
		
		const std::map <string, shared_ptr <NumbatType>> & getTypes () const {return types;}
		const std::map <string, shared_ptr <OperatorDecleration>> & getOperators () const {return operators;}
		const std::multimap <string, shared_ptr <FunctionDecleration>> & getFunctions () const {return functions;}
		const std::set <shared_ptr <Module>> & getDependencies () const {return dependencies;}
		
		static const shared_ptr <Module> createFromFile (const string & file);
		static const shared_ptr <Module> createFromFile (const string & dir, const string & file);
		static const shared_ptr <Module> import (const string & file);
		static const shared_ptr <Module> importLocal (const string & dir, const string & file);
		
	protected:
	private:
		
		Module () {}
		Module (const std::map <string, shared_ptr <NumbatType>> & types, const std::multimap <string, shared_ptr <FunctionDecleration>> & functions, const std::map <string, shared_ptr <OperatorDecleration>> & operators, const std::set <shared_ptr <Module>> & dependencies) : types (types), functions (functions), operators (operators), dependencies (dependencies) {}
		
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		std::map <string, shared_ptr <OperatorDecleration>> operators;
		std::set <shared_ptr <Module>> dependencies;
		
		static std::map <string, shared_ptr <Module>> allModules;
		static std::set <string> includeDirs;
		
};


}
}

#endif /*MODULE_HPP*/