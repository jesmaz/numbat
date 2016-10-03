#include <codegen/interpreter.hpp>
#include <iostream>
#include <nir/module.hpp>
#include <parse/handparser.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>



int numbatMain (const Config & cfg) {
	
	nir::Module nirmodule;
	auto globalScope = nirmodule.getGlobalScope ();
	codegen::Interpreter interpreter (nirmodule.getEntry ());
	numbat::File dummyFile;
	
	string line;
	for (;;) {
		
#ifndef N_PROMPT
		std::cout << " >> " << std::flush;
#endif
		if (not std::getline (std::cin, line)) {
			break;
		}
		auto parseTree = parse (line, &dummyFile);
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
		
		const nir::Instruction * val = parseTree->build (globalScope);
		
		if (report::compilationFailed ()) {
			report::printLogs ();
			report::reset ();
			continue;
		}
		
		std::cout << interpreter (val) << '\n';
		
	}
	return 0;
	
}
