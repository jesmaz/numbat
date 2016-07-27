#pragma once


#include <codegen/analysis/callGraph.hpp>
#include <codegen/genericPass.hpp>
#include <memory>
#include <set>


namespace codegen {
namespace analysis {


class FunctionComplexity : public GenericPass {
		
	public:
		
		struct Behaviour {
			enum class COMPLEXITY {UNSOLVED, CONSTANT, LOGARITHMIC, LINEAR, N_LOG_N, POLYNOMIAL, EXPENENTIAL};
			
			// The McCarrbe metric is M = E - N + X
			// E -> the number of edges
			// N -> the number of nodes
			// X -> the number of exits
			// Projects can set warnings if this gets too high
			size_t mcCarbe=0;
			
			// Measures how many instructions are actually in the function, usefull for determining inlining
			// in future this should be recorded for each part of the complexity as well as overall
			size_t weight=0;
			
			// Count of how many times this function is called and how many calls it makes respectivly
			size_t called=0, calls=0;
			
			// In future it would be wise to record the factors causing complexity (parameters, global state)
			COMPLEXITY time = COMPLEXITY::UNSOLVED, space = COMPLEXITY::UNSOLVED;
			
			// The outcome of deterministic functions can be reliably
			// predicted and reproduced independent of the runtimne environment
			// External functions are assumed to be non-deterministic
			bool deterministic=true;
			
			// Functions with side effects mutate global state
			// External functions are assumed to have side effects
			bool hasSideEffects=false;
			
			// Any function that changes the state of a parameter
			// For example, methods
			// External functions that accept non-const references are assumed to mutate them
			bool mutatesParameters=false;
			
			// Pure functions do not access or change global state or call non-pure functions
			// External functions are assumed to be impure
			bool pure=true;
		};
		
		const std::map <const nir::Function *, Behaviour> & getFuncData () const {return funcData;}
		
		virtual ~FunctionComplexity () {}
		
	protected:
	private:
		
		friend PassProcession;
		
		FunctionComplexity (PassProcession * procession, nir::Module * module);
		
		const CallGraph * callGraph;
		std::map <const nir::Function *, Behaviour> funcData;
		
};


}
}