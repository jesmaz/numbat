#pragma once

#include <ostream>
#include <nir/forward.hpp>
#include <utility/text.hpp>
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
				
				PrintIterator (const Block * block, text::PrintMode pm) : block (block), pm (pm) {}
				
				const Instruction & operator = (const Instruction &)=delete;
				
				friend Block;
				
				const Block * block=nullptr;
				size_t pos=0;
				text::PrintMode pm;
				bool printedName=false;
				
		};
		
		bool validate () const;
		
		std::vector <const Instruction *> & getInstructions () {return instructions;}
		const std::vector <const Instruction *> & getInstructions () const {return instructions;}
		
		const Instruction * give (const Instruction * instr);
		
		PrintIterator printerBeg (text::PrintMode pm=text::PLAIN) const {return PrintIterator (this, pm);}
		
		symbol getName () const {return name;}
		
		void setFallthrough (Block * fallthrough) {this->fallthrough = fallthrough;}
		
		Block (symbol name) : name (name) {}
		
	protected:
	private:
		
		std::vector <const Instruction *> instructions;
		Block * fallthrough=nullptr;
		symbol name;
		
};


};
