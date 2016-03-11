#include "../../include/parse/tree.hpp"

numbat::parser::ASTnode ParseTree::build (numbat::parser::NumbatScope * scope) {
	if (!treeScope) {
		treeScope = scope;
		declareInline ();
	}
	for (auto * b : body) {
		if (auto n = b->build (treeScope)) numbat::parser::addToBody (treeScope, n);
	}
	//TODO: Fix memory leak
	return * new numbat::parser::ASTnode (treeScope);
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

void ParseTree::declare (numbat::parser::NumbatScope * scope) {
	treeScope = numbat::parser::createChild (scope);
	declareInline ();
}

void ParseTree::declareInline () {
	auto cvt = [&](const std::vector <PTNode> & args) {
		std::vector <numbat::parser::ASTnode> nodes (args.size ());
		for (size_t i=0; i<args.size (); ++i) nodes[i] = args[i]->build (treeScope);
		return nodes;
	};
	for (PTNode n : import) {
		n->declare (treeScope);
	}
	for (Struct * s : structs) {
		s->declare (treeScope);
	}
	for (Function * f : functions) {
		f->declare (treeScope);
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