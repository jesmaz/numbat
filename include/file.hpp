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


struct File {
	
	public:
		
		const nir::Scope * getScope () const {return scope;}
		const string & getDirectory () const {return directory;}
		const string & getFileName () const {return fileName;}
		
		// Compile the provided file
		static File * compile (const string & path, nir::Module * module);
		// Searches the system for the requested file and compile it
		static File * import (const string & path, nir::Module * module);
		// Searches the system and provided path for the requested file and compile it
		static File * import (const string & dir, const string & path, nir::Module * module);
		
		static void addIncludeDir (const string & dir) {includeDirs.push_back (dir);}
		
	protected:
	private:
		
		static bool asyncEnabled;
		static std::atomic_uint files, processed;
		static std::map <string, std::unique_ptr <File>> compiledFiles;
		static DynArray <string> includeDirs;
		
		nir::Scope * scope;
		string directory, fileName;
		
};


}
