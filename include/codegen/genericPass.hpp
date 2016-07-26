#pragma once


#include <cassert>
#include <nir/forward.hpp>
#include <map>
#include <set>
#include <typeinfo>


namespace codegen {


class PassProcession;


class GenericPass {
		
	public:
		
		virtual void run (const nir::Module * module)=0;
		
		GenericPass (PassProcession * procession) {}
		
		virtual ~GenericPass () {}
		
	protected:
	private:
		
};


}