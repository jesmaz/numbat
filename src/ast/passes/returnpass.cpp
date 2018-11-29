#include <ast/flowcontrol.hpp>
#include <ast/passes/returnpass.hpp>
#include <ast/sequence.hpp>


namespace AST {


void FunctionReturnsPass::visit (const And & node) {
	FunctionReturnsPass lhs;
	FunctionReturnsPass rhs;
	lhs.analyse (node.getFirst ());
	rhs.analyse (node.getSecond ());
	returns = lhs.returns and rhs.returns;
}

void FunctionReturnsPass::visit (const IfElse & node) {
	if (node.getBody ()) {
		FunctionReturnsPass body;
		FunctionReturnsPass alt;
		body.analyse (node.getBody ());
		alt.analyse (node.getAlt ());
		returns = body.returns and alt.returns;
	}
}

void FunctionReturnsPass::visit (const Or & node) {
	FunctionReturnsPass lhs;
	FunctionReturnsPass rhs;
	lhs.analyse (node.getFirst ());
	rhs.analyse (node.getSecond ());
	returns = lhs.returns and rhs.returns;
}

void FunctionReturnsPass::visit (const Return & node) {
	returns = true;
}

void FunctionReturnsPass::visit (const Sequence & node) {
	for (auto & n : node.getNodes ()) {
		FunctionReturnsPass body;
		body.analyse (n);
		if (body.returns) {
			returns = true;
			return;
		}
	}
}



}
