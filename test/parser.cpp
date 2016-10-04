#include <fstream>
#include <file.hpp>
#include <iostream>
#include <parse/handparser.hpp>
#include <parse/tree.hpp>
#include <utility/config.hpp>


void printHelp (char * cmd) {
	std::cerr << "Usage: " << cmd << " [Option]... [File...]\n";
}

void print (parser::PTNode node) {
	parser::ParseTree * ptree = dynamic_cast <parser::ParseTree *> (node);
	if (ptree) {
		for (parser::PTNode n : ptree->getBody ()) {
			std::cout << n->toString (text::COLOUR) << std::endl;
		}
	}
}

int numbatMain (const Config & cfg) {
	
	numbat::File dummyFile;
	
	if (cfg.files.empty ()) {
		
		string line, prog;
		while (std::getline (std::cin, line)) {
			if (not line.empty ()) {
				prog += line + "\n";
			} else if (not prog.empty ()) {
				auto n = parser::parse (prog, &dummyFile);
				print (n);
				delete n;
				std::cout << std::endl;
				prog = line;
			}
		}
		if (not prog.empty()) {
			auto n = parser::parse (prog, &dummyFile);
			print (n);
			delete n;
		}
		
	} else {
		for (auto & f : cfg.files) {
			std::ifstream fin (f);
			string buff, prog;
			while (std::getline (fin, buff)) prog += buff + "\n";
			std::cout << "########" << f << "########" << std::endl;
			auto n = parser::parse (prog, &dummyFile);
			print (n);
			delete n;
			std::cout << "########" << f << "########" << std::endl;
		}
	}
	
	return 0;
	
}
