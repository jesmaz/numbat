#include "../../../include/parse/tree/function.hpp"

string Function::strDump (text::PrintMode mode) {
	
	string s = iden.empty () ? "" : "def ";
	s = mode&text::COLOUR ? text::blu + s + text::reset : s;
	s += iden.empty () ? "" : iden + " ";
	if (not tplate.empty ())
		s += "@[" + text::listPtrToStr (tplate.begin (), tplate.end (), mode) + "]";
	s += "(" + text::listPtrToStr (params.begin (), params.end (), mode) + ")";
	s += "->(" + text::listPtrToStr (type.begin (), type.end (), mode) + ")";
	s += body->toString ();
	return s;
	
}

Function::Function (const std::vector <PTNode> & args) : Function (args.empty () ? 0 : args.front ()->getLine (), args.empty () ? 0 : args.front ()->getPos ()) {
	
	
	
}