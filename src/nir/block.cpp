#include <nir/block.hpp>
#include <nir/instruction.hpp> 


namespace nir {


bool Block::validate () const {
	
	bool valid = true;
	for (const Instruction * instr : instructions) {
		valid &= instr->validate ();
	}
	return valid;
	
}


}