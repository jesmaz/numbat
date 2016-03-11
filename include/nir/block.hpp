#ifndef NIR_BLOCK
#define NIR_BLOCK


#include "forward.hpp"


#include <vector>


namespace nir {


struct Block {
	
	public:
		
		const std::vector <const Instruction *> getInstructions () const {return instructions;}
		
		const Instruction * give (Instruction * instr) {instructions.push_back (instr); return instr;}
		
	protected:
	private:
		
		std::vector <const Instruction *> instructions;
		Block * fallthrough=nullptr;
		
};


};


#endif/*NIR_BLOCK*/