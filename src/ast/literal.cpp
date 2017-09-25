#include <ast/literal.hpp>
#include <ast/type.hpp>


namespace AST {


string Number::toString (text::PrintMode mode) const {
	return value;
}

Number::Number (numbat::lexer::position pos, const string & value) : Value (pos, [&](){
	if (value.back () == 'd') {
		return Numeric::get (Numeric::ArithmaticType::FPINT, 64);
	} else if (value.back () == 'f') {
		return Numeric::get (Numeric::ArithmaticType::FPINT, 32);
	} else if (value.back () == 'h') {
		return Numeric::get (Numeric::ArithmaticType::FPINT, 16);
	} else if (value.back () == 'q') {
		return Numeric::get (Numeric::ArithmaticType::FPINT, 128);
	} else if (value.back () == 'u') {
		return Numeric::get (Numeric::ArithmaticType::UINT, 64);
	} else {
		return Numeric::get (Numeric::ArithmaticType::ARBITRARY, 0);
	}
} ()), value (value) {}


}
