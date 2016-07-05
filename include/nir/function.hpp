#ifndef NIR_FUNCTION
#define NIR_FUNCTION


#include <nir/forward.hpp>
#include <string>
#include <vector>


namespace nir {


struct Function {
	
	public:
		
		enum CallConv : char {CCC, NCC, SV_ABI};
		
		bool validate () const;
		
		Block * getEntryPoint () {return entry;}
		const Block * getEntryPoint () const {return entry;}
		const Type * getType () const {return type;}
		const std::string getLabel () const {return label;}
		const std::vector <const Parameter *> & getArgs () const {return args;}
		const std::vector <const Parameter *> & getRet () const {return ret;}
		const std::vector <const Type *> getRetTypes () const {return retTypes;}
		void pushBlock (Block * block) {blocks.push_back (block);}
		
		Function (const std::string & label);
		Function (std::vector <const Parameter *> args, std::vector <const Parameter *> ret, const std::string & label);
		
	protected:
	private:
		
		Block * entry=nullptr;
		std::vector <Block *> blocks;
		
		
		std::string label;
		std::vector <const Parameter *> args, ret;
		std::vector <const Type *> retTypes;
		const Type * type;
		CallConv cc = CallConv::NCC;
		
};


};


#endif/*NIR_FUNCTION*/