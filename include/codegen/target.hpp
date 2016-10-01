#pragma once


#include <cinttypes>
#include <map>
#include <nir/forward.hpp>
#include <string>
#include <utility/array.hpp>


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
		
		virtual void generate (const nir::Function * func, BasicArray <uint8_t> & output) {}
		
		static std::map <string, FFunc> * targets;
		
};


};
