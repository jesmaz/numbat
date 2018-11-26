#pragma once


#include <atomic>
#include <forward.hpp>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <utility/array.hpp>


namespace numbat {


using std::string;


string joinPaths (const string & lhs, const string & rhs);


struct File {
	
	public:
		
		const AST::Context & getContext () const {return *context;}
		const AST::NodePtr & getAST () const {return ast;}
		const string & getDirectory () const {return directory;}
		const string & getFileName () const {return fileName;}
		
		// Returns the built in file (this is where all built in constructs are held)
		static File * builtIn ();
		// Compile the provided file
		static File * compile (const string & path);
		// Searches the system for the requested file and compile it
		static File * import (const string & path);
		// Searches the system and provided path for the requested file and compile it
		static File * import (const string & dir, const string & path);
		
		static std::pair <File *, AST::Context *> newBuiltinModule (const string & name);
		
		static void addIncludeDir (const string & dir) {includeDirs.push_back (dir);}
		
	protected:
	private:
		
		static File * getFile (const string & path);
		
		static bool asyncEnabled;
		static std::atomic_uint files, processed;
		static std::map <string, std::unique_ptr <File>> compiledFiles;
		static std::unique_ptr <File> builtInPtr;
		static DynArray <string> includeDirs;
		
		
		std::unique_ptr <AST::Context> context;;
		AST::NodePtr ast;
		string directory, fileName;
		
};


}
