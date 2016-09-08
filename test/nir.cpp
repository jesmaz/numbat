#include <fstream>
#include <iostream>
#include <nir/instruction.hpp>
#include <nir/module.hpp>
#include <parse/handparser.hpp>
#include <set>

void printHelp (char * cmd) {
	std::cerr << "Usage: " << cmd << " [Option]... [File...]\n";
}

int main (int argl, char ** argc) {
	
	std::vector <char *> files;
	
	for (int i=1; i<argl; ++i) {
		if (argc [i][0] == '-') {
			if (string (argc [i]) == "--help") {
				printHelp (argc [0]);
				return 0;
			}
		} else {
			files.push_back (argc [i]);
		}
	}
	
	if (files.empty ()) {
		
		nir::Module nirmodule (nullptr);
		auto globalScope = nirmodule.getGlobalScope ();
		
		string line, prog;
		while (std::getline (std::cin, line)) {
			if (not line.empty ()) {
				prog += line + "\n";
			} else if (not prog.empty ()) {
				parse (prog)->build (globalScope);
				std::cout << std::endl;
				prog = line;
			}
		}
		if (not prog.empty()) {
			parse (prog)->build (globalScope);
		}
		
	} else {
		for (char * f : files) {
			nir::Module nirmodule (nullptr);
			auto globalScope = nirmodule.getGlobalScope ();
			std::ifstream fin (f);
			string buff, prog;
			while (std::getline (fin, buff)) prog += buff + "\n";
			std::cout << "########" << f << "########" << std::endl;
			parse (prog)->build (globalScope);
			std::cout << "########" << f << "########" << std::endl;
		}
	}
	
	return 0;
	
}
