#include "../include/ast.hpp"
#include "../include/module.hpp"

#include <fstream>


namespace numbat {
namespace parser {
using lexer::tkstring;

bool Module::debugMode = false;
std::map <string, shared_ptr <Module>> Module::allModules;
std::set <string> Module::includeDirs;
std::vector <ASTnode> Module::main;

tkstring loadFromStream (std::istream & is) {
	std::string out;
	std::string buffer;
	while (std::getline (is, buffer))
		out += buffer + "\n";
	return lexer::lexFile (out);
}

tkstring loadFromFile (const std::string & file) {
	std::ifstream fin (file);
	tkstring ret;
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
		for (auto t : types) {
			if (!t.second->isValid ()) {
				valid = 0;
				if (debugMode) {
					std::cerr << "Invalid type: " << t.second->toString () << std::endl;
				}
			}
		}
		
		for (auto f : functions) {
			if (f.second->getBody ()) {
				if (!f.second->getBody ()->isValid ()) {
					valid = 0;
					if (debugMode) {
						std::cerr << "Invalid function: " << f.second->toString () << std::endl;
					}
				}
			}
		}
		
		for (auto m : dependencies) {
			if (!m->validate ()) {
				valid = 0;
				if (debugMode) {
					for (auto & mod : allModules) {
						if (mod.second == m) {
							std::cerr << "Invalid dependency: " << mod.first << std::endl;
						}
					}
				}
			}
		}
		
	}
	
	return valid > 0;
	
}

const shared_ptr <Module> Module::createEmpty (const string & id) {
	
	auto itt = allModules.find (id);
	if (itt != allModules.end ()) {
		return itt->second;
	}
	return allModules [id] = shared_ptr <Module> (new Module);
	
}

const shared_ptr <Module> Module::createFromFile (const string & file) {
	
	auto itt = allModules.find (file);
	if (itt != allModules.end ()) {
		return itt->second;
	}
	tkstring tks = loadFromFile (file);
	if (tks.empty ()) {
		Module * m = new Module;
		m->valid = -2;
		return shared_ptr <Module> (m);
	}
	size_t pos = file.find_last_of ("/");
	allModules [file] = shared_ptr <Module> (new Module);
	AbstractSyntaxTree ast (tks.begin (), lexer::findEOF (tks.begin (), tks.end ()), file.substr (0, pos != string::npos ? pos : 0));
	if (debugMode) {
		std::cerr << ast.toString () << std::endl;
	}
	main.push_back (ASTnode (new ASTbody (ast.getBody ())));
	*allModules [file] = Module (ast.getTypes (), ast.getFunctions (), ast.getOperators (), ast.getDependencies (), ast.getStatmentParsers ());
	return allModules [file];
	
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


}
}