#pragma once


#include <map>
#include <nir/type.hpp>
#include <nir/type/struct.hpp>


namespace nir {


class Array : public Type {
	CONST_VISITABLE
	public:
		
		virtual size_t calculateSize (size_t ptrSize) const {return type.calculateSize (ptrSize);}
		
		static Array * arrayOf (const Type * t);
		
	protected:
	private:
		
		Array () {}
		
		virtual string strDump (text::PrintMode mode) const {return type.toString (mode);}
		
		
		Struct type;
		
		static std::map <const Type *, Array *> arrayTypes;
		
};


}
