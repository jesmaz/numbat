#pragma once


#include <file.hpp>
#include <nir/forward.hpp>
#include <nir/type.hpp>

#include <string>


namespace nir {

using std::string;

class ImportHandle : public Type {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		size_t calculateSize (size_t ptrSize) const {return 0;}
		
		ImportHandle (const nir::Scope * scope) : scope (scope) {}
		
	protected:
	private:
		
		const nir::Scope * scope;
		
		virtual string strDump (text::PrintMode mode) const {return "import";}
		
};


};