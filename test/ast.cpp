#include <fstream>
#include <file.hpp>
#include <iostream>
#include <ast/passes/nir.hpp>
#include <nir/block.hpp>
#include <nir/module.hpp>
#include <nir/scope.hpp>
#include <parse/handparser.hpp>
#include <parse/tree.hpp>
#include <utility/config.hpp>


void printHelp (char * cmd) {
	std::cerr << "Usage: " << cmd << " [Option]... [File...]\n";
}

void print (AST::NodePtr node) {
	std::cout << node->toString (text::COLOUR) << std::endl;
}

int numbatMain (const Config & cfg) {
	
	numbat::File dummyFile;
	AST::Context context (&dummyFile);
	context.func ("foo", std::make_shared <AST::Function> ());
	nir::Module nirmodule;
	auto globalScope = nirmodule.getGlobalScope ();
	auto printItt = globalScope->getCurrentBlock ()->printerBeg ();
	
	if (cfg.files.empty ()) {
		
		string line, prog;
		while (std::getline (std::cin, line)) {
			if (not line.empty ()) {
				prog += line + "\n";
			} else if (not prog.empty ()) {
				auto n = parser::parse (prog, &dummyFile);
				print (AST::transform (n->createAST (context)));
				delete n;
				std::cout << std::endl;
				prog = line;
			}
		}
		if (not prog.empty()) {
			auto n = parser::parse (prog, &dummyFile);
			print (AST::transform (n->createAST (context)));
			delete n;
		}
		
	} else {
		for (auto & f : cfg.files) {
			std::ifstream fin (f);
			string buff, prog;
			while (std::getline (fin, buff)) prog += buff + "\n";
			std::cout << "########" << f << "########" << std::endl;
			auto n = parser::parse (prog, &dummyFile);
			auto tree = AST::transform (n->createAST (context));
			delete n;
			print (tree);
			AST::NirPass nirpass (globalScope);
			nirpass (tree);
			while (printItt) {
				std::cout << printItt << '\n';
				++printItt;
			}
			std::cout << "########" << f << "########" << std::endl;
		}
	}
	
	return 0;
	
}
