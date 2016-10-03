#include <nir/type/number.hpp>

namespace nir {


std::map <std::pair <Type::ArithmaticType, uint32_t>, std::unique_ptr <const Number>> Number::numericalTypes;


const Number * Number::getNumberType (Type::ArithmaticType arith, uint32_t width) {
	
	auto key = std::make_pair (arith, width);
	
	auto & num = numericalTypes [key];
	
	if (!num) {
		num = std::unique_ptr <const Number> (new Number (arith, width));
	}
	
	return num.get ();
	
}



}

