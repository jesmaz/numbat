#include <file.hpp>
#include <fstream>
#include <ast/context.hpp>
#include <ast/passes/nir.hpp>
#include <nir/module.hpp>
#include <nir/scope.hpp>
#include <mutex>
#include <parse/handparser.hpp>
#include <utility/array.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>


namespace numbat {


parser::PTNode parseFile (std::ifstream & fin, const File * f) {
	
	parser::PTNode parseTree;
	{
		std::string file;
		std::string buffer;
		while (std::getline (fin, buffer))
			file += buffer + "\n";
		parseTree = parser::parse (file, f);
		std::cerr << file << std::endl;
		std::cerr << parseTree->toString () << std::endl;
	}
	return parseTree;
	
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


bool File::asyncEnabled=false;
std::atomic_uint File::files, File::processed;
std::map <string, std::unique_ptr <File>> File::compiledFiles;
std::unique_ptr <File> File::builtInPtr;
DynArray <string> File::includeDirs = {"/usr/include/numbat", "."};

std::mutex fileMutex;

File * File::builtIn () {
	if (not builtInPtr) {
		File * f;
		builtInPtr = std::unique_ptr <File> (f = new File);
		f->directory = "";
		f->fileName = "numbat";
		f->context = std::make_unique <AST::Context> (f);
		auto & cfg = Config::globalConfig ();
		for (auto & s : {"string.nbt"}) {
			auto path = joinPaths (cfg.coreLibDir, s);
			File * lib = getFile (path);
			std::ifstream fin (path);
			if (not fin) {
				report::logMessage (report::Severity::WARNING, f, {0,0}, "Could not find '" + std::string (s) + "' in core library path");
			} else {
				parser::PTNode parseTree = parseFile (fin, lib);
				lib->ast = parseTree->extendAST (*f->context);
				lib->ast = AST::transform (lib->ast);
				delete parseTree;
			}
		}
	}
	return builtInPtr.get ();
}

File * File::compile (const string & path) {
	
	std::ifstream fin;
	File * f = getFile (path);
	
	fin.open (path);
	if (not fin.is_open ()) {
		std::cout << "'" << path << "' not found" << std::endl;
		std::lock_guard <std::mutex> mutLock (fileMutex);
		compiledFiles.erase (path);
		return nullptr;
	}
	
	parser::PTNode parseTree = parseFile (fin, f);
	
	f->context = std::make_unique <AST::Context> (*builtIn ()->context, f);
	f->ast = parseTree->createAST (*f->context);
	f->ast = AST::transform (f->ast);
	delete parseTree;
	return f;
	
}

File * File::getFile (const string & path) {
	
	File * f;
	{
		std::lock_guard <std::mutex> mutLock (fileMutex);
		auto itt = compiledFiles.find (path);
		if (itt != compiledFiles.end ()) {
			return itt->second.get ();
		}
		
		compiledFiles [path] = std::unique_ptr <File> (f = new File);
		
	}
	
	size_t pos = path.find_last_of ("/");
	if (pos == string::npos) {
		f->directory = path;
		f->fileName = "";
	} else {
		f->directory = path.substr (0, pos);
		f->fileName = path.substr (pos+1);
	}
	return f;
	
}

File * File::import (const string & dir, const string & path) {
	
	File * f = compile (joinPaths (dir, path + ".nbt"));
	if (not f) f = import (path);
	return f;
	
}

File * File::import (const string & path) {
	
	File * f;
	for (const string & dir : includeDirs) {
		if ((f = compile (joinPaths (dir, path + ".nbt")))) {
			return f;
		}
	}
	return nullptr;
	
}


}
