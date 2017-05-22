#include <ast/node.hpp>
#include <ast/passes/resolve.hpp>

namespace AST {


template <typename Pass>
NodePtr runPass (const NodePtr & node) {
	Pass pass;
	return pass (node);
}

NodePtr transform (const NodePtr & node) {
	
	return runPass <ResolvePass> (node);
	
}


};
