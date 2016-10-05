#include <codegen/interpreter.hpp>
#include <nir/block.hpp>
#include <nir/type/array.hpp>
#include <nir/type/pointer.hpp>
#include <nir/type/struct.hpp>

#include <functional>
#include <iostream>

namespace codegen {

using namespace nir;

BasicArray <Value> Interpreter::callFunction (const Function * func, const BasicArray <Value> & args) {
	
	Interpreter call (func->getEntryPoint ());
	auto & funcArgs = func->getArgs ();
	assert (funcArgs.size () == args.size ());
	
	for (size_t i=0, l=args.size (); i<l; ++i) {
		const Parameter * param = funcArgs [i];
		call.lookupTable [&param->getIden ()] = args [i];
	}
	
	return call ();
	
}


template <typename K>
Value Interpreter::binaryOpp (Argument ilhs, Argument irhs, const Type * type) {
	Value lhs = evaluate (ilhs);
	Value rhs = evaluate (irhs);
	K T;
	
	switch (type->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			assert (false);
		case Type::FPINT:
			return T (double (*lhs), double (*rhs));
		case Type::INT:
			return T (int64_t (*lhs), int64_t (*rhs));
		case Type::UINT:
			return T (uint64_t (*lhs), uint64_t (*rhs));
	}
}

template <typename K>
Value Interpreter::bitwiseOpp (Argument ilhs, Argument irhs, const Type * type) {
	Value lhs = evaluate (ilhs);
	Value rhs = evaluate (irhs);
	K T;
	
	switch (type->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
		case Type::FPINT:
			assert (false);
		case Type::INT:
			return T (int64_t (*lhs), int64_t (*rhs));
		case Type::UINT:
			return T (uint64_t (*lhs), uint64_t (*rhs));
	}
}

void Interpreter::reset () {
	cleanup ();
	*this = Interpreter (entry);
}

void Interpreter::visit (const Add & add) {
	
	Value res = binaryOpp <std::plus <void>> (add.getLhs (), add.getRhs (), add.getType ());
	lookupTable [add.getIden ()] = res;
	
}

void Interpreter::visit (const Alloc & alloc) {
	
	Value amount = evaluate (alloc.getAmount ());
	lookupTable [alloc.getIden ()] = Value::allocate (alloc.getType ()->getDereferenceType (), int64_t (*amount)).referenceTo ();
	
}

void Interpreter::visit (const BitAnd & bitAnd) {
	Value res = bitwiseOpp <std::bit_and <void>> (bitAnd.getLhs (), bitAnd.getRhs (), bitAnd.getType ());
	lookupTable [bitAnd.getIden ()] = res;
}

void Interpreter::visit (const BitNot & bitNot) {
	
	Value ret, arg = evaluate (bitNot.getArg ());
	switch (bitNot.getType ()->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
		case Type::FPINT:
			abort ();
		case Type::INT:
			ret = Value (~int64_t (*arg));
			break;
		case Type::UINT:
			ret = Value (~uint64_t (*arg));
			break;
	}
	lookupTable [bitNot.getIden ()] = ret;
	
}

void Interpreter::visit (const BitOr & bitOr) {
	Value res = bitwiseOpp <std::bit_or <void>> (bitOr.getLhs (), bitOr.getRhs (), bitOr.getType ());
	lookupTable [bitOr.getIden ()] = res;
}

void Interpreter::visit (const BitXor & bitXor) {
	Value res = bitwiseOpp <std::bit_xor <void>> (bitXor.getLhs (), bitXor.getRhs (), bitXor.getType ());
	lookupTable [bitXor.getIden ()] = res;
}

void Interpreter::visit (const Block & block) {
	
}

void Interpreter::visit (const Composite & comp) {
	
	auto l = comp.getArguments ().size ();
	DynArray <Value> vals (l);
	for (auto i=0ul; i<l; ++i) {
		vals [i] = evaluate (comp.getArguments () [i]);
	}
	const Struct * str = reinterpret_cast <const Struct *> (comp.getType ());
	if (typeid (*str) == typeid (Array)) str = &reinterpret_cast <const Array *> (str)->getUnderlyingType ();
	assert (typeid (*str) == typeid (Struct));
	lookupTable [comp.getIden ()] = Value (vals, str);
	
}

void Interpreter::visit (const Constant & con) {
	
	const auto & res = con.getResults ();
	const auto & values = con.getValues ();
	
	assert (res.size () == values.size ());
	
	for (size_t i=0, l=res.size (); i<l; ++i) {
		
		lookupTable [res [i].iden] = values [i];
		
	}
	
}

void Interpreter::visit (const Div & div) {
	
	Value res = binaryOpp <std::divides <void>> (div.getLhs (), div.getRhs (), div.getType ());
	lookupTable [div.getIden ()] = res;
	
}

void Interpreter::visit (const DirectCall & call) {
	
	DynArray <Value> args;
	for (auto & a : call.getArgs ()) {
		args.push_back (evaluate (a));
	}
	auto ret = callFunction (call.getFunc (), args);
	assert (ret.size () == call.getResults ().size ());
	const auto & res = call.getResults ();
	for (size_t i=0, l=ret.size (); i<l; ++i) {
		lookupTable [res [i].iden] = ret [i];
	}
	
}

void Interpreter::visit (const Equal & equal) {
	
	Value res = binaryOpp <std::equal_to <void>> (equal.getLhs (), equal.getRhs (), equal.getType ());
	lookupTable [equal.getIden ()] = res;
	
}

void Interpreter::visit (const Function * func) {
	
}

void Interpreter::visit (const FunctionPointer & func) {
	
	lookupTable [func.getIden ()] = Value (func.getFunction ());
	
}

void Interpreter::visit (const Get & get) {
	
	lookupTable [get.getIden ()] = evaluate (get.getSrc ())->dereference ();
	
}

void Interpreter::visit (const Jump & jump) {
	
	bool doJump = true;
	if (jump.getCondition ().instr) {
		doJump = bool (uint64_t (*evaluate (jump.getCondition ())));
	}
	
	if (doJump) {
		instItt = jump.getBlock ()->beg ();
	}
	
}

void Interpreter::visit (const Less & less) {
	
	Value res = binaryOpp <std::less <void>> (less.getLhs (), less.getRhs (), less.getType ());
	lookupTable [less.getIden ()] = res;
	
}

void Interpreter::visit (const LessEqual & lessequal) {
	
	Value res = binaryOpp <std::less_equal <void>> (lessequal.getLhs (), lessequal.getRhs (), lessequal.getType ());
	lookupTable [lessequal.getIden ()] = res;
	
}

void Interpreter::visit (const Mul & mul) {
	
	Value res = binaryOpp <std::multiplies <void>> (mul.getLhs (), mul.getRhs (), mul.getType ());
	lookupTable [mul.getIden ()] = res;
	
}

void Interpreter::visit (const Neg & neg) {
	
	Value ret, arg = evaluate (neg.getArg ());
	switch (neg.getType ()->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			abort ();
		case Type::FPINT:
			ret = Value (-double (*arg));
			break;
		case Type::INT:
			ret = Value (-int64_t (*arg));
			break;
		case Type::UINT:
			ret = Value (-uint64_t (*arg));
			break;
	}
	lookupTable [neg.getIden ()] = ret;
	
}

void Interpreter::visit (const NEqual & nequal) {
	
	Value res = binaryOpp <std::not_equal_to <void>> (nequal.getLhs (), nequal.getRhs (), nequal.getType ());
	lookupTable [nequal.getIden ()] = res;
	
}

void Interpreter::visit (const Number & num) {
	
}

void Interpreter::visit (const Parameter & param) {
	
	assert (false);
	
}

void Interpreter::visit (const PtrAdd & ptrAdd) {
	
	const Parameter * param = ptrAdd.getParam ();
	auto & args = ptrAdd.getArguments ();
	Value src = evaluate (args [0]);
	Value val = src->dereference ();
	if (param) {
		val = (*val) [param];
	} else {
		val = (*val) [uint64_t (*evaluate (args [1]))];
	}
	
	lookupTable [ptrAdd.getIden ()] = val.referenceTo ();
	
}

void Interpreter::visit (const Put & put) {
	
	Value src = evaluate (put.getSrc ());
	Value dest = evaluate (put.getDest ());
	*(dest->dereference ()) = *src;
	lookupTable [put.getIden ()] = src;
	
}

void Interpreter::visit (const Ret & ret) {
	
	for (Argument arg : ret.getArgs ()) {
		evaluate (arg);
	}
	
}

void Interpreter::visit (const Reinterpret & reinterpret) {
	
	const Type * type = reinterpret.getType ();
	lookupTable [reinterpret.getIden ()] = evaluate (reinterpret.getArg ())->reinterpret (type);
	
}

void Interpreter::visit (const Struct & stru) {
	
}

void Interpreter::visit (const Sub & sub) {
	
	Value res = binaryOpp <std::minus <void>> (sub.getLhs (), sub.getRhs (), sub.getType ());
	lookupTable [sub.getIden ()] = res;
	
}

void Interpreter::visit (const Type * type) {
	
}

Value Interpreter::evaluate (Argument val) {
	
	auto itt = lookupTable.find (val.sym ? val.sym : val.instr->getIden ());
	if (itt != lookupTable.end ()) {
		return itt->second;
	}
	
	val.instr->accept (*this);
	itt = lookupTable.find (val.sym ? val.sym : val.instr->getIden ());
	if (itt != lookupTable.end ()) {
		return itt->second;
	}
	return Value ();
	
}

Value Interpreter::staticCast (const Value & source, const Type * const target) {
	
	switch (target->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			assert (false);
		case Type::FPINT:
			return Value (double (*source));
		case Type::INT:
			return Value (int64_t (*source));
		case Type::UINT:
			return Value (uint64_t (*source));
	}
	
}


std::string Interpreter::operator ()(const nir::Instruction * val) {
	
	(*this) ();
	
	if (const Type * type = val->getType ()->getDereferenceType ()) {
		auto get = Get (type, {val, nullptr}, nullptr);
		return this->operator ()(&get);
	}
	
	std::string s;
	
	for (const Value & v : evaluate (val)) {
		
		s += v->toString (text::PrintMode::PLAIN) + " ";
		
	}
	s.pop_back ();
	
	return s;
	
}

BasicArray <Value> Interpreter::operator ()() {
	
	DynArray <Value> last;
	while (instItt) {
		auto & val = *instItt;
		++instItt;
		if (val.getIden ()) {
			last = evaluate (&val);
		} else {
			val.accept (*this);
		}
	}
	return last;
	
}

BasicArray <Value> Interpreter::evaluate (const Instruction * val) {
	
	size_t l=val->getResults ().size ();
	DynArray <Value> vals (l);
	const auto & res = val->getResults ();
	bool calc = false;
	
	for (size_t i=0; i<l; ++i) {
		auto itt = lookupTable.find (res [i].iden);
		
		if (itt != lookupTable.end ()) {
			vals [i] = itt->second;
		} else {
			calc = true;
		}
	}
	
	if (not calc) return vals;
	
	val->accept (*this);
	
	for (size_t i=0; i<l; ++i) {
		auto itt = lookupTable.find (res [i].iden);
		
		if (itt != lookupTable.end ()) {
			vals [i] = itt->second;
		} else {
			abort ();
		}
		
	}
	
	return vals;
	
}


Interpreter::~Interpreter () {
	cleanup ();
}

void Interpreter::cleanup() {
	
}


};
