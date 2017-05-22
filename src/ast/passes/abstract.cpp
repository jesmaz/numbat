#include <ast/node.hpp>
#include <ast/passes/abstract.hpp>
#include <cassert>


namespace AST {


NodePtr AbstractPass::operator () (const NodePtr & node) {
	assert (nPtr = node);
	node->accept (*this);
	assert (nPtr);
	return nPtr;
}


}
