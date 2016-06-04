#include <file.hpp>
#include <nir/scope.hpp>
#include <parse/handparser.hpp>
#include <mutex>
#include <fstream>
#include <parse/handparser.hpp>


namespace numbat {


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
std::vector <string> File::includeDirs = {"/usr/include/numbat"};

std::mutex fileMutex;

File * File::compile (const string & path, nir::Module * module) {
	
	std::ifstream fin;
	File * f;
	{
		std::lock_guard <std::mutex> mutLock (fileMutex);
		auto itt = compiledFiles.find (path);
		if (itt != compiledFiles.end ()) {
			return itt->second.get ();
		}
		
		fin.open (path);
		if (not fin.is_open ()) {
			std::cout << "'" << path << "' not found" << std::endl;
			return nullptr;
		}
		
		compiledFiles [path] = std::unique_ptr <File> (f = new File);
		
	}
	
	size_t pos = path.find_last_of ("/");
	if (pos == string::npos) {
		f->directory = path;
		f->fileName = "";
	} else {
		f->directory = path.substr (pos+1);
		f->fileName = path.substr (0, pos);
	}
	
	PTNode parseTree;
	{
		std::string file;
		std::string buffer;
		while (std::getline (fin, buffer))
			file += buffer + "\n";
		parseTree = parse (file);
		std::cerr << file << std::endl;
		std::cerr << parseTree->toString () << std::endl;
	}
	
	nir::Scope * root = module->createRootScope ();
	parseTree->declare (root);
	parseTree->build (root, ParseTreeNode::BuildMode::NORMAL);
	f->scope = root;
	return f;
	
}

File * File::import (const string & dir, const string & path, nir::Module * module) {
	
	File * f = compile (joinPaths (dir, path + ".nbt"), module);
	if (not f) f = import (path, module);
	return f;
	
}

File * File::import (const string & path, nir::Module * module) {
	
	File * f;
	for (const string & dir : includeDirs) {
		if (f = compile (joinPaths (dir, path + ".nbt"), module)) {
			return f;
		}
	}
	return nullptr;
	
}


}