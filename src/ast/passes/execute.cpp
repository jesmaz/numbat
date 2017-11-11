#include <ast/call.hpp>
#include <ast/function.hpp>
#include <ast/literal.hpp>
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

void FoldConstPass::visit (const Basic_Operation & node) {
	bool allValues = true;
	readVar = false;
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
			nPtr = group (native ({}, {node.getPos (), node.getFile ()}));
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
			nPtr = group (native ({arg}, {node.getPos (), node.getFile ()}));
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
			nPtr = group (native ({lhs, rhs}, {node.getPos (), node.getFile ()}));
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
				result = arg->getLiteral ().to_aint0 ();
				break;
			}
			case Numeric::ArithmaticType::DECINT:
				abort ();
				break;
			case Numeric::ArithmaticType::FPINT:
				if (targetType->getMinPrec () <= 32) {
					result = float (arg->getLiteral ().to_double ());
				} else {
					result = arg->getLiteral ().to_double ();
				}
				break;
			case Numeric::ArithmaticType::INT:
				if (targetType->getMinPrec () <= 8) {
					result = int8_t (arg->getLiteral ().to_int64 ());
				} else if (targetType->getMinPrec () <= 16) {
					result = int16_t (arg->getLiteral ().to_int64 ());
				} else if (targetType->getMinPrec () <= 32) {
					result = int32_t (arg->getLiteral ().to_int64 ());
				} else {
					result = arg->getLiteral ().to_int64 ();
				}
				break;
			case Numeric::ArithmaticType::UINT:
				if (targetType->getMinPrec () <= 8) {
					result = uint8_t (arg->getLiteral ().to_uint64 ());
				} else if (targetType->getMinPrec () <= 16) {
					result = uint16_t (arg->getLiteral ().to_uint64 ());
				} else if (targetType->getMinPrec () <= 32) {
					result = uint32_t (arg->getLiteral ().to_uint64 ());
				} else {
					result = arg->getLiteral ().to_uint64 ();
				}
				break;
		}
		nPtr = std::make_shared <Value> (nPtr->getPos (), nPtr->getFile (), targetType, result);
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
	
	auto parent = visit (node.getParent ());
	if (parent->isValue ()) {
		abort ();
		
	} else {
		nPtr = std::make_shared <StaticIndex> (node.getPos (), node.getFile (), node.getType (), parent, node.getIndex ());
		
	}
	
}

void FoldConstPass::visit (const Variable & node) {
	if (readVar) {
		nPtr = std::make_shared <Value> (node.getPos (), node.getFile (), node.getType (), node.getCurrentValue ());
	}
}


}
