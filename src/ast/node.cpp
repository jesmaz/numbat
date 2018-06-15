#include <ast/context.hpp>
#include <ast/node.hpp>
#include <ast/passes/callgraph.hpp>
#include <ast/passes/execute.hpp>
#include <ast/passes/functionpurity.hpp>
#include <ast/passes/metatagswizzle.hpp>
#include <ast/passes/prunedead.hpp>
#include <ast/passes/reflectpass.hpp>
#include <ast/passes/resolve.hpp>
#include <utility/config.hpp>

namespace AST {


NodePtr transform (const NodePtr & node) {
	
	auto & funcs = Context::getAllFunctions ();
	
	auto n = MetaTagSwizzlePass () (node);
	assert (n);
	for (auto & f : funcs) {
		if (f->getBody ()) {
			f->replaceBody (MetaTagSwizzlePass () (f->getBody ()));
		}
	}
	
	n = ResolvePass () (n);
	assert (n);
	for (auto & f : funcs) {
		if (f->getBody ()) {
			f->replaceBody (ResolvePass () (f->getBody ()));
		}
	}
	
	n = ReflectPass () (n);
	assert (n);
	for (auto & f : funcs) {
		if (f->getBody ()) {
			f->replaceBody (ReflectPass () (f->getBody ()));
		}
	}
	
	for (auto & f : funcs) {
		if (f->getBody ()) {
			CallGraph (f).analyse (f->getBody ());
		}
	}
	
	FunctionPurityPass::analyse (funcs);
	
	if (Config::globalConfig ().const_folding) {
		
		n = FoldConstPass () (n);
		assert (n);
		for (auto & f : funcs) {
			if (f->getBody ()) {
				f->replaceBody (FoldConstPass () (f->getBody ()));
			}
		}
		
	}
	
	if (Config::globalConfig ().prune_dead_code) {
		n = PruneDeadCodePass () (n);
		assert (n);
		for (auto & f : funcs) {
			if (f->getBody ()) {
				f->replaceBody (PruneDeadCodePass () (f->getBody ()));
			}
		}
	}
	
	return n;
	
}


};
