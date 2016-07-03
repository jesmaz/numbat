#pragma once

#include <set>
#include <string>


struct Config {
	
	static const Config & globalConfig ();
	static void initalise (int argl, char ** args);
	
	std::string outfile = "a.out";
	bool emitAssembly = false;
	bool link = true;
	bool emitLLVM = false;
	bool jit = false;
	std::set <std::string> files;
	
};