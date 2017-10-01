#include <ast/literal.hpp>
#include <ast/type.hpp>
#include <gmpxx.h>


namespace AST {


string ArrayVal::toString (text::PrintMode mode) const {
	string s = "[";
	for (auto & v : value->getData ()) {
		s += v->toString (mode) + ", ";
	}
	if (s.back () == ' ') {
		s.pop_back ();
		s.back () = ']';
		return s;
	}
	return s + "]";
}

string Number::toString (text::PrintMode mode) const {
	return value->toString (mode);
}

Number::Number (numbat::lexer::position pos, const numbat::File * file, const string & value) : Value (pos, file, [&](){
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
	} else if (value.back () == 'a') {
		return Numeric::get (Numeric::ArithmaticType::ARBITRARY, 0);
	} else {
		return Numeric::get (Numeric::ArithmaticType::UNDETERMINED, 0);
	}
} ()), value ([&]()->std::shared_ptr <NumericLiteral>{
	if (value.back () == 'd') {
		return std::make_shared <NumericLiteralTemplate <double>> (std::stod (value));
	} else if (value.back () == 'f') {
		return std::make_shared <NumericLiteralTemplate <float>> (std::stof (value));
	} else if (value.back () == 'h') {
		return std::make_shared <NumericLiteralTemplate <float>> (std::stof (value));
	} else if (value.back () == 'q') {
		return std::make_shared <NumericLiteralTemplate <double>> (std::stold (value));
	} else if (value.back () == 'u') {
		return std::make_shared <NumericLiteralTemplate <uint64_t>> (std::stoull (value));
	} else if (value.back () == 'a') {
		string::size_type pos;
		auto v = value.substr (0, value.length () - 1);
		string l;
		if ((pos = v.find ('.')) != string::npos) {
			l = v.substr (0, pos) + v.substr (pos + 1) + "/1" + string (v.length () - (pos+1), '0');
		} else {
			l = v;
		}
		return std::make_shared <NumericLiteralTemplate <mpq_class>> (mpq_class (v));
	} else {
		string::size_type pos;
		string l;
		if ((pos = value.find ('.')) != string::npos) {
			l = value.substr (0, pos) + value.substr (pos + 1) + "/1" + string (value.length () - (pos+1), '0');
		} else {
			l = value;
		}
		return std::make_shared <NumericLiteralTemplate <mpq_class>> (mpq_class (l));
	}
} ()) {}


}
