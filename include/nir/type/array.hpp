#pragma once


#include <map>
#include <nir/type.hpp>
#include <nir/type/struct.hpp>


namespace nir {


class Array : public Type {
	CONST_VISITABLE
	public:
		
		virtual ArithmaticType getArithmaticType () const {return Type::ArithmaticType::UINT;}
		virtual const Type * getDereferenceType () const {return type;}
		virtual size_t calculateSize (size_t ptrSize) const {return ptrSize*2;}
		
		static Struct * arrayOf (const Type * t);
		
	protected:
	private:
		
		Array (const Type * type) : type (type) {}
		
		virtual string strDump (text::PrintMode mode) const {return type->toString (mode) + "*";}
		
		
		const Type * type;
		
		static std::map <const Type *, Struct *> arrayTypes;
		
};


}
