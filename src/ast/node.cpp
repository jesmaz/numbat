#include <ast/node.hpp>
#include <ast/passes/execute.hpp>
#include <ast/passes/metatagswizzle.hpp>
#include <ast/passes/reflectpass.hpp>
#include <ast/passes/resolve.hpp>

namespace AST {


NodePtr transform (const NodePtr & node) {
	
	auto n = MetaTagSwizzlePass () (node);
	n = ResolvePass () (n);
	n = ReflectPass () (n);
	n = FoldConstPass () (n);
	return n;
	
}


};
