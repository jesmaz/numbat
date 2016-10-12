#include <nir/module.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/type/struct.hpp>
#include <parse/tree/struct.hpp>
#include <utility/report.hpp>


namespace parser {


const nir::Instruction * Struct::build (nir::Scope * scope) {
	
	if (not type) {
		declare (scope);
	}
	
	if (not type) {
		report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), iden + " declared twice");
		return nullptr;
	}
	
	bool success = true;
	auto conv = members.map <const nir::Parameter *> ([&scope, &success](ParseTreeNode *& arg){
		return arg->buildParameter (scope);
	});
	if (success) type->populate (conv);
	return nullptr;
	
}

void Struct::declare (nir::Scope * scope) {
	
	type = scope->registerStruct (iden, getPos ());
	
}

string Struct::strDump (text::PrintMode mode) {
	string s = (mode&text::COLOUR ? text::blu + "struct " + text::reset : "struct ") + iden;
	s += " {" + text::listPtrToStr (members.begin (), members.end (), mode) + "}";
	return s;
}


}
