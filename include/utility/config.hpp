#pragma once

#include <set>
#include <string>
#include <utility/text.hpp>


struct Config {
	
	static Config & mutableGlobalConfig ();
	static const Config & globalConfig ();
	static void initalise (int argl, char ** args);
	
	std::string outfile = "a.out", coreLibDir = "";
	bool const_folding = true;
	bool emitAssembly = false;
	bool emitLLVM = false;
	bool interpret = false;
	bool jit = false;
	bool link = true;
	bool prune_dead_code = true;
	text::PrintMode printMode = text::PLAIN, printModeSTDERR = text::PLAIN;
	std::set <std::string> files;
	
};
