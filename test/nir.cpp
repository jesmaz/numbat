#include <fstream>
#include <iostream>
#include <nir/block.hpp>
#include <nir/instruction.hpp>
#include <nir/module.hpp>
#include <ast/passes/nir.hpp>
#include <nir/scope.hpp>
#include <parse/handparser.hpp>
#include <set>
#include <utility/config.hpp>
#include <utility/report.hpp>

void printHelp (char * cmd) {
	std::cerr << "Usage: " << cmd << " [Option]... [File...]\n";
}

int numbatMain (const Config & cfg) {
	
	numbat::File dummyFile;
	AST::Context context (&dummyFile);
	
	if (cfg.files.empty ()) {
		
		nir::Module nirmodule;
		auto globalScope = nirmodule.getGlobalScope ();
		auto printItt = globalScope->getCurrentBlock ()->printerBeg ();
		
		string line, prog;
		while (std::getline (std::cin, line)) {
			if (not line.empty ()) {
				prog += line + "\n";
			} else if (not prog.empty ()) {
				auto node = parser::parse (prog, &dummyFile);
				auto ast = node->extendAST (context);
				ast = AST::transform (ast);
				AST::NirPass nirPass (globalScope);
				nirPass (ast);
				while (printItt) {
					std::cout << printItt << '\n';
					++printItt;
				}
				std::cout << std::endl;
				prog = line;
				delete node;
				report::dumpLogs ();
			}
		}
		if (not prog.empty()) {
			auto node = parser::parse (prog, &dummyFile);
			auto ast = node->extendAST (context);
			ast = AST::transform (ast);
			AST::NirPass nirPass (globalScope);
			nirPass (ast);
			while (printItt) {
				std::cout << printItt << '\n';
				++printItt;
			}
			delete node;
			report::dumpLogs ();
		}
		
	} else {
		for (auto & f : cfg.files) {
			nir::Module nirmodule;
			auto globalScope = nirmodule.getGlobalScope ();
			auto printItt = globalScope->getCurrentBlock ()->printerBeg ();
			std::ifstream fin (f);
			string buff, prog;
			while (std::getline (fin, buff)) prog += buff + "\n";
			std::cout << "########" << f << "########" << std::endl;
			auto node = parser::parse (prog, &dummyFile);
			auto ast = node->createAST (context);
			ast = AST::transform (ast);
			AST::NirPass nirPass (globalScope);
			nirPass (ast);
			while (printItt) {
				std::cout << printItt << std::endl;
				++printItt;
			}
			std::cout << "########" << f << "########" << std::endl;
			delete node;
			report::dumpLogs ();
		}
	}
	
	return 0;
	
}
