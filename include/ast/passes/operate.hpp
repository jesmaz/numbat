#pragma once


#include <ast/literal.hpp>
#include <ast/passes/shallnot.hpp>
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
		
		OperatePass (const BasicArray <NodePtr> & args) : args (args) {}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
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
	Variable * var = static_cast <Variable *> (args [0].get ());
	if (var) {
		assert (var->getType () == args [1]->getType ());
		if (args [1]->isValue ()) {
			var->getCurrentValue () = std::dynamic_pointer_cast <Value> (args [1]);
		} else if (typeid (args [1].get ()) == typeid (Variable*)) {
			Variable * rhs = static_cast <Variable *> (args [1].get ());
			var->getCurrentValue () = rhs->getCurrentValue ();
		} else {
			abort ();
		}
	} else {
		abort ();
	}
	return args [0];
}

template <typename FUNCTOR>
NodePtr bitwiseBinary (const Numeric & type, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Number*> (args [0].get ())->getValue ();
	const auto & rhs = static_cast <Number*> (args [1].get ())->getValue ();
	auto val = functr (*lhs, *rhs);
	std::shared_ptr <NumericLiteral> result;
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
				result = std::make_shared <NumericLiteralTemplate <int8_t>> (val->toInt64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <int16_t>> (val->toInt64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <int32_t>> (val->toInt64 ());
			} else {
				result = std::make_shared <NumericLiteralTemplate <int64_t>> (val->toInt64 ());
			}
			break;
		case Numeric::ArithmaticType::UINT:
			if (type.getMinPrec () <= 8) {
				result = std::make_shared <NumericLiteralTemplate <uint8_t>> (val->toUint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <uint16_t>> (val->toUint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <uint32_t>> (val->toUint64 ());
			} else {
				result = std::make_shared <NumericLiteralTemplate <uint64_t>> (val->toUint64 ());
			}
			break;
	}
	return std::make_shared <Number> (args [0]->getPos (), args [0]->getFile (), result, Numeric::get (type.getArith (), type.getMinPrec ()));
}

template <typename FUNCTOR>
NodePtr standardBinary (const Numeric & type, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Number*> (args [0].get ())->getValue ();
	const auto & rhs = static_cast <Number*> (args [1].get ())->getValue ();
	auto val = functr (*lhs, *rhs);
	std::shared_ptr <NumericLiteral> result;
	switch (type.getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY: {
			result = std::make_shared <NumericLiteralTemplate <mpq_class>> (val->toMPQ ());
			break;
		}
		case Numeric::ArithmaticType::DECINT:
			abort ();
			break;
		case Numeric::ArithmaticType::FPINT:
			result = std::make_shared <NumericLiteralTemplate <mpq_class>> (val->toMPQ ());
			break;
		case Numeric::ArithmaticType::INT:
			if (type.getMinPrec () <= 8) {
				result = std::make_shared <NumericLiteralTemplate <int8_t>> (val->toInt64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <int16_t>> (val->toInt64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <int32_t>> (val->toInt64 ());
			} else {
				result = std::make_shared <NumericLiteralTemplate <int64_t>> (val->toInt64 ());
			}
			break;
		case Numeric::ArithmaticType::UINT:
			if (type.getMinPrec () <= 8) {
				result = std::make_shared <NumericLiteralTemplate <uint8_t>> (val->toUint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <uint16_t>> (val->toUint64 ());
			} else if (type.getMinPrec () <= 16) {
				result = std::make_shared <NumericLiteralTemplate <uint32_t>> (val->toUint64 ());
			} else {
				result = std::make_shared <NumericLiteralTemplate <uint64_t>> (val->toUint64 ());
			}
			break;
	}
	return std::make_shared <Number> (args [0]->getPos (), args [1]->getFile (), result, Numeric::get (type.getArith (), type.getMinPrec ()));
}

template <typename FUNCTOR>
NodePtr predicateBinary (const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral ();
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral ();
	auto result = std::make_shared <NumericLiteralTemplate <bool>> (functr (*lhs, *rhs));
	return std::make_shared <Number> (args [0]->getPos (), args [1]->getFile (), result, Numeric::get (Numeric::ArithmaticType::UINT, 1));
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
	auto lhs = static_cast <const ArrayVal *> (args [0].get ())->getValue ();
	auto rhs = static_cast <const ArrayVal *> (args [1].get ())->getValue ();
	BasicArray <Literal> result (lhs->getData().size () + rhs->getData().size ());
	auto itt = std::copy (lhs->getData ().begin (), lhs->getData ().end (), result.begin ());
	std::copy (rhs->getData ().begin (), rhs->getData ().end (), itt);
	nPtr = std::make_shared <ArrayVal> (args [0]->getPos (), args [1]->getFile (), std::make_shared <ArrayLiteral> (result), args [0]->getType ());
}

template <>
void OperatePass <parser::OPERATION::CMPEQ>::visit (const Array & node) {
	nPtr = predicateBinary (args, std::equal_to <> ());
}


template <>
void OperatePass <parser::OPERATION::ADD>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::plus <> ());
}

template <>
void OperatePass <parser::OPERATION::BAND>::visit (const Numeric & node) {
	nPtr = bitwiseBinary (node, args, std::bit_and <> ());
}

template <>
void OperatePass <parser::OPERATION::BOR>::visit (const Numeric & node) {
	nPtr = bitwiseBinary (node, args, std::bit_or <> ());
}

template <>
void OperatePass <parser::OPERATION::BXOR>::visit (const Numeric & node) {
	nPtr = bitwiseBinary (node, args, std::bit_xor <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPEQ>::visit (const Numeric & node) {
	nPtr = predicateBinary (args, std::equal_to <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPGT>::visit (const Numeric & node) {
	nPtr = predicateBinary (args, std::greater <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPGTE>::visit (const Numeric & node) {
	nPtr = predicateBinary (args, std::greater_equal <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPLT>::visit (const Numeric & node) {
	nPtr = predicateBinary (args, std::less <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPLTE>::visit (const Numeric & node) {
	nPtr = predicateBinary (args, std::less_equal <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPNE>::visit (const Numeric & node) {
	nPtr = predicateBinary (args, std::not_equal_to <> ());
}

template <>
void OperatePass <parser::OPERATION::DIV>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::divides <> ());
}

template <>
void OperatePass <parser::OPERATION::LNOT>::visit (const Numeric & node) {
	bool val;
	auto * arg = static_cast <Number *> (args [0].get ());
	val = not *arg->getValue ();
	nPtr = std::make_shared <Number> (arg->getPos (), arg->getFile (), std::make_shared <NumericLiteralTemplate <bool>> (val), Numeric::get (Numeric::ArithmaticType::UINT, 1));
}

template <>
void OperatePass <parser::OPERATION::MUL>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::multiplies <> ());
}

template <>
void OperatePass <parser::OPERATION::SUB>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::minus <> ());
}


}
