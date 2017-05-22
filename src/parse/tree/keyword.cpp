#include <ast/type.hpp>
#include <parse/tree/keyword.hpp>


namespace parser {


AST::TypePtr ParseTreeKeyword::createASTtype (AST::Context &) {
	
	AST::TypePtr type = std::make_shared <AST::Inferred> (getPos ());
	if (sym == "ref") {
		type = std::make_shared <AST::Const> (getPos (), type);
		type = std::make_shared <AST::Ref> (getPos (), type);
		
	} else if (sym == "val") {
		type = std::make_shared <AST::Const> (getPos (), type);
		
	} else if (sym == "var") {
	} else if (sym == "vref") {
		type = std::make_shared <AST::Ref> (getPos (), type);
		
	}
	
	return type;
	
}

string ParseTreeKeyword::strDump (text::PrintMode mode) {
	return mode&text::COLOUR ? text::blu + sym + text::reset : sym;
}


}
