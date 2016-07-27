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
		const LINKAGE getLinkage () const {return linkage;}
		const Type * getType () const {return type;}
		const std::string getLabel () const {return label;}
		const std::vector <Block *> getBody () const {return blocks;}
		const std::vector <const Parameter *> & getArgs () const {return args;}
		const std::vector <const Parameter *> & getRet () const {return ret;}
		const std::vector <const Type *> getRetTypes () const {return retTypes;}
		void pushBlock (Block * block) {blocks.push_back (block);}
		//WARNING: Deleting the old body is the responsibility of the user of this method
		void dirtyReplaceBody (const std::vector <Block *> & b) {blocks = b;}
		
		Function (const std::string & label, LINKAGE linkage);
		Function (std::vector <const Parameter *> args, std::vector <const Parameter *> ret, const std::string & label, LINKAGE linkage);
		
	protected:
	private:
		
		Block * entry=nullptr;
		std::vector <Block *> blocks;
		
		
		std::string label;
		std::vector <const Parameter *> args, ret;
		std::vector <const Type *> retTypes;
		const Type * type;
		CallConv cc = CallConv::NCC;
		LINKAGE linkage;
		
};


};


#endif/*NIR_FUNCTION*/