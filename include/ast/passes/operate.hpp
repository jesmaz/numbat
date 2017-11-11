#pragma once


#include <ast/literal.hpp>
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
		assert (var->getType ()->getRegType () == args [1]->getType ()->getRegType ());
		if (typeid (*args [1].get ()) == typeid (Variable)) {
			Variable * rhs = static_cast <Variable *> (args [1].get ());
			var->getCurrentValue () = rhs->getCurrentValue ();
		} else if (typeid (*args [1].get ()) == typeid (Value)) {
			Value * rhs = static_cast <Value *> (args [1].get ());
			var->getCurrentValue () = rhs->getLiteral ();
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
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral ();
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral ();
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
	return std::make_shared <Value> (args [0]->getPos (), args [0]->getFile (), Numeric::get (type.getArith (), type.getMinPrec ()), result);
}

template <typename FUNCTOR>
NodePtr standardBinary (const Numeric & type, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral ();
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral ();
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
	return std::make_shared <Value> (args [0]->getPos (), args [1]->getFile (), Numeric::get (type.getArith (), type.getMinPrec ()), result);
}

template <typename FUNCTOR>
NodePtr predicateBinary (const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const auto & lhs = static_cast <Value*> (args [0].get ())->getLiteral ();
	const auto & rhs = static_cast <Value*> (args [1].get ())->getLiteral ();
	auto result = functr (lhs, rhs);
	return std::make_shared <Value> (args [0]->getPos (), args [1]->getFile (), Numeric::get (Numeric::ArithmaticType::UINT, 1), result);
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
	auto lhs = static_cast <const Value *> (args [0].get ())->getLiteral ();
	auto rhs = static_cast <const Value *> (args [1].get ())->getLiteral ();
	nPtr = std::make_shared <Value> (args [0]->getPos (), args [1]->getFile (), args [0]->getType (), lhs.concat (rhs));
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
	auto * arg = static_cast <Value *> (args [0].get ());
	val = not arg->getLiteral ();
	nPtr = std::make_shared <Value> (arg->getPos (), arg->getFile (), Numeric::get (Numeric::ArithmaticType::UINT, 1), val);
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
