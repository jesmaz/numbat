#include "../include/parse/parser.hpp"
#include "../include/parse/numbatparser.hpp"

#include <fstream>
#include <iostream>


class TestParser : public numbat::Parser {
	public:
		TestParser () {
			this->addRules ("PROGRAM", {"L=R", "R"});
			this->addRules ("L", {"*R", "IDENTIFIER"});
			this->addRules ("R", {"L"});
			auto kernals = this->generateKernals ();
			int i = 0;
			for (auto & k : kernals) {
				std::cout << "I" << i << '\n';
				for (auto & e : k.second.expected) {
					std::cout << e.second.reduce << " -> " << k.second.seen << "." << e.first << '\n';
				}
				std::cout << '\n';
				++i;
			}
			this->buildRules ();
		}
};
class TestParser2 : public numbat::Parser {
	public:
		TestParser2 () {
			this->addRules ("PROGRAM", {"E"});
			this->addRules ("E", {"E+T", "T"});
			this->addRules ("T", {"T*F", "F"});
			this->addRules ("F", {"(E)", "IDENTIFIER"});
			auto kernals = this->generateKernals ();
			int i = 0;
			for (auto & k : kernals) {
				std::cout << "I" << i << '\n';
				for (auto & e : k.second.expected) {
					std::cout << e.second.reduce << " -> " << k.second.seen << "." << e.first << '\n';
				}
				std::cout << '\n';
				++i;
			}
			this->buildRules ();
		}
};

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
		for (char * f : files) {
			std::ifstream fin (f);
			string buff, prog;
			while (std::getline (fin, buff)) prog += buff + "\n";
			std::cout << "########" << f << "########" << std::endl;
			print (parser.parse (prog));
			std::cout << "########" << f << "########" << std::endl;
		}
	}
	
	return 0;
	
}