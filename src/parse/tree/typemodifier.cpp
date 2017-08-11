#include <nir/scope.hpp>
#include <parse/tree/typemodifier.hpp>
#include <utility/report.hpp>

namespace parser {


AST::TypePtr ParseTreeTypeModifier::createASTtype (AST::Context & ctx) {
	auto t = type->createASTtype (ctx);
	if (sym == "ref") {
		t = AST::Const::get (t);
		t = AST::Ref::get (t);
		
	} else if (sym == "val") {
		t = AST::Const::get (t);
		
	} else if (sym == "var") {
	} else if (sym == "vref") {
		t = AST::Ref::get (t);
		
	}
	
	return t;
}

const nir::Instruction * ParseTreeTypeModifier::buildAllocate (nir::Scope * scope, const string & iden) {
	auto * t = resolveType (scope);
	if (not t) return nullptr;
	return scope->allocateVariable (t, iden);
}

const nir::Type * ParseTreeTypeModifier::resolveType (nir::Scope * scope) {
	const nir::Type * t = type->resolveType (scope);
	if (sym == "ref") {
		return t->getPointerTo ();
	} else if (sym == "val") {
		//TODO: Make this type constant
		return t;
	} else if (sym == "var") {
		return t;
	} else if (sym == "vref") {
		//TODO: Make this type constant
		return t->getPointerTo ();
	} else {
		report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Unidentified symbol: " + sym);
		return nullptr;
	}
}

string ParseTreeTypeModifier::strDump (text::PrintMode mode) {
	return sym + " " + type->toString (mode);
}


}
