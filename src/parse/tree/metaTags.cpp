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

string MetaTags::strDump (text::PrintMode mode) {
	string s = "(";
	for (auto & tag : tags) {
		s += "@" + tag->toString (mode);
		s += " ";
	}
	return s + componant->toString (mode) + ")";
}


}
