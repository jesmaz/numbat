#pragma once


#include "../nir/forward.hpp"

#include <cinttypes>
#include <map>
#include <string>
#include <vector>


namespace codegen {

using std::string;
using namespace nir;

class Target {
	
	public:
		
		virtual void finalise () {}
		virtual void reset ()=0;
		virtual void visit (const nir::Function * module)=0;
		virtual void visit (const Type * module)=0;
		
		void * jitFunction (const nir::Function *);
		
		static Target * find (const string & iden);
		
		virtual ~Target () {}
		
	protected:
		
		typedef Target* (*FFunc)();
		struct RegTarget {
			RegTarget (const string & iden, FFunc ffunc);
		};
		
	private:
		
		virtual void generate (const nir::Function * func, std::vector <uint8_t> & output) {}
		
		static std::map <string, FFunc> * targets;
		
};


};