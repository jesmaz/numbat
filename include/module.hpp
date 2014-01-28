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
		
		const std::multimap <string, shared_ptr <FunctionDecleration>> & getFunctions () const {return functions;}
		
		static const Module & createFromFile (const string & file);
		static const Module & createFromFile (const string & dir, const string & file);
		static const Module & import (const string & file);
		static const Module & importLocal (const string & dir, const string & file);
		
	protected:
	private:
		
		Module () {}
		Module (const std::map <string, shared_ptr <NumbatType>> & types, const std::multimap <string, shared_ptr <FunctionDecleration>> & functions, const std::map <string, shared_ptr <OperatorDecleration>> & operators) : types (types), functions (functions), operators (operators) {}
		
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		std::map <string, shared_ptr <OperatorDecleration>> operators;
		
		static std::map <string, Module> allModules;
		static std::set <string> includeDirs;
		
};


}
}

#endif /*MODULE_HPP*/