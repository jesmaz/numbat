#include "../../../include/parse/tree/function.hpp"

auto convArgs = [](const std::vector <PTNode> & args, numbat::parser::NumbatScope * scope){
	std::vector <numbat::parser::ASTnode> conv;
	conv.reserve (args.size ());
	for (auto & arg : args) {
		assert (arg);
		conv.push_back (arg->build (scope));
		assert (conv.back ());
	}
	return conv;
};

numbat::parser::ASTnode Function::build (numbat::parser::NumbatScope * scope) {
	
	if (not fScope) {
		//Must be anon
		fScope = createChild (scope);
		auto func = numbat::parser::createFunctionDecleration (fScope, iden, convArgs (params, fScope), convArgs (type, fScope), {}, fScope);
		numbat::parser::setFunction (fScope, func);
	}
	
	if (body) {
		return body->build (fScope);
	} else {
		return nullptr;
	}
	
}

void Function::declare (numbat::parser::NumbatScope * scope) {
	
	fScope = createChild (scope);
	auto func = numbat::parser::createFunctionDecleration (scope, iden, convArgs (params, fScope), convArgs (type, fScope), {}, fScope);
	numbat::parser::setFunction (fScope, func);
	
}

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