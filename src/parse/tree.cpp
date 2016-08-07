#include "../../include/parse/tree.hpp"

const nir::Instruction * ParseTree::build (nir::Scope * scope, BuildMode mode) {
	if (nirTreeScope) {
		scope = nirTreeScope;
	} else {
		declare (scope);
	}
	const nir::Instruction * last;
	for (auto * b : body) {
		last = b->build (scope, mode);
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