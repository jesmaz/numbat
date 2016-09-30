#pragma once

#include <ostream>
#include <nir/forward.hpp>
#include <vector>


namespace nir {


struct Block {
	
	public:
		
		struct PrintIterator {
			
			public:
				
				const PrintIterator & operator ++ ();
				friend std::ostream & operator << (std::ostream & out, const PrintIterator & itt);
				operator bool () const;
				
			protected:
			private:
				
				PrintIterator (const Block * block) : block (block) {}
				
				const Instruction & operator = (const Instruction &)=delete;
				
				friend Block;
				
				const Block * block=nullptr;
				size_t pos=0;
				
		};
		
		bool validate () const;
		
		std::vector <const Instruction *> & getInstructions () {return instructions;}
		const std::vector <const Instruction *> & getInstructions () const {return instructions;}
		
		const Instruction * give (const Instruction * instr);
		
		PrintIterator printerBeg () const {return PrintIterator (this);}
		
		void setFallthrough (Block * fallthrough) {this->fallthrough = fallthrough;}
		
	protected:
	private:
		
		std::vector <const Instruction *> instructions;
		Block * fallthrough=nullptr;
		
};


};
