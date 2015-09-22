#include "../../../include/parse/tree/function.hpp"

string Function::strDump (text::PrintMode mode) {
	
	string s = iden.empty () ? "" : "def ";
	s = mode&text::COLOUR ? text::blu + s + text::reset : s;
	s += iden.empty () ? "" : iden + " ";
	if (not tplate.empty ())
		s += "@[" + text::listPtrToStr (tplate.begin (), tplate.end (), mode) + "]";
	s += "(" + text::listPtrToStr (params.begin (), params.end (), mode) + ")";
	s += "->(" + text::listPtrToStr (type.begin (), type.end (), mode) + ")";
	s += body ? body->toString (mode) : "";
	return s;
	
}

Function::Function (PTNode iden, PTNode params, PTNode type, PTNode body) : ParseTreeNode (params->getLine (), params->getPos ()) {
	if (iden){
		this->iden = iden->getIden ();
	}
	if (params->isList ()) {
		this->params = params->getArgs ();
	} else {
		this->params.push_back (params);
	}
	if (type) {
		if (type->isList ()) {
			this->type = type->getArgs ();
		} else {
			this->type.push_back (type);
		}
	}
	this->body = body;
}