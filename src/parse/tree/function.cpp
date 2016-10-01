#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <parse/tree/function.hpp>


auto buildParams = [](const BasicArray <PTNode> & args, nir::Scope * scope) {
	DynArray <const nir::Parameter *> conv;
	for (auto & arg : args) {
		auto * p = arg->buildParameter (scope);
		if (p) {
			assert (typeid (*p) == typeid (nir::Parameter));
			conv.push_back (static_cast <const nir::Parameter *> (p));
		} else {
			return DynArray <const nir::Parameter *> ();
		}
	}
	return conv;
};

const nir::Instruction * Function::build (nir::Scope * scope) {
	
	if (not fScope) {
		//Must be anon
		auto nirParams = buildParams (params, scope);
		auto nirTypes = buildParams (type, scope);
		fScope = scope->declareFunction ({nirParams.begin (), nirParams.end ()}, {nirTypes.begin (), nirTypes.end ()}, "", linkage);
	}
	
	if (body) {
		auto b = body->build (fScope);
		fScope->createAutoReturn (b);
	}
	return fScope->getFunctionPointer ();
	
}

void Function::declare (nir::Scope * scope) {
	
	auto nirParams = buildParams (params, scope);
	auto nirTypes = buildParams (type, scope);
	fScope = scope->declareFunction ({nirParams.begin (), nirParams.end ()}, {nirTypes.begin (), nirTypes.end ()}, iden, linkage);
	
}

string Function::strDump (text::PrintMode mode) {
	
	string s = iden.empty () ? "" : "def ";
	s += mode&text::COLOUR ? text::blu + s + text::reset : s;
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
			this->params = {params};
		}
	}
	if (type) {
		if (type->isList ()) {
			this->type = type->getArgs ();
		} else {
			this->type = {type};
		}
	}
	this->body = body;
}

Function::Function (uint32_t line, uint32_t pos, const string & iden, const BasicArray <PTNode> & params, const BasicArray <PTNode> & type, PTNode body, nir::LINKAGE linkage) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, line, pos), params (params), type (type), iden (iden), body (body), linkage (linkage) {}
