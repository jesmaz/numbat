#ifndef MODULE_HPP
#define MODULE_HPP

#include "ast/functiondecleration.hpp"
#include "ast/numbattype.hpp"

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
		
		static Module * createFromFile (const string & file);
		static Module * createFromFile (const string & dir, const string & file);
		static Module * import (const string & file);
		static Module * importLocal (const string & dir, const string & file);
		
	protected:
	private:
		
		Module () {}
		
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		
		static std::map <string, Module> allModules;
		static std::set <string> includeDirs;
		
};


}
}

#endif /*MODULE_HPP*/