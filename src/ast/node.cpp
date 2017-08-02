#include <ast/node.hpp>
#include <ast/passes/metatagswizzle.hpp>
#include <ast/passes/resolve.hpp>

namespace AST {


NodePtr transform (const NodePtr & node) {
	
	auto n = MetaTagSwizzlePass () (node);
	n = ResolvePass () (n);
	return n;
	
}


};
