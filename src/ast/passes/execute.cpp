#include <ast/call.hpp>
#include <ast/function.hpp>
#include <ast/literal.hpp>
#include <ast/operation.hpp>
#include <ast/passes/execute.hpp>
#include <ast/passes/operate.hpp>
#include <ast/sequence.hpp>


namespace AST {


bool foldable (const FuncPtr & func) {
	auto functional = func->getMetaData ().find ("functional");
	return (functional != func->getMetaData ().end () and functional->second == "true");
}

NodePtr group (const BasicArray <NodePtr> & arr) {
	if (arr.empty ()) {
		abort ();
		//TODO: return a nil value
	} else if (arr.size () == 1) {
		return arr [0];
	} else {
		abort ();
		//TODO: Return a tuple
	}
}

void FoldConstPass::visit (const Basic_Operation & node) {
	bool allValues = true;
	auto args = node.getArgs ().map <NodePtr> ([&](const NodePtr & n) -> NodePtr {
		auto arg = visit (n);
		allValues &= arg->isValue ();
		return arg;
	});
	if (allValues) {
		switch (node.getOpp ()) {
			case parser::OPERATION::AND:
				abort ();
				break;
			case parser::OPERATION::ADD:
				nPtr = OperatePass <parser::OPERATION::ADD> (args) (node.getType ());
				break;
			case parser::OPERATION::AS:
				nPtr = OperatePass <parser::OPERATION::AS> (args) (node.getType ());
				break;
			case parser::OPERATION::ASSIGN:
				nPtr = OperatePass <parser::OPERATION::ASSIGN> (args) (node.getType ());
				break;
			case parser::OPERATION::BAND:
				nPtr = OperatePass <parser::OPERATION::BAND> (args) (node.getType ());
				break;
			case parser::OPERATION::BNOT:
				nPtr = OperatePass <parser::OPERATION::BNOT> (args) (node.getType ());
				break;
			case parser::OPERATION::BOR:
				nPtr = OperatePass <parser::OPERATION::BOR> (args) (node.getType ());
				break;
			case parser::OPERATION::BXOR:
				nPtr = OperatePass <parser::OPERATION::BXOR> (args) (node.getType ());
				break;
			case parser::OPERATION::CMPEQ:
				nPtr = OperatePass <parser::OPERATION::CMPEQ> (args) (node.getType ());
				break;
			case parser::OPERATION::CMPGT:
				nPtr = OperatePass <parser::OPERATION::CMPGT> (args) (node.getType ());
				break;
			case parser::OPERATION::CMPGTE:
				nPtr = OperatePass <parser::OPERATION::CMPGTE> (args) (node.getType ());
				break;
			case parser::OPERATION::CMPLT:
				nPtr = OperatePass <parser::OPERATION::CMPLT> (args) (node.getType ());
				break;
			case parser::OPERATION::CMPLTE:
				nPtr = OperatePass <parser::OPERATION::CMPLTE> (args) (node.getType ());
				break;
			case parser::OPERATION::CMPNE:
				nPtr = OperatePass <parser::OPERATION::CMPNE> (args) (node.getType ());
				break;
			case parser::OPERATION::CONCAT:
				nPtr = OperatePass <parser::OPERATION::CONCAT> (args) (node.getType ());
				break;
			case parser::OPERATION::DECREMENT:
				nPtr = OperatePass <parser::OPERATION::DECREMENT> (args) (node.getType ());
				break;
			case parser::OPERATION::DIV:
				nPtr = OperatePass <parser::OPERATION::DIV> (args) (node.getType ());
				break;
			case parser::OPERATION::IN:
				nPtr = OperatePass <parser::OPERATION::IN> (args) (node.getType ());
				break;
			case parser::OPERATION::INCREMENT:
				nPtr = OperatePass <parser::OPERATION::INCREMENT> (args) (node.getType ());
				break;
			case parser::OPERATION::INDEX:
				nPtr = OperatePass <parser::OPERATION::INDEX> (args) (node.getType ());
				break;
			case parser::OPERATION::LNOT:
				nPtr = OperatePass <parser::OPERATION::LNOT> (args) (node.getType ());
				break;
			case parser::OPERATION::MUL:
				nPtr = OperatePass <parser::OPERATION::MUL> (args) (node.getType ());
				break;
			case parser::OPERATION::NEG:
				nPtr = OperatePass <parser::OPERATION::NEG> (args) (node.getType ());
				break;
			case parser::OPERATION::NONE:
			case parser::OPERATION::OR:
				abort ();
				break;
			case parser::OPERATION::REM:
				nPtr = OperatePass <parser::OPERATION::REM> (args) (node.getType ());
				break;
			case parser::OPERATION::SUB:
				nPtr = OperatePass <parser::OPERATION::SUB> (args) (node.getType ());
				break;
			case parser::OPERATION::__COUNT__:
				abort ();
				break;
		}
	} else {
		nPtr = std::make_shared <Basic_Operation> (
			node.getPos (),
			node.getType (),
			node.getIden (),
			args,
			node.getOpp ()
		);
	}
}

void FoldConstPass::visit (const Call_0 & node) {
	if (foldable (node.getFunc ())) {
		auto native = node.getFunc ()->getNative ();
		if (native) {
			nPtr = group (native ({}));
		} else {
			abort ();
		}
	}
}

void FoldConstPass::visit (const Call_1 & node) {
	if (foldable (node.getFunc ())) {
		auto arg = this->visit (node.getArg ());
		if (!arg->isValue ()) return;
		auto native = node.getFunc ()->getNative ();
		if (native) {
			nPtr = group (native ({arg}));
		} else {
			abort ();
		}
	}
}

void FoldConstPass::visit (const Call_2 & node) {
	if (foldable (node.getFunc ())) {
		auto lhs = this->visit (node.getLhs ());
		auto rhs = this->visit (node.getRhs ());
		if (!lhs->isValue () or !rhs->isValue ()) return;
		auto native = node.getFunc ()->getNative ();
		if (native) {
			nPtr = group (native ({lhs, rhs}));
		} else {
			abort ();
		}
	}
}

void FoldConstPass::visit (const Call_n & node) {
	if (foldable (node.getFunc ())) {
		auto native = node.getFunc ()->getNative ();
		if (native) {
			abort ();
		} else {
			abort ();
		}
	}
}

void FoldConstPass::visit (const Sequence & node) {
	nPtr = std::make_shared <Sequence> (
		node.getPos (), 
		node.getNodes ().map <NodePtr> ([&](const NodePtr & n) -> NodePtr {return visit (n);})
	);
	//TODO: chain onto a pruning pass (might as well do it here)
}


}
