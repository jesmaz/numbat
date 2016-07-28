#include <codegen/analysis/functionComplexity.hpp>
#include <codegen/interpreter.hpp>
#include <codegen/optimisation/staticEvaluation.hpp>
#include <iostream>
#include <nir/block.hpp>
#include <nir/instruction.hpp>
#include <nir/inst/constant.hpp>
#include <nir/module.hpp>


namespace codegen {
namespace optimisation {


class CTimeInterpreter : public Interpreter {
	
	public:
		
		const nir::Instruction * tryfold (const nir::Instruction * instr);
		void replaceBody (const std::vector <nir::Block *> & body);
		
		void visit (const nir::DirectCall & call);
		
		CTimeInterpreter (const analysis::FunctionComplexity * funcCplx) : funcCplx (funcCplx) {}
		
	private:
		
		const analysis::FunctionComplexity * funcCplx;
		std::map <nir::symbol, nir::Argument> replaceMap;
		
};


const nir::Instruction * CTimeInterpreter::tryfold (const nir::Instruction * instr) {
	
	auto args = instr->getArguments ();
	bool unknowable = false, replace = false;
	
	for (nir::Argument & arg : args) {
		auto itt = replaceMap.find (arg.sym);
		if (itt != replaceMap.end ()) {
			arg = itt->second;
			replace = true;
		} else {
			unknowable = true;
		}
	}
	
	if (unknowable) {
		if (replace) {
			const nir::Instruction * i = instr->recreate (args);
			delete instr;
			return i;
		} else {
			return instr;
		}
	}
	
	if (not replace) {
		for (nir::symbol s : instr->getIdens ()) {
			replaceMap [s] = {instr, s};
		}
		return instr;
	}
	
	auto atoms = lookupAtoms (instr);
	nir::Constant * constant = new nir::Constant (instr->getTypes (), instr->getIdens ());
	
	for (Interpreter::Atom atom : atoms) {
		switch (atom.atomicType) {
			case F32:
				constant->push_back (atom.data.f32);
				break;
			case F64:
				constant->push_back (atom.data.f64);
				break;
			case S8:
				constant->push_back (int64_t (atom.data.s8));
				break;
			case S16:
				constant->push_back (int64_t (atom.data.s16));
				break;
			case S32:
				constant->push_back (int64_t (atom.data.s32));
				break;
			case S64:
				constant->push_back (int64_t (atom.data.s64));
				break;
			case U8:
				constant->push_back (uint64_t (atom.data.u8));
				break;
			case U16:
				constant->push_back (uint64_t (atom.data.u16));
				break;
			case U32:
				constant->push_back (uint64_t (atom.data.u32));
				break;
			case U64:
				constant->push_back (uint64_t (atom.data.u64));
				break;
			default:
				abort ();
		}
	}
	
	delete instr;
	return constant;
	
}


void CTimeInterpreter::replaceBody (const std::vector <nir::Block *> & body) {
	
	for (nir::Block * block : body) {
		for (const nir::Instruction *& instr : block->getInstructions ()) {
			std::cout << "Input: " << instr->toString () << std::endl;
			instr = tryfold (instr);
			std::cout << "Output: " << instr->toString () << std::endl;
		}
	}
	
}


void CTimeInterpreter::visit (const nir::DirectCall & call) {
	
	const auto & bdb = funcCplx->getFuncData ();
	auto itt = bdb.find (call.getFunc ());
	if (itt == bdb.end ()) return;
	
	const analysis::FunctionComplexity::Behaviour & b = itt->second;
	
	if (b.deterministic and not b.hasSideEffects and b.pure) {
		codegen::Interpreter::visit (call);
	}
	
}



StaticEvaluator::StaticEvaluator (PassProcession * passProcession, nir::Module * module) {
	
	const analysis::FunctionComplexity * funcCplx = passProcession->runPass <analysis::FunctionComplexity> (module);
	
	for (auto funcPair : module->getFunctions ()) {
		
		if (funcPair.second->getLinkage () == nir::LINKAGE::EXTERNAL) continue;
		
		CTimeInterpreter inter (funcCplx);
		
		inter.replaceBody (funcPair.second->getBody ());
		
	}
	
}



}
}