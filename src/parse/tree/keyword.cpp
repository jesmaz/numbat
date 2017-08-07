#include <ast/type.hpp>
#include <parse/tree/keyword.hpp>


namespace parser {


AST::TypePtr ParseTreeKeyword::createASTtype (AST::Context &) {
	
	AST::TypePtr type = std::make_shared <AST::Inferred> (getPos ());
	if (sym == "ref") {
		type = AST::Const::get (type);
		type = AST::Ref::get (type);
		
	} else if (sym == "val") {
		type = AST::Const::get (type);
		
	} else if (sym == "var") {
	} else if (sym == "vref") {
		type = AST::Ref::get (type);
		
	}
	
	return type;
	
}

string ParseTreeKeyword::strDump (text::PrintMode mode) {
	return mode&text::COLOUR ? text::blu + sym + text::reset : sym;
}


}
