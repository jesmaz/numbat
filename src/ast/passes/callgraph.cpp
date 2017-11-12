#include <ast/call.hpp>
#include <ast/passes/callgraph.hpp>


namespace AST {


void CallGraph::visit (const Call_0 & node) {
	AnalysisPass::visit (node);
	activeFunc->getDirectlyCalled ().push_back (node.getFunc ());
}

void CallGraph::visit (const Call_1 & node) {
	AnalysisPass::visit (node);
	activeFunc->getDirectlyCalled ().push_back (node.getFunc ());
}

void CallGraph::visit (const Call_2 & node) {
	AnalysisPass::visit (node);
	activeFunc->getDirectlyCalled ().push_back (node.getFunc ());
}

void CallGraph::visit (const Call_n & node) {
	AnalysisPass::visit (node);
	activeFunc->getDirectlyCalled ().push_back (node.getFunc ());
}


}
