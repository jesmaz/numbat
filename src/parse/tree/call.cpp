#include <ast/call.hpp>
#include <nir/inst/functionPointer.hpp>
#include <nir/scope.hpp>
#include <parse/tree/call.hpp>
#include <utility/array.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeCall::createAST (AST::Context & ctx) {
	
	auto callee = iden->createAST (ctx);
	auto params = args.map <AST::NodePtr> ([&](auto & a){return a->createAST (ctx);});
	return std::make_shared <AST::Unresolved_Call> (getPos (), ctx.getSourceFile (), callee, params);
	
}

string ParseTreeCall::strDump (text::PrintMode mode) {
	string s = iden->toString (mode) + " (";
	if (not args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1, l=args.size (); i<l; ++i) {
			s += ", " + args [i]->toString (mode);
		}
	}
	return s + ")";
}


}
