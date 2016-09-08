#pragma once


#include "forward.hpp"


#include <vector>


namespace nir {


struct Block {
	
	public:
		
		bool validate () const;
		
		std::vector <const Instruction *> & getInstructions () {return instructions;}
		const std::vector <const Instruction *> & getInstructions () const {return instructions;}
		
		const Instruction * give (const Instruction * instr);
		
	protected:
	private:
		
		std::vector <const Instruction *> instructions;
		Block * fallthrough=nullptr;
		
};


};
