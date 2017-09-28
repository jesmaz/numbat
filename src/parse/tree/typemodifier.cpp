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

string ParseTreeTypeModifier::strDump (text::PrintMode mode) {
	return sym + " " + type->toString (mode);
}


}
