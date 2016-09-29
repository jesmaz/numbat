#include <nir/type/number.hpp>

namespace nir {


std::map <std::pair <Type::ArithmaticType, uint32_t>, const Number *> Number::numericalTypes;


const Number * Number::getNumberType (Type::ArithmaticType arith, uint32_t width) {
	
	auto key = std::make_pair (arith, width);
	
	const Number *& num = numericalTypes [key];
	
	if (!num) {
		num = new Number (arith, width);
	}
	
	return num;
	
}



}

