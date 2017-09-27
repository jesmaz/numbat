#include <ast/context.hpp>
#include <ast/meta.hpp>
#include <parse/tree/metaTags.hpp>


namespace parser {


AST::NodePtr MetaTags::createAST (AST::Context & ctx) {
	auto node = componant->createAST (ctx);
	for (auto itt=tags.rbegin (), end=tags.rend (); itt!=end; ++itt) {
		node = std::make_shared <AST::Reflect> (getPos (), ctx.getSourceFile (), (*itt)->createASTmeta (ctx), node);
	}
	return node;
}

AST::NodePtr MetaTags::createASTparam (AST::Context & ctx) {
	auto node = componant->createASTparam (ctx);
	for (auto itt=tags.rbegin (), end=tags.rend (); itt!=end; ++itt) {
		node = std::make_shared <AST::Reflect> (getPos (), ctx.getSourceFile (), (*itt)->createASTmeta (ctx), node);
	}
	return node; 
}

const nir::Instruction * MetaTags::build (nir::Scope * scope) {
	
	const auto * cmpt = componant->build (scope);
	for (auto itt = tags.rbegin (), end = tags.rend (); itt!=end; ++itt) {
		//TODO: Implement this method. Will require some attention to defining meta tags in the first place
		// One option is: def @tag (function f) -> (function)
		// presumably this function would be staticly interpeted here
		// This is fine, other than two things. What properties should functions be allowed to have?
		// And what happens when (not if) a user tries to modify the global state of the program in a meta tag?
	}
	return cmpt;
	
}

void MetaTags::declare (nir::Scope * scope) {
	componant->declare (scope);
}

string MetaTags::strDump (text::PrintMode mode) {
	string s = "(";
	for (auto & tag : tags) {
		s += "@" + tag->toString (mode);
		s += " ";
	}
	return s + componant->toString (mode) + ")";
}


}
