#include <ast/passes/stackmachine.hpp>
#include <parse/handparser.hpp>

#include "stackmachinetest.hpp"

stackmachine::Chunk createChunk (const string & program) {
	
	numbat::File dummyFile;
	auto parseTree = parser::parse (program, &dummyFile);
	AST::Context context (&dummyFile);
	
	auto transformed = AST::transform (parseTree->createAST (context));
	
	auto chunk = stackMachinePass (transformed);
	for (auto f : AST::Context::getAllFunctions ()) {
		chunk += stackMachinePass (f);
	}
	return chunk;
	
}
