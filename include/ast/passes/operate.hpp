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
	const string & lhs = static_cast <Number*> (args [0].get ())->getValue ();
	const string & rhs = static_cast <Number*> (args [1].get ())->getValue ();
	string result;
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
				result = std::to_string (functr (int8_t (std::stoll (lhs)), int8_t (std::stoll (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int16_t (std::stoll (lhs)), int16_t (std::stoll (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int32_t (std::stoll (lhs)), int32_t (std::stoll (rhs))));
			} else {
				result = std::to_string (functr (int64_t (std::stoll (lhs)), int64_t (std::stoll (rhs))));
			}
			break;
		case Numeric::ArithmaticType::UINT:
			if (type.getMinPrec () <= 8) {
				result = std::to_string (functr (int8_t (std::stoull (lhs)), int8_t (std::stoull (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int16_t (std::stoull (lhs)), int16_t (std::stoull (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int32_t (std::stoull (lhs)), int32_t (std::stoull (rhs))));
			} else {
				result = std::to_string (functr (int64_t (std::stoull (lhs)), int64_t (std::stoull (rhs))));
			}
			break;
	}
	return std::make_shared <Number> (args [0]->getPos (), args [0]->getFile (), result, Numeric::get (type.getArith (), type.getMinPrec ()));
}

template <typename FUNCTOR>
NodePtr standardBinary (const Numeric & type, const BasicArray <NodePtr> & args, FUNCTOR functr) {
	const string & lhs = static_cast <Number*> (args [0].get ())->getValue ();
	const string & rhs = static_cast <Number*> (args [1].get ())->getValue ();
	string result;
	switch (type.getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY: {
			mpq_class l, r;
			string::size_type pos;
			if ((pos = lhs.find ('.')) != string::npos) {
				l = lhs.substr (0, pos) + lhs.substr (pos + 1) + "/1" + string (lhs.length () - (pos+1), '0');
			} else {
				l = lhs;
			}
			if ((pos = rhs.find ('.')) != string::npos) {
				r = rhs.substr (0, pos) + rhs.substr (pos + 1) + "/1" + string (rhs.length () - (pos+1), '0');
			} else {
				r = rhs;
			}
			result = mpq_class (functr (l, r)).get_str ();
			break;
		}
		case Numeric::ArithmaticType::DECINT:
			abort ();
			break;
		case Numeric::ArithmaticType::FPINT:
			if (type.getMinPrec () <= 32) {
				result = std::to_string (functr (std::stof (lhs), std::stof (rhs)));
			} else if (type.getMinPrec () <= 64) {
				result = std::to_string (functr (std::stod (lhs), std::stod (rhs)));
			} else {
				result = std::to_string (functr (std::stold (lhs), std::stold (rhs)));
			}
			break;
		case Numeric::ArithmaticType::INT:
			if (type.getMinPrec () <= 8) {
				result = std::to_string (functr (int8_t (std::stoll (lhs)), int8_t (std::stoll (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int16_t (std::stoll (lhs)), int16_t (std::stoll (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int32_t (std::stoll (lhs)), int32_t (std::stoll (rhs))));
			} else {
				result = std::to_string (functr (int64_t (std::stoll (lhs)), int64_t (std::stoll (rhs))));
			}
			break;
		case Numeric::ArithmaticType::UINT:
			if (type.getMinPrec () <= 8) {
				result = std::to_string (functr (int8_t (std::stoull (lhs)), int8_t (std::stoull (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int16_t (std::stoull (lhs)), int16_t (std::stoull (rhs))));
			} else if (type.getMinPrec () <= 16) {
				result = std::to_string (functr (int32_t (std::stoull (lhs)), int32_t (std::stoull (rhs))));
			} else {
				result = std::to_string (functr (int64_t (std::stoull (lhs)), int64_t (std::stoull (rhs))));
			}
			break;
	}
	return std::make_shared <Number> (args [0]->getPos (), args [1]->getFile (), result, Numeric::get (type.getArith (), type.getMinPrec ()));
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
	nPtr = standardBinary (node, args, std::equal_to <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPGT>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::greater <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPGTE>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::greater_equal <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPLT>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::less <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPLTE>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::less_equal <> ());
}

template <>
void OperatePass <parser::OPERATION::CMPNE>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::not_equal_to <> ());
}

template <>
void OperatePass <parser::OPERATION::DIV>::visit (const Numeric & node) {
	nPtr = standardBinary (node, args, std::divides <> ());
}

template <>
void OperatePass <parser::OPERATION::LNOT>::visit (const Numeric & node) {
	bool val;
	auto * arg = static_cast <Number *> (args [0].get ());
	switch (node.getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY: {
			mpq_class mpq;
			string::size_type pos;
			if ((pos = arg->getValue ().find ('.')) != string::npos) {
				mpq = arg->getValue ().substr (0, pos) + arg->getValue ().substr (pos + 1) + "/1" + string (arg->getValue ().length () - (pos+1), '0');
			} else {
				mpq = arg->getValue ();
			}
			val = not mpq;
			break;
		}
		case Numeric::ArithmaticType::DECINT:
			abort ();
			break;
		case Numeric::ArithmaticType::FPINT:
			val = not std::stold (arg->getValue ());
			break;
		case Numeric::ArithmaticType::INT:
			val = not std::stoll (arg->getValue ());
			break;
		case Numeric::ArithmaticType::UINT:
			val = not std::stoull (arg->getValue ());
			break;
	}
	nPtr = std::make_shared <Number> (arg->getPos (), arg->getFile (), std::to_string (val), Numeric::get (Numeric::ArithmaticType::UINT, 1));
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
