#ifndef NIR_TYPE_INTEGER
#define NIR_TYPE_INTEGER


#include "../type.hpp"

namespace nir {

class Number : public Type {
	CONST_VISITABLE
	public:
		
		virtual ArithmaticType getArithmaticType () const {return arith;}
		virtual size_t calculateSize (size_t ptrSize) const {return (width+7)/8;}
		
		Number (Type::ArithmaticType arith, uint32_t width) : width (width), arith (arith) {}
		
	protected:
	private:
		
		uint32_t width;//in bits
		Type::ArithmaticType arith;
		
		virtual string strDump (text::PrintMode mode) const {return char (arith) + std::to_string (width);}
		
};

};


#endif/*NIR_TYPE_BASE_HPP*/