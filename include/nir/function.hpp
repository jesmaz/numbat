#pragma once


#include <nir/forward.hpp>
#include <string>
#include <utility/array.hpp>


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
		const BasicArray <Block *> getBody () const {return blocks;}
		const BasicArray <const Parameter *> & getArgs () const {return args;}
		const BasicArray <const Parameter *> & getRet () const {return ret;}
		const BasicArray <const Type *> getRetTypes () const {return retTypes;}
		void pushBlock (Block * block);
		//WARNING: Deleting the old body is the responsibility of the user of this method
		void dirtyReplaceBody (const BasicArray <Block *> & b) {blocks = b;}
		
		Function (const std::string & label, LINKAGE linkage);
		Function (const BasicArray <const Parameter *> & args, const BasicArray <const Parameter *> & ret, const std::string & label, LINKAGE linkage);
		
		~Function ();
		
	protected:
	private:
		
		Function (const Function & func)=delete;
		const Function & operator = (const Function & func)=delete;
		
		Block * entry=nullptr;
		DynArray <Block *> blocks;
		
		
		std::string label;
		BasicArray <const Parameter *> args, ret;
		BasicArray <const Type *> retTypes;
		const Type * type;
		CallConv cc = CallConv::NCC;
		LINKAGE linkage;
		
};


}
