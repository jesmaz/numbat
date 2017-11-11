#include <ast/literal.hpp>
#include <ast/passes/shallnot.hpp>
#include <ast/type.hpp>
#include <gmpxx.h>


namespace AST {


string Value::toString (text::PrintMode mode) const {
	return literal.toString (mode);
}

ValPtr Value::parseNumber (numbat::lexer::position pos, const numbat::File * file, const string & num) {
	if (num.back () == 'd') {
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 64), std::stod (num));
	} else if (num.back () == 'f') {
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 32), std::stof (num));
	} else if (num.back () == 'h') {
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 16), std::stof (num));
	} else if (num.back () == 'q') {
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 128), std::stod (num));
	} else if (num.back () == 'u') {
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::UINT, 64), std::stod (num));
	} else if (num.back () == 'a') {
		string::size_type p;
		auto v = num.substr (0, num.length () - 1);
		string l;
		if ((p = v.find ('.')) != string::npos) {
			l = v.substr (0, p) + v.substr (p + 1) + "/1" + string (v.length () - (p+1), '0');
		} else {
			l = v;
		}
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::ARBITRARY, 0), mpq_class (l));
	} else {
		string::size_type p;
		string l;
		if ((p = num.find ('.')) != string::npos) {
			l = num.substr (0, p) + num.substr (p + 1) + "/1" + string (num.length () - (p+1), '0');
		} else {
			l = num;
		}
		return std::make_shared <Value> (pos, file, Numeric::get (Numeric::ArithmaticType::UNDETERMINED, 0), mpq_class (l));
	}
}


}
