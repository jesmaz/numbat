#include "../include/ast.hpp"
#include "../include/module.hpp"
#include "../include/parse/numbatparser.hpp"

#include <fstream>


namespace numbat {
namespace parser {
using lexer::tkstring;

bool Module::debugMode = false;
shared_ptr <FunctionDecleration> Module::memalloc = nullptr, Module::memfree = nullptr;
std::map <string, shared_ptr <Module>> Module::allModules;
std::set <string> Module::includeDirs;
std::vector <ASTnode> Module::main;

NumbatParser numbatparser;

PTNode loadFromStream (std::istream & is) {
	std::string out;
	std::string buffer;
	while (std::getline (is, buffer))
		out += buffer + "\n";
	return numbatparser.parse (out);
}

PTNode loadFromFile (const std::string & file) {
	std::ifstream fin (file);
	PTNode ret = nullptr;
	if (fin.is_open ()) {
		ret = loadFromStream (fin);
		fin.close ();
	}
	return ret;
}


string joinPaths (const string & lhs, const string & rhs) {
	
	//TODO: Properly implement this function for all operating systems
	//TODO: Handle paths with '..' and '.'
	if (lhs.empty ()) return rhs;
	if (rhs.empty ()) return lhs;
	string path;
	path.reserve (lhs.size () + rhs.size ());
	if (lhs.back () != '/' and rhs.front () != '/') {
		path = lhs + '/' + rhs;
	} else if (lhs.back () == '/' and rhs.front () == '/') {
		path = lhs + rhs.substr (1);
	} else {
		path = lhs + rhs;
	}
	return path;
	
}


const bool Module::validate () const {
	
	if (valid == -2) return false;
	
	if (0 > valid) {
		
		valid = 1;
		valid = ast->isValid () ? 1 : 0;
		
	}
	
	return valid > 0;
	
}

const shared_ptr <Module> Module::createEmpty (const string & id) {
	
	auto itt = allModules.find (id);
	if (itt != allModules.end ()) {
		return itt->second;
	}
	Module * mod = new Module;
	getContext (mod->getAST ())->file = id;
	return allModules [id] = shared_ptr <Module> (mod);
	
}

const shared_ptr <Module> Module::createFromFile (const string & file) {
	
	auto itt = allModules.find (file);
	if (itt != allModules.end ()) {
		return itt->second;
	}
	
	size_t pos = file.find_last_of ("/");
	const shared_ptr <Module> thisMod = shared_ptr <Module> (new Module);
	allModules [file] = thisMod;
	AbstractSyntaxTree * ast = thisMod->ast;
	if (pos == string::npos) {
		getContext (ast)->file = file;
		getContext (ast)->path = "";
	} else {
		getContext (ast)->file = file.substr (pos+1);
		getContext (ast)->path = file.substr (0, pos);
	}
	
	auto impMod = [&](const shared_ptr <Module> module, bool ext) {
		if (module.get () != thisMod.get ()) {
			ast->importModule (module, ext);
		}
	};
	
	impMod (Module::createEmpty ("numbat core"), true);
	impMod (Module::import (getContext (ast)->path, "core util"), false);
	PTNode node = loadFromFile (file);
	if (node) {
		node->declare (ast);
		//NOTE: This block can be deferred and put in a thread
		{
			node->build (ast);
			delete node;
		}
	} else {
		thisMod->valid = -2;
	}
	
	if (debugMode) {
		std::cerr << ast->toString () << std::endl;
		std::cerr << ASTnode (new ASTbody (0, ast->getBody ()))->toString ("") << std::endl;
	}
	main.push_back (ASTnode (new ASTbody (0, ast->getBody ())));
	thisMod->ast = ast;
	checkForBuiltins (*thisMod);
	return thisMod;
	
}

const shared_ptr <Module> Module::createFromFile (const string & dir, const string & file) {
	
	return createFromFile (joinPaths (dir, file));
	
}

const shared_ptr <Module> Module::import (const string & file) {
	
	return import ("", file);
	
}

const shared_ptr <Module> Module::import (const string & dir, const string & file) {
	
	shared_ptr <Module> mod = nullptr;
	if (-2 < (mod = createFromFile (dir, file + ".nbt"))->valid) {
	} else {
		for (const string & inc : includeDirs) {
			if (-2 < (mod = createFromFile (inc, file + ".nbt"))->valid) {
				return mod;
			}
		}
		std::cerr << "Could not import '" << file << "'" << std::endl;
	}
	return mod;
	
}


void Module::addBrace (const string & beg, const string & end) {
	getContext (ast)->blocks [beg] = end;
}

void Module::addOperator (int precidence, bool ltr, const string & pattern, OperatorDecleration::OperatorParser parser, OperatorDecleration::DefaultImplementation defImp) {
	
	ParsingContext * context = getContext (ast);
	OperatorDecleration oppdec (precidence, ltr, pattern, nullptr, parser, defImp);
	
	if (context->operators.find (pattern) != context->operators.end ()) {
		return;
	}
	
	shared_ptr <OperatorDecleration> opp (new OperatorDecleration (oppdec));
	
	context->operators [pattern] = opp;
	
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			context->operatorsByFirstToken.insert (std::make_pair (s.substr(0,1), opp));
			break;
		}
	}
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			context->keywords.insert(s);
		}
	}
	
}

void Module::checkForBuiltins (Module & mod) {
	
	
	
}

ASTnode ASTmodule::resolveSymbol (const string & iden) const {
	return module->getAST ()->resolveSymbol (iden);
}


Module::Module () {
	ast = new AbstractSyntaxTree ();
}


Module::~Module () {
	if (ast)
		delete ast;
}


}
}