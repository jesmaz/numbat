#include <ast/sequence.hpp>
#include <parse/tree/function.hpp>
#include <parse/tree/struct.hpp>
#include <parse/tree.hpp>


namespace parser {


AST::Context * ParseTree::createContext (const AST::Context & ctx) {
	
	if (context) return context.get ();
	
	context = std::make_unique <AST::Context> (ctx);
	
	for (auto & str : structs) {
		context->type (str->getIden (), str->createType (*context));
	}
	
	for (auto & func : functions) {
		context->func (func->getIden (), func->createFunc (*context));
	}
	
	return context.get ();
	
}

AST::NodePtr ParseTree::createAST (AST::Context & ctx) {
	
	auto * context = createContext (ctx);
	auto arr = body.map <AST::NodePtr> ([&](auto & a){return a->createAST (*context);});
	return std::make_shared <AST::Sequence> (getPos (), ctx.getSourceFile (), arr);
	
}

AST::NodePtr ParseTree::extendAST (AST::Context & ctx) {
	
	for (auto & str : structs) {
		ctx.type (str->getIden (), str->createType (ctx));
	}
	
	for (auto & func : functions) {
		ctx.func (func->getIden (), func->createFunc (ctx));
	}
	
	auto arr = body.map <AST::NodePtr> ([&](auto & a){return a->extendAST (ctx);});
	return std::make_shared <AST::Sequence> (getPos (), ctx.getSourceFile (), arr);
	
}

const nir::Instruction * ParseTree::build (nir::Scope * scope) {
	if (nirTreeScope) {
		scope = nirTreeScope;
	} else {
		declare (scope);
	}
	const nir::Instruction * last=nullptr;
	for (auto * b : body) {
		last = b->build (scope);
	}
	
	return last;
}

string ParseTree::strDump (text::PrintMode mode) {
	if (not body.empty ()) {
		string s = "{"; 
		for (size_t i=0; i+1<body.size (); ++i) {
			s += body [i]->toString () + "; ";
		}
		return s + body.back ()->toString () + ";}";
	} else {
		return "{}";
	}
}

void ParseTree::declare (nir::Scope * scope) {
	nirTreeScope = scope;
	declareInline ();
}

void ParseTree::declareInline () {
	for (Function * f : functions) {
		f->declare (nirTreeScope);
	}
}

void ParseTree::push_back (PTNode node) {
	switch (node->getType ()) {
			case ParseTreeNode::NodeType::FUNCTION:
					functions.push_back (node->asFunction ());
					break;
			case ParseTreeNode::NodeType::IMPORT:
					import.push_back (node);
					break;
			case ParseTreeNode::NodeType::STRUCT:
					structs.push_back (node->asStruct ());
					break;
			default:
					break;
	}
	body.push_back (node);
}


}
