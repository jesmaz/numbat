#include "../include/ast.hpp"
#include "../include/module.hpp"

#include <fstream>


namespace numbat {
namespace parser {
using lexer::tkstring;

std::map <string, shared_ptr <Module>> Module::allModules;
std::set <string> Module::includeDirs;

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
	AbstractSyntaxTree ast (tks.begin (), lexer::findEOF (tks.begin (), tks.end ()));
	std::cerr << ast.toString () << std::endl;
	return allModules [file] = shared_ptr <Module> (new Module (ast.getTypes (), ast.getFunctions (), ast.getOperators (), ast.getDependencies (), ast.getStatmentParsers ()));
	
}

const shared_ptr <Module> Module::createFromFile (const string & dir, const string & file) {
	
	return createFromFile (joinPaths (dir, file));
	
}

const shared_ptr <Module> Module::import (const string & file) {
	
	shared_ptr <Module> mod = nullptr;
	if (mod = createFromFile (file + ".nbt")) {
		return mod;
	} else {
		std::cerr << "Could not import '" << file << "'" << std::endl;
		return nullptr;
	}
	
}

const shared_ptr <Module> Module::importLocal (const string & dir, const string & file) {
	
	//TODO: error checking
	return createFromFile (dir, file);
	
}


}
}