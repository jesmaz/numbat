#include <fstream>
#include <file.hpp>
#include <iostream>
#include <parse/handparser.hpp>
#include <parse/tree.hpp>


void printHelp (char * cmd) {
	std::cerr << "Usage: " << cmd << " [Option]... [File...]\n";
}

void print (PTNode node) {
	ParseTree * ptree = dynamic_cast <ParseTree *> (node);
	if (ptree) {
		for (PTNode n : ptree->getBody ()) {
			std::cout << n->toString (text::COLOUR) << std::endl;
		}
	}
}

int main (int argl, char ** argc) {
	
	DynArray <char *> files;
	numbat::File dummyFile;
	
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
		
		string line, prog;
		while (std::getline (std::cin, line)) {
			if (not line.empty ()) {
				prog += line + "\n";
			} else if (not prog.empty ()) {
				auto n = parse (prog, &dummyFile);
				print (n);
				delete n;
				std::cout << std::endl;
				prog = line;
			}
		}
		if (not prog.empty()) {
			auto n = parse (prog, &dummyFile);
			print (n);
			delete n;
		}
		
	} else {
		for (char * f : files) {
			std::ifstream fin (f);
			string buff, prog;
			while (std::getline (fin, buff)) prog += buff + "\n";
			std::cout << "########" << f << "########" << std::endl;
			auto n = parse (prog, &dummyFile);
			print (n);
			delete n;
			std::cout << "########" << f << "########" << std::endl;
		}
	}
	
	return 0;
	
}
