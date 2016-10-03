#pragma once

#include <map>
#include <memory>
#include <nir/type.hpp>

namespace nir {

class Number : public Type {
	CONST_VISITABLE
	public:
		
		virtual ArithmaticType getArithmaticType () const {return arith;}
		virtual size_t calculateSize (size_t ptrSize) const {return (width+7)/8;}
		
		static const Number * getNumberType (Type::ArithmaticType arith, uint32_t width);
		
	protected:
	private:
		
		Number (Type::ArithmaticType arith, uint32_t width) : width (width), arith (arith) {}
		
		uint32_t width;//in bits
		Type::ArithmaticType arith;
		
		virtual string strDump (text::PrintMode mode) const {return char (arith) + std::to_string (width);}
		
		static std::map <std::pair <Type::ArithmaticType, uint32_t>, std::unique_ptr <const Number>> numericalTypes;
		
};

};
