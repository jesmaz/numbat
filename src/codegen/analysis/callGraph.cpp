#include <codegen/analysis/callGraph.hpp>
#include <iostream>
#include <nir/block.hpp>
#include <nir/instruction.hpp>
#include <nir/inst/call.hpp>
#include <nir/module.hpp>


namespace codegen {
namespace analysis {


void CallGraph::run (const nir::Module * module) {
	
	for (auto funcPair : module->getFunctions ()) {
		for (Block * block : funcPair.second->getBody ()) {
			for (const nir::Instruction * instr : block->getInstructions ()) {
				if (typeid (*instr) == typeid (nir::DirectCall)) {
					edges.insert (std::make_pair (funcPair.second, reinterpret_cast <const nir::DirectCall*> (instr)->getFunc ()));
				}
			}
		}
	}
	
}

CallGraph::CallGraph (PassProcession * PassProcession) : GenericPass (PassProcession) {}


}
}