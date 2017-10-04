#include <ast/call.hpp>
#include <parse/tree/resolvescope.hpp>


namespace parser {


AST::NodePtr ResolveScope::createAST (AST::Context & ctx) {
	
	AST::NodePtr par = parent->createAST (ctx);
	return std::make_shared <AST::Unresolved_Get_Member> (getPos (), ctx.getSourceFile (), par, iden);
	
}

string ResolveScope::strDump (text::PrintMode mode) {
	
	return parent->toString (mode) + "." + iden;
	
}


}
