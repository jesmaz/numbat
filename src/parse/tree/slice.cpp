#include <parse/tree/slice.hpp>


namespace parser {


string ParseTreeSlice::strDump (text::PrintMode mode) {
	string s = text::PrintMode::COLOUR & mode ? text::red + "[" + text::reset : "[";
	if (start) {
		s += start->toString (mode) + " ";
	}
	s += ":";
	if (end) {
		s += " " + end->toString (mode);
	}
	if (step) {
		s += " : " + step->toString (mode);
	}
	return s + (text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]");
}

string ParseTreeSliceDecorator::strDump (text::PrintMode mode) {
	return "(" + decor->toString (mode) + " " + slice->toString (mode) + ")";
}

string ParseTreeSliceForEach::strDump (text::PrintMode mode) {
	string s = text::PrintMode::COLOUR & mode ? text::red + "[" + text::reset : "[";
	s += var;
	s += " in ";
	s += range->toString (mode);
	s += text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]";
	return s;
}


}
