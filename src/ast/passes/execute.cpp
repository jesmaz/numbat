#include <ast/call.hpp>
#include <ast/function.hpp>
#include <ast/operation.hpp>
#include <ast/passes/execute.hpp>
#include <ast/passes/operate.hpp>
#include <ast/sequence.hpp>
#include <ast/typecast.hpp>


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

Literal group (const BasicArray <Literal> & arr) {
	if (arr.empty ()) {
		return Literal ();
	} else if (arr.size () == 1) {
		return arr [0];
	} else {
		return Literal (arr);
	}
}

void FoldConstPass::visit (const Basic_Operation & node) {
	bool allValues = true;
	auto args = node.getArgs ().map <NodePtr> ([&](const NodePtr & n) -> NodePtr {
		auto arg = visit (n, false);
		allValues &= arg->isValue ();
		return arg;
	});
	if (allValues) {
		switch (node.getOpp ()) {
			case parser::OPERATION::AND:
				abort ();
				break;
			case parser::OPERATION::ADD:
				nPtr = OperatePass <parser::OPERATION::ADD> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::AS:
				nPtr = OperatePass <parser::OPERATION::AS> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::ASSIGN:
				nPtr = OperatePass <parser::OPERATION::ASSIGN> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::BAND:
				nPtr = OperatePass <parser::OPERATION::BAND> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::BNOT:
				nPtr = OperatePass <parser::OPERATION::BNOT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::BOR:
				nPtr = OperatePass <parser::OPERATION::BOR> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::BXOR:
				nPtr = OperatePass <parser::OPERATION::BXOR> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CMPEQ:
				nPtr = OperatePass <parser::OPERATION::CMPEQ> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CMPGT:
				nPtr = OperatePass <parser::OPERATION::CMPGT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CMPGTE:
				nPtr = OperatePass <parser::OPERATION::CMPGTE> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CMPLT:
				nPtr = OperatePass <parser::OPERATION::CMPLT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CMPLTE:
				nPtr = OperatePass <parser::OPERATION::CMPLTE> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CMPNE:
				nPtr = OperatePass <parser::OPERATION::CMPNE> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::CONCAT:
				nPtr = OperatePass <parser::OPERATION::CONCAT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::DECREMENT:
				nPtr = OperatePass <parser::OPERATION::DECREMENT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::DIV:
				nPtr = OperatePass <parser::OPERATION::DIV> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::IN:
				nPtr = OperatePass <parser::OPERATION::IN> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::INCREMENT:
				nPtr = OperatePass <parser::OPERATION::INCREMENT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::INDEX:
				nPtr = OperatePass <parser::OPERATION::INDEX> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::LNOT:
				nPtr = OperatePass <parser::OPERATION::LNOT> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::MUL:
				nPtr = OperatePass <parser::OPERATION::MUL> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::NEG:
				nPtr = OperatePass <parser::OPERATION::NEG> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::NONE:
			case parser::OPERATION::OR:
				abort ();
				break;
			case parser::OPERATION::REM:
				nPtr = OperatePass <parser::OPERATION::REM> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::SUB:
				nPtr = OperatePass <parser::OPERATION::SUB> (*executionStack, args) (node.getType ());
				break;
			case parser::OPERATION::__COUNT__:
				abort ();
				break;
		}
	} else {
		nPtr = std::make_shared <Basic_Operation> (
			node.getPos (),
			node.getFile (),
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
			nPtr = std::make_shared <StaticValue> (node.getPos (), node.getFile (), node.getType (), group (native ({}, {node.getPos (), node.getFile ()})));
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
			auto lit = group (native ({static_cast <Value *> (arg.get ())->getLiteral (*executionStack)}, {node.getPos (), node.getFile ()}));
			nPtr = std::make_shared <StaticValue> (node.getPos (), node.getFile (), node.getType (), lit);
		} else {
			auto stack = std::make_shared <LiteralStack> ();
			(*stack) [0] = static_cast <Value *> (arg.get ())->getLiteral (*executionStack);
			auto result = this->visit (node.getFunc ()->getBody (), false, stack);
			assert (result->isValue ());
			auto lit = static_cast <Value *> (result.get ())->getLiteral (*stack);
			nPtr = std::make_shared <StaticValue> (node.getPos (), node.getFile (), node.getType (), lit);
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
			auto lit = group (native ({
				static_cast <Value *> (lhs.get ())->getLiteral (*executionStack), 
				static_cast <Value *> (rhs.get ())->getLiteral (*executionStack)
			}, {node.getPos (), node.getFile ()}));
			nPtr = std::make_shared <StaticValue> (node.getPos (), node.getFile (), node.getType (), lit);
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

void FoldConstPass::visit (const CastToArbPrec & node) {
	auto nodeType = std::static_pointer_cast <Numeric> (node.getNode ()->getType ());
	assert (nodeType);
	abort ();
}

void FoldConstPass::visit (const CastToDecimal & node) {
	auto nodeType = std::static_pointer_cast <Numeric> (node.getNode ()->getType ());
	assert (nodeType);
	abort ();
}

void FoldConstPass::visit (const CastToFloat & node) {
	auto nodeType = std::static_pointer_cast <Numeric> (node.getNode ()->getType ());
	assert (nodeType);
	abort ();
}

void FoldConstPass::visit (const CastToInt & node) {
	auto arg = std::static_pointer_cast <Value> (node.getNode ());
	auto targetType = std::static_pointer_cast <Numeric> (node.getType ());
	if (arg and targetType) {
		Literal result;
		switch (targetType->getArith ()) {
			case Numeric::ArithmaticType::UNDETERMINED:
			case Numeric::ArithmaticType::ARBITRARY: {
				result = arg->getLiteral (*executionStack).to_aint0 ();
				break;
			}
			case Numeric::ArithmaticType::DECINT:
				abort ();
				break;
			case Numeric::ArithmaticType::FPINT:
				if (targetType->getMinPrec () <= 32) {
					result = float (arg->getLiteral (*executionStack).to_double ());
				} else {
					result = arg->getLiteral (*executionStack).to_double ();
				}
				break;
			case Numeric::ArithmaticType::INT:
				if (targetType->getMinPrec () <= 8) {
					result = int8_t (arg->getLiteral (*executionStack).to_int64 ());
				} else if (targetType->getMinPrec () <= 16) {
					result = int16_t (arg->getLiteral (*executionStack).to_int64 ());
				} else if (targetType->getMinPrec () <= 32) {
					result = int32_t (arg->getLiteral (*executionStack).to_int64 ());
				} else {
					result = arg->getLiteral (*executionStack).to_int64 ();
				}
				break;
			case Numeric::ArithmaticType::UINT:
				if (targetType->getMinPrec () <= 8) {
					result = uint8_t (arg->getLiteral (*executionStack).to_uint64 ());
				} else if (targetType->getMinPrec () <= 16) {
					result = uint16_t (arg->getLiteral (*executionStack).to_uint64 ());
				} else if (targetType->getMinPrec () <= 32) {
					result = uint32_t (arg->getLiteral (*executionStack).to_uint64 ());
				} else {
					result = arg->getLiteral (*executionStack).to_uint64 ();
				}
				break;
		}
		nPtr = std::make_shared <StaticValue> (nPtr->getPos (), nPtr->getFile (), targetType, result);
	}
}

void FoldConstPass::visit (const CastToUint & node) {
	auto nodeType = std::static_pointer_cast <Numeric> (node.getNode ()->getType ());
	assert (nodeType);
	abort ();
}

void FoldConstPass::visit (const Sequence & node) {
	bool allValues=true;
	auto args = node.getNodes ().map <NodePtr> ([&](const NodePtr & n) -> NodePtr {
		auto arg = visit (n);
		allValues &= arg->isValue ();
		return arg;
	});
	if (args.size () == 1) {
		nPtr = args [0];
	} else if (args.size () > 1 and allValues) {
		nPtr = args.back ();
	} else {
		nPtr = std::make_shared <Sequence> (node.getPos (), node.getFile (), args);
	}
	//TODO: chain onto a pruning pass (might as well do it here)
}

void FoldConstPass::visit (const StaticIndex & node) {
	
	auto parent = visit (node.getParent (), true);
	nPtr = std::make_shared <StaticIndex> (node.getPos (), node.getFile (), node.getType (), parent, node.getIndex ());
	
}

void FoldConstPass::visit (const Variable & node) {}


}
