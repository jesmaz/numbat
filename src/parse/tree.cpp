#include "../../include/parse/tree.hpp"

numbat::parser::ASTnode ParseTree::build (numbat::parser::NumbatScope * scope) {
	numbat::parser::NumbatScope * child = numbat::parser::createChild (scope);
	auto cvt = [&](const std::vector <PTNode> & args) {
		std::vector <numbat::parser::ASTnode> nodes (args.size ());
		for (size_t i=0; i<args.size (); ++i) nodes[i] = args[i]->build (scope);
		return nodes;
	};
	for (auto * s : structs) {
		numbat::parser::createStruct (child, s->getIden (), {});
	}
	for (auto * f : functions) {
		numbat::parser::NumbatScope * fscope = numbat::parser::createChild (child);
		numbat::parser::createFunctionDecleration (child, f->getIden (), cvt (f->getParams ()), cvt (f->getType ()), {}, fscope);
	}
	for (auto * b : body) {
		numbat::parser::addToBody (child, b->build (child));
	}
	return * new numbat::parser::ASTnode (child);
}