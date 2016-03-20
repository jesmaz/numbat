#include "../../../include/core.hpp"
#include "../../../include/nir/parameter.hpp"
#include "../../../include/nir/scope.hpp"
#include "../../../include/parse/tree/function.hpp"


auto buildParams = [](const std::vector <PTNode> & args, nir::Scope * scope) {
	std::vector <const nir::Parameter *> conv;
	conv.reserve (args.size ());
	for (auto & arg : args) {
		auto * p = arg->build (scope, ParseTreeNode::BuildMode::PARAMETER);
		assert (p);
		assert (typeid (*p) == typeid (nir::Parameter));
		conv.push_back (static_cast <const nir::Parameter *> (p));
	}
	return conv;
};

const nir::Instruction * Function::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	if (not fScope) {
		//Must be anon
		auto nirParams = buildParams (params, scope);
		auto nirTypes = buildParams (type, scope);
		fScope = scope->declareFunction (nirParams, nirTypes);
	}
	
	if (body) {
		auto b = body->build (fScope, mode);
	}
	return fScope->getFunctionPointer ();
	
}

void Function::declare (nir::Scope * scope) {
	
	auto nirParams = buildParams (params, scope);
	auto nirTypes = buildParams (type, scope);
	fScope = scope->declareFunction (nirParams, nirTypes, iden);
	
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

Function::Function (PTNode iden, PTNode params, PTNode type, PTNode body) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, (iden ? iden : params)->getLine (), (iden ? iden : params)->getPos ()) {	
	if (iden){
		this->iden = iden->getIden ();
	}
	if (params) {
		if (params->isList ()) {
			this->params = params->getArgs ();
		} else {
			this->params.push_back (params);
		}
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