#pragma once


#include <map>
#include <memory>
#include <nir/type.hpp>
#include <nir/type/tuple.hpp>


namespace nir {


class Array : public Type {
	CONST_VISITABLE
	public:
		
		static const size_t DATA_INDEX, LENGTH_INDEX;
		
		virtual const Parameter * getParam (const string & str) const {return type->getParam (str);}
		virtual size_t calculateSize (size_t ptrSize) const {return type->calculateSize (ptrSize);}
		
		const Tuple & getUnderlyingType () const {return *type;}
		const Type * getElementType () const {return elementType;}
		
		static Array * arrayOf (const Type * t);
		
	protected:
	private:
		
		Array (const Type * elementType);
		
		virtual string strDump (text::PrintMode mode) const {return type->toString (mode);}
		
		
		const Tuple * type;
		const Type * elementType;
		
		static std::map <const Type *, std::unique_ptr <Array>> arrayTypes;
		
};


}
