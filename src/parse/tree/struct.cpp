#include <nir/module.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/type/struct.hpp>
#include <parse/tree/struct.hpp>
#include <utility/report.hpp>



const nir::Instruction * Struct::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	if (not type) {
		declare (scope);
	}
	
	if (not type) {
		report::logMessage (report::ERROR, scope->getSourceFile ()->getFileName (), getLine (), getPos (), iden + " declared twice");
	}
	
	std::vector <const nir::Parameter *> conv;
	conv.reserve (members.size ());
	for (auto & arg : members) {
		auto * p = arg->build (scope, ParseTreeNode::BuildMode::PARAMETER);
		assert (p);
		assert (typeid (*p) == typeid (nir::Parameter));
		conv.push_back (static_cast <const nir::Parameter *> (p));
	}
	type->populate (conv);
	return scope->createParameter (type);
	
}

void Struct::declare (nir::Scope * scope) {
	
	type = scope->registerStruct (iden);
	
}

string Struct::strDump (text::PrintMode mode) {
	string s = (mode&text::COLOUR ? text::blu + "struct " + text::reset : "struct ") + iden;
	s += " {" + text::listPtrToStr (members.begin (), members.end (), mode) + "}";
	return s;
}