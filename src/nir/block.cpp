#include <nir/block.hpp>
#include <nir/instruction.hpp> 
#ifdef D_PRINT
#include <iostream>
#endif


namespace nir {


bool Block::validate () const {
	
	bool valid = true;
	for (const Instruction * instr : instructions) {
		valid &= instr->validate ();
	}
	return valid;
	
}

const Instruction * Block::give (const Instruction * instr) {
	instructions.push_back (instr);
#ifdef D_PRINT
	std::cout << instr->toString () << std::endl;
#endif
	return instr;
}

}
