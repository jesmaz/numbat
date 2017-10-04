#include <ast/operation.hpp>
#include <ast/type.hpp>
#include <parse/tree/index.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/type/array.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeIndex::createAST (AST::Context & ctx) {
	
	BasicArray <AST::NodePtr> oppArgs (args.size () + 1);
	oppArgs [0] = index->createAST (ctx);
	for (size_t i=1; i<oppArgs.size (); ++i)  {
		oppArgs [i] = args [i-1]->createAST (ctx);
	}
	return std::make_shared <AST::Unresolved_Operation> (getPos (), ctx.getSourceFile (), " [ ]", oppArgs, OPERATION::INDEX);
	
}

AST::TypePtr ParseTreeIndex::createASTtype (AST::Context & ctx) {
	if (args.empty ()) {
		return AST::Array::get (index->createASTtype (ctx));
	} else {
		return std::make_shared <AST::ArrayInit> (getPos (), ctx.getSourceFile (), index->createASTtype (ctx), args [0]->createAST (ctx), nullptr);
	}
}

string ParseTreeIndex::strDump (text::PrintMode mode) {
	string s = "(" + index->toString (mode) + (text::PrintMode::COLOUR & mode ? text::red + " [" + text::reset : " [");
	if (!args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1; i<args.size (); ++i) s += args [i]->toString (mode);
	}
	return s + (text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]") + ")";
}


}
