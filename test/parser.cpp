#include "../include/parse/parser.hpp"
#include "../include/parse/numbatparser.hpp"

#include <iostream>


void printHelp (char * cmd) {
	std::cerr << "Usage: " << cmd << " [Option]... [File...]\n";
}

void print (PTNode node) {
	ParseTree * ptree = dynamic_cast <ParseTree *> (node);
	if (ptree) {
		for (Struct * t : ptree->getStructs ()) {
			std::cout << t->toString () << std::endl;
		}
		for (Function * func : ptree->getFunctions ()) {
			std::cout << func->toString () << std::endl;
		}
		for (PTNode n : ptree->getBody ()) {
			std::cout << n->toString () << std::endl;
		}
	}
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
	
	numbat::NumbatParser parser;
	if (files.empty ()) {
		
		string line, prog;
		while (std::getline (std::cin, line)) {
			if (not line.empty ()) {
				prog += line + "\n";
			} else if (not prog.empty ()) {
				print (parser.parse (prog));
				std::cout << std::endl;
				prog = line;
			}
		}
		if (not prog.empty()) {
			print (parser.parse (prog));
		}
		
	} else {
		std::cerr << "Reading from files not yet supported\n";
	}
	
	return 0;
	
}