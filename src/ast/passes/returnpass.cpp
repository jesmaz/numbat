#include <ast/call.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
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



void InsertReturnPass::visit (const IfElse & node) {
	defAction (node);
}

void InsertReturnPass::visit (const Sequence & node) {
	if (func->getRetVals ().empty ()) {
		DynArray <NodePtr> arr (node.getNodes ());
		if (arr.empty ()) {
			arr.push_back (std::make_shared <AST::Return> (node.getPos (), node.getFile ()));
		} else {
			arr.push_back (std::make_shared <AST::Return> (arr.back ()->getPos (), arr.back ()->getFile ()));
		}
		nPtr = std::make_shared <AST::Sequence> (node.getPos (), node.getFile (), node.getType (), node.getLocalStack (), arr);
	} else {
		nPtr = std::make_shared <AST::Return> (node.getPos (), node.getFile (), nPtr);
	}
}

void InsertReturnPass::visit (const SystemCall & node) {
	defAction (node);
}

void InsertReturnPass::defAction (const Node & node) {
	if (func->getRetVals ().empty ()) {
		auto ret = std::make_shared <AST::Return> (node.getPos (), node.getFile ());
		nPtr = std::make_shared <AST::Sequence> (node.getPos (), node.getFile (), node.getType (), BasicArray <VarPtr> {}, BasicArray <NodePtr> {nPtr, ret});
	} else {
		nPtr = std::make_shared <AST::Return> (node.getPos (), node.getFile (), nPtr);
	}
}



}
