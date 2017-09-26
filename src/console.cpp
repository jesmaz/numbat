#include <ast/passes/execute.hpp>
#include <iostream>
#include <nir/module.hpp>
#include <parse/handparser.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>



int numbatMain (const Config & cfg) {
	
	nir::Module nirmodule;
	auto globalScope = nirmodule.getGlobalScope ();
	numbat::File dummyFile;
	AST::Context context (&dummyFile);
	
	string line;
	for (;;) {
		
#ifndef N_PROMPT
		std::cout << " >> " << std::flush;
#endif
		if (not std::getline (std::cin, line)) {
			break;
		}
		auto parseTree = parser::parse (line, &dummyFile);
		std::cerr << parseTree->toString () << std::endl;
		
		if (report::compilationFailed ()) {
			report::printLogs ();
			report::reset ();
			continue;
		}
		
		parseTree->declare (globalScope);
		
		if (report::compilationFailed ()) {
			report::printLogs ();
			report::reset ();
			continue;
		}
		
		auto ast = parseTree->createAST (context);
		std::cerr << ast->toString (text::PrintMode::PLAIN) << std::endl;
		delete parseTree;
		
		if (report::compilationFailed ()) {
			report::printLogs ();
			report::reset ();
			continue;
		}
		
		ast = AST::transform (ast);
		std::cerr << ast->toString (text::PrintMode::PLAIN) << std::endl;
		
		if (report::compilationFailed ()) {
			report::printLogs ();
			report::reset ();
			continue;
		}
		
		auto val = AST::ExecutePass () (ast);
		
		if (val) {
			std::cout << val->toString (text::PrintMode::PLAIN) << std::endl;
		} else {
			std::cout << '\n';
		}
		
	}
	return 0;
	
}
