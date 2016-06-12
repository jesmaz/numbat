#ifndef NIR_FUNCTION
#define NIR_FUNCTION


#include "forward.hpp"

#include <vector>


namespace nir {


struct Function {
	
	public:
		
		enum CallConv : char {CCC, NCC, SV_ABI};
		
		Block * getEntryPoint () {return entry;}
		const Block * getEntryPoint () const {return entry;}
		const Type * getType () const {return type;}
		const std::vector <const Parameter *> & getArgs () const {return args;}
		const std::vector <const Parameter *> & getRet () const {return ret;}
		void pushBlock (Block * block) {blocks.push_back (block);}
		
		Function ();
		Function (std::vector <const Parameter *> args, std::vector <const Parameter *> ret);
		
	protected:
	private:
		
		Block * entry=nullptr;
		std::vector <Block *> blocks;
		
		
		std::vector <const Parameter *> args, ret;
		const Type * type;
		CallConv cc = CallConv::NCC;
		
};


};


#endif/*NIR_FUNCTION*/