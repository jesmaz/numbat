#pragma once

#include <ostream>
#include <nir/forward.hpp>
#include <utility/array.hpp>
#include <utility/text.hpp>
 


namespace nir {


struct Block {
	
	public:
		
		struct Iterator {
			
			public:
				
				const Iterator & operator ++ ();
				const Instruction & operator * ();
				operator bool () const;
				
			protected:
			private:
				
				Iterator (const Block * block) : block (block) {}
				
				friend Block;
				
				const Block * block=nullptr;
				size_t pos=0;
				
		};
		
		struct PrintIterator {
			
			public:
				
				const PrintIterator & operator ++ ();
				friend std::ostream & operator << (std::ostream & out, const PrintIterator & itt);
				operator bool () const;
				
			protected:
			private:
				
				PrintIterator (const Block * block, text::PrintMode pm) : block (block), pm (pm) {}
				
				const PrintIterator & operator = (const PrintIterator &)=delete;
				
				friend Block;
				
				const Block * block=nullptr;
				size_t pos=0;
				text::PrintMode pm;
				bool printedName=false;
				
		};
		
		const Block * getFallthrough () const {return fallthrough;}
		
		bool validate () const;
		
		BasicArray <const Instruction *> & getInstructions () {return instructions;}
		const BasicArray <const Instruction *> & getInstructions () const {return instructions;}
		
		const Instruction * give (const Instruction * instr);
		
		Iterator beg () const {return Iterator (this);}
		
		PrintIterator printerBeg (text::PrintMode pm=text::PLAIN) const {return PrintIterator (this, pm);}
		
		symbol getName () const {return name;}
		
		void setFallthrough (Block * fallthrough) {this->fallthrough = fallthrough;}
		
		Block (symbol name) : name (name) {}
		~Block ();
		
	protected:
	private:
		
		DynArray <const Instruction *> instructions;
		Block * fallthrough=nullptr;
		symbol name;
		
};


};
