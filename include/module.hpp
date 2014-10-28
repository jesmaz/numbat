#ifndef MODULE_HPP
#define MODULE_HPP

#include "ast/control/astbody.hpp"
#include "ast/functiondecleration.hpp"
#include "ast/type/numbattype.hpp"
#include "ast/numbatcontext.hpp"
#include "ast/operatordecleration.hpp"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace numbat {
namespace parser {

using std::shared_ptr;
using std::string;


string joinPaths (const string & lhs, const string & rhs);

struct Module {
	
	public:
		
		AbstractSyntaxTree * getAST () {return ast;}
		const AbstractSyntaxTree * getAST () const {return ast;}
		
		const bool validate () const;
		
		void addBrace (const string & beg, const string & end);
		void addOperator (int precidence, bool ltr, const string & pattern, OperatorDecleration::OperatorParser parser, OperatorDecleration::DefaultImplementation defImp = nullptr);
		
		static const shared_ptr <FunctionDecleration> & getFree () {return memfree;}
		static const shared_ptr <FunctionDecleration> & getMalloc () {return memalloc;}
		
		static const shared_ptr <Module> createEmpty (const string & id);
		static const shared_ptr <Module> createFromFile (const string & file);
		static const shared_ptr <Module> createFromFile (const string & dir, const string & file);
		static const shared_ptr <Module> import (const string & file);
		static const shared_ptr <Module> import (const string & dir, const string & file);
		
		static const std::vector <ASTnode> & getMain () {return main;}
		
		static void setDebugMode (bool b) {debugMode = b;}
		static void addIncludeDir (const string & dir) {includeDirs.insert (dir);}
		
		~Module ();
		
	protected:
	private:
		
		Module ();
		
		AbstractSyntaxTree * ast;
		
		
		static void checkForBuiltins (Module & mod);
		
		static bool debugMode;
		static shared_ptr <FunctionDecleration> memalloc, memfree;
		static std::map <string, shared_ptr <Module>> allModules;
		static std::set <string> includeDirs;
		static std::vector <ASTnode> main;
		mutable int8_t valid = -1;
		
};


}
}

#endif /*MODULE_HPP*/