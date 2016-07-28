#ifndef NIR_BLOCK
#define NIR_BLOCK


#include "forward.hpp"


#include <vector>


namespace nir {


struct Block {
	
	public:
		
		bool validate () const;
		
		std::vector <const Instruction *> & getInstructions () {return instructions;}
		const std::vector <const Instruction *> & getInstructions () const {return instructions;}
		
		const Instruction * give (const Instruction * instr) {instructions.push_back (instr); return instr;}
		
	protected:
	private:
		
		std::vector <const Instruction *> instructions;
		Block * fallthrough=nullptr;
		
};


};


#endif/*NIR_BLOCK*/