#include <codegen/interpreter.hpp>
#include <nir/block.hpp>

#include <functional>

namespace codegen {

using namespace nir;

std::vector <Interpreter::Atom> Interpreter::callFunction (const Function * func, const std::vector <Interpreter::Atom> & args) {
	
	Interpreter call;
	
	auto & funcArgs = func->getArgs ();
	assert (funcArgs.size () == args.size ());
	for (size_t i=0, l=args.size (); i<l; ++i) {
		const Interpreter::Atom & atom = args [i];
		const Parameter * param = funcArgs [i];
		call.lookupTable [param->getIden ()] = atom;
	}
	
	return call (func->getEntryPoint ());
	
}


template <typename K>
Interpreter::Atom Interpreter::binaryOpp (Argument ilhs, Argument irhs, const Type * type) {
	Interpreter::Atom lhs = staticCast (lookupAtom (ilhs), type);
	Interpreter::Atom rhs = staticCast (lookupAtom (irhs), type);
	Atom ret = rhs;
	K T;
	switch (lhs.atomicType) {
		case DATA:
			assert (false);
		case F32:
			ret.data.f32 = T (lhs.data.f32, rhs.data.f32);
			break;
		case F64:
			ret.data.f64 = T (lhs.data.f64, rhs.data.f64);
			break;
		case S8:
			ret.data.s8 = T (lhs.data.s8, rhs.data.s8);
			break;
		case S16:
			ret.data.s16 = T (lhs.data.s16, rhs.data.s16);
			break;
		case S32:
			ret.data.s32 = T (lhs.data.s32, rhs.data.s32);
			break;
		case S64:
			ret.data.s64 = T (lhs.data.s64, rhs.data.s64);
			break;
		case U8:
			ret.data.u8 = T (lhs.data.u8, rhs.data.u8);
			break;
		case U16:
			ret.data.u16 = T (lhs.data.u16, rhs.data.u16);
			break;
		case U32:
			ret.data.u32 = T (lhs.data.u32, rhs.data.u32);
			break;
		case U64:
			ret.data.u64 = T (lhs.data.u64, rhs.data.u64);
			break;
	}
	return ret;
}

template <typename K>
Interpreter::Atom Interpreter::bitwiseOpp (Argument ilhs, Argument irhs, const Type * type) {
	Interpreter::Atom lhs = staticCast (lookupAtom (ilhs), type);
	Interpreter::Atom rhs = staticCast (lookupAtom (irhs), type);
	Atom ret = rhs;
	K T;
	switch (lhs.atomicType) {
		case DATA:
		case F32:
		case F64:
			assert (false);
			break;
		case S8:
			ret.data.s8 = T (lhs.data.s8, rhs.data.s8);
			break;
		case S16:
			ret.data.s16 = T (lhs.data.s16, rhs.data.s16);
			break;
		case S32:
			ret.data.s32 = T (lhs.data.s32, rhs.data.s32);
			break;
		case S64:
			ret.data.s64 = T (lhs.data.s64, rhs.data.s64);
			break;
		case U8:
			ret.data.u8 = T (lhs.data.u8, rhs.data.u8);
			break;
		case U16:
			ret.data.u16 = T (lhs.data.u16, rhs.data.u16);
			break;
		case U32:
			ret.data.u32 = T (lhs.data.u32, rhs.data.u32);
			break;
		case U64:
			ret.data.u64 = T (lhs.data.u64, rhs.data.u64);
			break;
	}
	return ret;
}

void Interpreter::reset () {
	cleanup ();
	*this = Interpreter ();
}

void Interpreter::visit (const Add & add) {
	
	Atom res = binaryOpp <std::plus <void>> (add.getLhs (), add.getRhs (), add.getType ());
	lookupTable [add.getIden ()] = res;
	
}

void Interpreter::visit (const Alloc & alloc) {
	
	Atom amount = lookupAtom (alloc.getAmount ());
	const Type * type = alloc.getType ()->getDereferenceType ();
	size_t size = (type->calculateSize (8) + 7)/8;
	uint8_t * data = new uint8_t [size];
	Atom res;
	res.type = alloc.getType ();
	res.atomicType = AtomicType::DATA;
	res.data.ptr = data;
	ptrs.insert (data);
	lookupTable [alloc.getIden ()] = res;
	
}

void Interpreter::visit (const BitAnd & bitAnd) {
	Atom res = bitwiseOpp <std::bit_and <void>> (bitAnd.getLhs (), bitAnd.getRhs (), bitAnd.getType ());
	lookupTable [bitAnd.getIden ()] = res;
}

void Interpreter::visit (const BitOr & bitOr) {
	Atom res = bitwiseOpp <std::bit_or <void>> (bitOr.getLhs (), bitOr.getRhs (), bitOr.getType ());
	lookupTable [bitOr.getIden ()] = res;
}

void Interpreter::visit (const BitXor & bitXor) {
	Atom res = bitwiseOpp <std::bit_xor <void>> (bitXor.getLhs (), bitXor.getRhs (), bitXor.getType ());
	lookupTable [bitXor.getIden ()] = res;
}

void Interpreter::visit (const Block & block) {
	
}

void Interpreter::visit (const Constant & con) {
	
	Atom res;
	res.type = con.getType ();
	if (con.getType ()->calculateSize (8) == 0) return;
	
	switch (con.getType ()->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			assert (false);
		case Type::FPINT:
			res.atomicType = AtomicType::F64;
			res.data.f64 = std::stod (con.getVal ());
			break;
		case Type::INT:
		case Type::UINT:
			res.atomicType = AtomicType::U64;
			res.data.u64 = std::stoull (con.getVal ());
			break;
	}
	lookupTable [con.getIden ()] = res;
	
}

void Interpreter::visit (const Div & div) {
	
	Atom res = binaryOpp <std::divides <void>> (div.getLhs (), div.getRhs (), div.getType ());
	lookupTable [div.getIden ()] = res;
	
}

void Interpreter::visit (const DirectCall & call) {
	
	std::vector <Atom> args;
	args.reserve (call.getArgs ().size ());
	for (auto & a : call.getArgs ()) {
		args.push_back (lookupAtom (a));
	}
	auto ret = callFunction (call.getFunc (), args);
	assert (ret.size () == call.getIdens ().size ());
	const auto & idens = call.getIdens ();
	for (size_t i=0, l=ret.size (); i<l; ++i) {
		lookupTable [idens [i]] = ret [i];
	}
	
}

void Interpreter::visit (const Function * func) {
	
}

void Interpreter::visit (const FunctionPointer & func) {
	
	Atom res;
	res.atomicType = AtomicType::FUNCTION;
	res.type = func.getType ();
	res.data.func = func.getFunction ();
	lookupTable [func.getIden ()] = res;
	
}

void Interpreter::visit (const Get & get) {
	
	Atom src = lookupAtom (get.getSrc ());
	const Type * type = src.type->getDereferenceType ();
	assert (type);
	Atom res;
	res.type = type;
	switch (type->getArithmaticType ()) {
		case Type::DECINT:
			assert (false);
		case Type::DEFAULT:
			res.atomicType = AtomicType::STRUCT;
			res.type = type;
			res.data = src.data;
			break;
		case Type::FPINT:
			res.atomicType = AtomicType::F64;
			std::copy (src.data.ptr, src.data.ptr+8, reinterpret_cast <uint8_t *> (&res.data));
			break;
		case Type::INT:
		case Type::UINT:
			res.atomicType = AtomicType::U64;
			std::copy (src.data.ptr, src.data.ptr+8, reinterpret_cast <uint8_t *> (&res.data));
			break;
	}
	lookupTable [get.getIden ()] = res;
	
}

void Interpreter::visit (const Less & less) {
	
	Atom res = binaryOpp <std::less <void>> (less.getLhs (), less.getRhs (), less.getType ());
	lookupTable [less.getIden ()] = res;
	
}

void Interpreter::visit (const Mul & mul) {
	
	Atom res = binaryOpp <std::multiplies <void>> (mul.getLhs (), mul.getRhs (), mul.getType ());
	lookupTable [mul.getIden ()] = res;
	
}

void Interpreter::visit (const Neg & neg) {
	
	Interpreter::Atom arg = lookupAtom (neg.getArg ());
	switch (arg.atomicType) {
		case DATA:
			assert (false);
		case F32:
			arg.data.f32 = -arg.data.f32;
			break;
		case F64:
			arg.data.f64 = -arg.data.f64;
			break;
		case S8:
			arg.data.s8 = -arg.data.s8;
			break;
		case S16:
			arg.data.s16 = -arg.data.s16;
			break;
		case S32:
			arg.data.s32 = -arg.data.s32;
			break;
		case S64:
			arg.data.s64 = -arg.data.s64;
			break;
		case U8:
			arg.data.u8 = -arg.data.u8;
			break;
		case U16:
			arg.data.u16 = -arg.data.u16;
			break;
		case U32:
			arg.data.u32 = -arg.data.u32;
			break;
		case U64:
			arg.data.u64 = -arg.data.u64;
			break;
	}
	lookupTable [neg.getIden ()] = arg;
	
}

void Interpreter::visit (const Number & num) {
	
}

void Interpreter::visit (const Parameter & param) {
	
	assert (false);
	
}

void Interpreter::visit (const Put & put) {
	
	Atom dest = lookupAtom (put.getDest ());
	Atom src = lookupAtom (put.getSrc ());
	assert (dest.atomicType == AtomicType::DATA);
	size_t amount = src.type->calculateSize (8);
	std::copy (reinterpret_cast <uint8_t *> (&src.data), reinterpret_cast <uint8_t *> (&src.data)+amount, dest.data.ptr);
	
}

void Interpreter::visit (const Ret & ret) {
	
	for (Argument arg : ret.getArgs ()) {
		lookupAtom (arg);
	}
	
}

void Interpreter::visit (const Sub & sub) {
	
	Atom res = binaryOpp <std::minus <void>> (sub.getLhs (), sub.getRhs (), sub.getType ());
	lookupTable [sub.getIden ()] = res;
	
}

void Interpreter::visit (const Type * type) {
	
}

Interpreter::Atom Interpreter::lookupAtom (Argument val) {
	
	if (val.sym) {
		auto itt = lookupTable.find (val.instr->getIden ());
		if (itt != lookupTable.end ()) {
			return itt->second;
		}
	}
	
	val.instr->accept (*this);
	auto itt = lookupTable.find (val.sym ? val.sym : val.instr->getIden ());
	if (itt != lookupTable.end ()) {
		return itt->second;
	}
	return Atom ();
	
}


template <typename T>
T atomCast (const Interpreter::Atom & atom) {
	
	switch (atom.atomicType) {
		case Interpreter::AtomicType::DATA:
			assert (false);
		case Interpreter::AtomicType::F32:
			return T (atom.data.f32);
		case Interpreter::AtomicType::F64:
			return T (atom.data.f64);
		case Interpreter::AtomicType::S8:
			return T (atom.data.s8);
		case Interpreter::AtomicType::S16:
			return T (atom.data.s16);
		case Interpreter::AtomicType::S32:
			return T (atom.data.s32);
		case Interpreter::AtomicType::S64:
			return T (atom.data.s64);
		case Interpreter::AtomicType::U8:
			return T (atom.data.u8);
		case Interpreter::AtomicType::U16:
			return T (atom.data.u16);
		case Interpreter::AtomicType::U32:
			return T (atom.data.u32);
		case Interpreter::AtomicType::U64:
			return T (atom.data.u64);
		default:
			assert (false);
			return T ();
	}
	
}

Interpreter::Atom Interpreter::staticCast (const Interpreter::Atom & source, const Type * const target) {
	
	if (source.type == target) {
		return source;
	}
	
	Atom result;
	switch (target->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			assert (false);
		case Type::FPINT:
			result.atomicType = AtomicType::F64;
			result.data.f64 = atomCast <double> (source);
			break;
		case Type::INT:
			result.atomicType = AtomicType::S64;
			result.data.s64 = atomCast <int64_t> (source);
			break;
		case Type::UINT:
			result.atomicType = AtomicType::U64;
			result.data.u64 = atomCast <uint64_t> (source);
			break;
	}
	return result;
	
}


std::string Interpreter::operator ()(const nir::Instruction * val) {
	
	if (const Type * type = val->getType ()->getDereferenceType ()) {
		auto get = Get (type, {val, nullptr}, nullptr);
		return this->operator ()(&get);
	}
	
	std::string s;
	
	for (Atom atom : lookupAtoms (val)) {
		
		switch (atom.atomicType) {
			case DATA:
				s += "Code could not be interpreted";
				break;
			case F32:
				s += std::to_string (atom.data.f32);
				break;
			case F64:
				s += std::to_string (atom.data.f64);
				break;
			case FUNCTION:
				s += atom.data.func->getType ()->toString ();
				break;
			case S8:
				s += std::to_string (atom.data.s8);
				break;
			case S16:
				s += std::to_string (atom.data.s16);
				break;
			case S32:
				s += std::to_string (atom.data.s32);
				break;
			case S64:
				s += std::to_string (atom.data.s64);
				break;
			case U8:
				s += std::to_string (atom.data.u8);
				break;
			case U16:
				s += std::to_string (atom.data.u16);
				break;
			case U32:
				s += std::to_string (atom.data.u32);
				break;
			case U64:
				s += std::to_string (atom.data.u64);
				break;
			case VOID:
				s += "void";
				break;
			default:
				abort ();
		}
		s += " ";
		
	}
	s.pop_back ();
	
	return s;
	
}

std::vector <Interpreter::Atom> Interpreter::operator ()(const Block * block) {
	
	currentBlock = block;
	std::vector <Interpreter::Atom> last;
	for (auto & val : block->getInstructions ()) {
		last = lookupAtoms (val);
		if (currentBlock != block) return (*this)(currentBlock);
	}
	return std::vector <Interpreter::Atom> (last);
	
}

std::vector <Interpreter::Atom> Interpreter::lookupAtoms (const Instruction * val) {
	
	size_t l=val->getIdens ().size ();
	std::vector <Interpreter::Atom> atoms;
	atoms.resize (l);
	const auto & idens = val->getIdens ();
	
	for (size_t i=0; i<l; ++i) {
		auto itt = lookupTable.find (idens [i]);
		
		if (itt != lookupTable.end ()) {
			atoms [i] = itt->second;
		}
	}
	
	val->accept (*this);
	
	for (size_t i=0; i<l; ++i) {
		auto itt = lookupTable.find (idens [i]);
		
		if (itt != lookupTable.end ()) {
			atoms [i] = itt->second;
		} else {
			abort ();
		}
		
	}
	
	return atoms;
	
}


Interpreter::~Interpreter () {
	cleanup ();
}

void Interpreter::cleanup() {
	for (void * p : ptrs) {
		delete p;
	}
	ptrs.clear ();
}


};