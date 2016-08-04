#include <codegen/interpreter.hpp>
#include <iostream>
#include <nir/module.hpp>
#include <parse/handparser.hpp>



int main (int argc, char ** args) {
	
	codegen::Interpreter
	interpreter;
	nir::Module nirmodule (&interpreter);
	auto globalScope = nirmodule.getGlobalScope ();
	
	string line;
	for (;;) {
#ifndef N_PROMPT
		std::cout << " >> " << std::flush;
#endif
		if (not std::getline (std::cin, line)) {
			break;
		}
		auto parseTree = parse (line);
		std::cerr << parseTree->toString () << std::endl;
		parseTree->declare (globalScope);
		const nir::Instruction * val = parseTree->build (globalScope, ParseTreeNode::BuildMode::NORMAL);
		std::cout << interpreter (val) << '\n';
	}
	return 0;
	
}