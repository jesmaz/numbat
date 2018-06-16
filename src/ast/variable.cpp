#include <ast/variable.hpp>


namespace AST {


LiteralStack Variable::globalContex;

string StaticIndex::toString (text::PrintMode mode) const {
	return parent->toString (mode) + "." + std::to_string (index);
}


ValPtr StaticValue::parseNumber (numbat::lexer::position pos, const numbat::File * file, const string & num) {
	if (num.back () == 'd') {
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 64), std::stod (num));
	} else if (num.back () == 'f') {
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 32), std::stof (num));
	} else if (num.back () == 'h') {
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 16), std::stof (num));
	} else if (num.back () == 'q') {
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::FPINT, 128), std::stod (num));
	} else if (num.back () == 'u') {
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::UINT, 64), std::stod (num));
	} else if (num.back () == 'a') {
		string::size_type p;
		auto v = num.substr (0, num.length () - 1);
		string l;
		if ((p = v.find ('.')) != string::npos) {
			l = v.substr (0, p) + v.substr (p + 1) + "/1" + string (v.length () - (p+1), '0');
		} else {
			l = v;
		}
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::ARBITRARY, 0), mpq_class (l, 10));
	} else {
		string::size_type p;
		string l;
		if ((p = num.find ('.')) != string::npos) {
			l = num.substr (0, p) + num.substr (p + 1) + "/1" + string (num.length () - (p+1), '0');
		} else {
			l = num;
		}
		return std::make_shared <StaticValue> (pos, file, Numeric::get (Numeric::ArithmaticType::UNDETERMINED, 0), mpq_class (l, 10));
	}
}


string Variable::toString (text::PrintMode mode) const {
	return "var (" + getType ()->toString (mode) + ") " + identifier;
}

Variable::Variable (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, uint32_t stackIndex, LOCATION location, const string & iden) : Value (pos, file, type), stackIndex (stackIndex), location (location), identifier (iden) {}


}
