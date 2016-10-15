#pragma once


#include <map>
#include <memory>
#include <nir/type.hpp>
#include <nir/type/struct.hpp>


namespace nir {


class Array : public Type {
	CONST_VISITABLE
	public:
		
		virtual const Parameter * getParam (const string & str) const {return type.getParam (str);}
		virtual size_t calculateSize (size_t ptrSize) const {return type.calculateSize (ptrSize);}
		virtual ssize_t calculateOffset (size_t ptrSize, const string & iden) const {return type.calculateOffset (ptrSize, iden);}
		
		const Struct & getUnderlyingType () const {return type;}
		const Type * getElementType () const {return elementType;}
		
		static Array * arrayOf (const Type * t);
		
	protected:
	private:
		
		Array (const Type * elementType) : elementType (elementType) {}
		
		virtual string strDump (text::PrintMode mode) const {return type.toString (mode);}
		
		
		Struct type;
		const Type * elementType;
		
		static std::map <const Type *, std::unique_ptr <Array>> arrayTypes;
		
};


}
