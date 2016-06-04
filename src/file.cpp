#include <file.hpp>
#include <nir/scope.hpp>
#include <parse/handparser.hpp>
#include <mutex>
#include <fstream>
#include <parse/handparser.hpp>


namespace numbat {


bool File::asyncEnabled=false;
std::atomic_uint File::files, File::processed;
std::map <string, std::unique_ptr <File>> File::compiledFiles;
std::vector <string> File::includeDirs;

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
			return nullptr;
		}
		
		compiledFiles [path] = std::unique_ptr <File> (f = new File);
		
	}
	
	size_t pos = path.find_last_of ("/");
	if (pos == string::npos) {
		f->directory = path;
		f->directory = "";
	} else {
		f->directory = path.substr (pos+1);
		f->directory = path.substr (0, pos);
	}
	
	PTNode parseTree;
	{
		const size_t buffer_size = 1024*4;
		char buffer [buffer_size];
		std::string file;
		size_t n;
		while (fin.read (buffer, buffer_size)) {
			n = fin.gcount ();
			file.append (buffer, n);
		}
		parseTree = parse (file);
	}
	
	nir::Scope * root = module->createRootScope ();
	parseTree->declare (root);
	parseTree->build (root, ParseTreeNode::BuildMode::NORMAL);
	f->scope = root;
	return f;
	
}

File * File::import (const string & dir, const string & path, nir::Module * module) {
	
	File * f = compile (dir + path, module);
	if (not f) f = import (path, module);
	return f;
	
}

File * File::import (const string & path, nir::Module * module) {
	
	File * f;
	for (const string & dir : includeDirs) {
		if (f = compile (dir + path, module)) {
			return f;
		}
	}
	return nullptr;
	
}


}