#pragma once


#include <ast/passes/shallnot.hpp>
#include <ast/type.hpp>
#include <ast/variable.hpp>
#include <gmpxx.h>


namespace AST {


template <parser::OPERATION OPP>
class OperatePass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr &) {abort ();}
		
		virtual void visit (const Array & node);
		virtual void visit (const Const & node);
		virtual void visit (const Ref & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Struct & node);
		
		OperatePass (LiteralStack & stack, const BasicArray <NodePtr> & args) : stack (stack), args (args) {}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
		LiteralStack & stack;
		const BasicArray <NodePtr> & args;
		
};


template <parser::OPERATION OPP>
NodePtr OperatePass <OPP>::operator () (const NodePtr & node) {
	node->accept (*this);
	assert (nPtr);
	return nPtr;
}

template <>
NodePtr OperatePass <parser::OPERATION::ASSIGN>::operator () (const NodePtr & node) {
	auto n = args [0].get ();
	Literal & lhs = static_cast <Value *> (args [0].get ())->getLiteral (stack);
	Literal & rhs = static_cast <Value *> (args [1].get ())->getLiteral (stack);
	lhs = rhs;
	return args [0];
}

template <typename FUNCTOR>
NodePtr bitwiseBinary (const Numeric & type, LiteralStack & stack, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral (stack);
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral (stack);
	auto result = functr (lhs, rhs);
	switch (type.getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY: {
			abort ();
			break;
		}
		case Numeric::ArithmaticType::DECINT:
			abort ();
			break;
		case Numeric::ArithmaticType::FPINT:
			abort ();
			break;
		case Numeric::ArithmaticType::INT:
			if (type.getMinPrec () <= 8) {
				result = int8_t (result.to_int64 ());
			} else if (type.getMinPrec () <= 16) {
				result = int16_t (result.to_int64 ());
			} else if (type.getMinPrec () <= 16) {
				result = int32_t (result.to_int64 ());
			} else {
				result = int64_t (result.to_int64 ());
			}
			break;
		case Numeric::ArithmaticType::UINT:
			if (type.getMinPrec () <= 8) {
				result = uint8_t (result.to_uint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = uint16_t (result.to_uint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = uint32_t (result.to_uint64 ());
			} else {
				result = uint64_t (result.to_uint64 ());
			}
			break;
	}
	return std::make_shared <StaticValue> (args [0]->getPos (), args [0]->getFile (), Numeric::get (type.getArith (), type.getMinPrec ()), result);
}

template <typename FUNCTOR>
NodePtr standardBinary (const Numeric & type, LiteralStack & stack, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral (stack);
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral (stack);
	auto result = functr (lhs, rhs);
	switch (type.getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY: {
			result = result.to_aint0 ();
			break;
		}
		case Numeric::ArithmaticType::DECINT:
			abort ();
			break;
		case Numeric::ArithmaticType::FPINT:
			result = result.to_double ();
			break;
		case Numeric::ArithmaticType::INT:
			if (type.getMinPrec () <= 8) {
				result = int8_t (result.to_int64 ());
			} else if (type.getMinPrec () <= 16) {
				result = int16_t (result.to_int64 ());
			} else if (type.getMinPrec () <= 16) {
				result = int32_t (result.to_int64 ());
			} else {
				result = int64_t (result.to_int64 ());
			}
			break;
		case Numeric::ArithmaticType::UINT:
			if (type.getMinPrec () <= 8) {
				result = uint8_t (result.to_uint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = uint16_t (result.to_uint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = uint32_t (result.to_uint64 ());
			} else {
				result = uint64_t (result.to_uint64 ());
			}
			break;
	}
	return std::make_shared <StaticValue> (args [0]->getPos (), args [1]->getFile (), Numeric::get (type.getArith (), type.getMinPrec ()), result);
}

template <typename FUNCTOR>
NodePtr predicateBinary (LiteralStack & stack, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral (stack);
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral (stack);
	auto result = functr (lhs, rhs);
	return std::make_shared <StaticValue> (args [0]->getPos (), args [1]->getFile (), Numeric::get (Numeric::ArithmaticType::UINT, 1), result);
}

template <parser::OPERATION OPP>
void OperatePass <OPP>::visit (const Array & node) {
	abort ();
}

template <parser::OPERATION OPP>
void OperatePass <OPP>::visit (const Const & node) {
	abort ();
}

template <parser::OPERATION OPP>
void OperatePass <OPP>::visit (const Ref & node) {
	abort ();
}

template <parser::OPERATION OPP>
void OperatePass <OPP>::visit (const Interface & node) {
	abort ();
}

template <parser::OPERATION OPP>
void OperatePass <OPP>::visit (const Numeric & node) {
	abort ();
}

template <parser::OPERATION OPP>
void OperatePass <OPP>::visit (const Struct & node) {
	abort ();
}


template <>
void OperatePass <parser::OPERATION::CONCAT>::visit (const Array & node) {
	auto lhs = static_cast <const Value *> (args [0].get ())->getLiteral (stack);
	auto rhs = static_cast <const Value *> (args [1].get ())->getLiteral (stack);
	nPtr = std::make_shared <StaticValue> (args [0]->getPos (), args [1]->getFile (), args [0]->getType (), lhs.concat (rhs));
}

template <>
void OperatePass <parser::OPERATION::CMPEQ>::visit (const Array & node) {
	nPtr = predicateBinary (stack, args, std::equal_to <> ());
}


template <>
void OperatePass <parser::OPERATION::ADD>::visit (const Numeric & node) {
	nPtr = standardBinary (node, stack, args, std::plus <> ());
}

template <>
void OperatePass <parser::OPERATION::BAND>::visit (const Numeric & node) {
	nPtr = bitwiseBinary (node, stack, args, std::bit_and <> ());
}

template <>
void OperatePass <parser::OPERATION::BOR>::visit (const Numeric & node) {
	nPtr = bitwiseBinary (node, stack, args, std::bit_or <> ());
}

template <>
void OperatePass <parser::OPERATION::BXOR>::visit (const Numeric & node) {
	nPtr = bitwiseBinary (node, stack, args, std::bit_xor <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPEQ>::visit (const Numeric & node) {
	nPtr = predicateBinary (stack, args, std::equal_to <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPGT>::visit (const Numeric & node) {
	nPtr = predicateBinary (stack, args, std::greater <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPGTE>::visit (const Numeric & node) {
	nPtr = predicateBinary (stack, args, std::greater_equal <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPLT>::visit (const Numeric & node) {
	nPtr = predicateBinary (stack, args, std::less <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPLTE>::visit (const Numeric & node) {
	nPtr = predicateBinary (stack, args, std::less_equal <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPNE>::visit (const Numeric & node) {
	nPtr = predicateBinary (stack, args, std::not_equal_to <> ());
}

template <>
void OperatePass <parser::OPERATION::DIV>::visit (const Numeric & node) {
	nPtr = standardBinary (node, stack, args, std::divides <> ());
}

template <>
void OperatePass <parser::OPERATION::LNOT>::visit (const Numeric & node) {
	bool val;
	auto * arg = static_cast <Value *> (args [0].get ());
	val = not arg->getLiteral (stack);
	nPtr = std::make_shared <StaticValue> (arg->getPos (), arg->getFile (), Numeric::get (Numeric::ArithmaticType::UINT, 1), val);
}

template <>
void OperatePass <parser::OPERATION::MUL>::visit (const Numeric & node) {
	nPtr = standardBinary (node, stack, args, std::multiplies <> ());
}

template <>
void OperatePass <parser::OPERATION::SUB>::visit (const Numeric & node) {
	nPtr = standardBinary (node, stack, args, std::minus <> ());
}


}
