#include <ast/passes/prunedead.hpp>
#include <ast/sequence.hpp>


namespace AST {


void PruneDeadCodePass::visit (const Sequence & seq) {
	DynArray <NodePtr> nodes;
	nodes.reserve (seq.getNodes ().size ());
	for (auto & n : seq.getNodes ()) {
		auto a = this->visit (n);
		if (a) {
			nodes.push_back (a);
		}
	}
	nPtr = std::make_shared <Sequence> (seq.getPos (), seq.getFile (), nodes);
}

NodePtr PruneDeadCodePass::operator () (const NodePtr & node) {
	assert (nPtr = node);
	node->accept (*this);
	return nPtr;
}


}
