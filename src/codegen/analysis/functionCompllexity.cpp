#include <codegen/analysis/functionComplexity.hpp>
#include <nir/module.hpp>


namespace codegen {
namespace analysis {


FunctionComplexity::FunctionComplexity (PassProcession * procession, nir::Module * module) : callGraph (procession->runPass <CallGraph> (module)) {
	
	for (auto funcPair : module->getFunctions ()) {
		nir::Function * f = funcPair.second;
		Behaviour & b = funcData [f];
		if (f->getLinkage () == nir::LINKAGE::EXTERNAL) {
			b.deterministic = false;
			b.hasSideEffects = true;
			b.pure = false;
		}
	}
	
	bool dirty;
	
	do {
		dirty=false;
		for (const auto & pair : callGraph->getEdges ()) {
			const Function * f_p = pair.first;
			const Function * f_c = pair.second;
			Behaviour & b_p = funcData [f_p];
			Behaviour & b_c = funcData [f_c];
			if (b_p.deterministic and not b_c.deterministic) {
				dirty = true;
				b_p.deterministic = b_c.deterministic;
			}
			if (not b_p.hasSideEffects and b_c.hasSideEffects) {
				dirty = true;
				b_p.hasSideEffects = b_c.hasSideEffects;
			}
			if (b_p.pure and not b_c.pure) {
				dirty = true;
				b_p.pure = b_c.pure;
			}
		}
	} while (dirty);
	
}


}
}