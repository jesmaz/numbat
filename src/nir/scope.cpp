#include <cerrno>
#include <iostream>
#include <map>
#include <nir/block.hpp>
#include <nir/module.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/inst/add.hpp>
#include <nir/inst/alloc.hpp>
#include <nir/inst/bitwise.hpp>
#include <nir/inst/call.hpp>
#include <nir/inst/composite.hpp>
#include <nir/inst/constant.hpp>
#include <nir/inst/div.hpp>
#include <nir/inst/equal.hpp>
#include <nir/inst/functionPointer.hpp>
#include <nir/inst/get.hpp>
#include <nir/inst/jump.hpp>
#include <nir/inst/less.hpp>
#include <nir/inst/lessequal.hpp>
#include <nir/inst/mul.hpp>
#include <nir/inst/neg.hpp>
#include <nir/inst/nequal.hpp>
#include <nir/inst/pickStructMember.hpp>
#include <nir/inst/put.hpp>
#include <nir/inst/ptradd.hpp>
#include <nir/inst/reinterpret.hpp>
#include <nir/inst/rem.hpp>
#include <nir/inst/ret.hpp>
#include <nir/inst/sub.hpp>
#include <nir/type/array.hpp>
#include <nir/type/importHandle.hpp>
#include <nir/type/number.hpp>
#include <nir/type/pointer.hpp>
#include <utility/report.hpp>


namespace nir {


Scope * Scope::declareFunction (const BasicArray <const Parameter *> params, const BasicArray <const Parameter *> ret, const string iden, LINKAGE linkage) {
	
	string label = "nbt_" + iden;// + "_-";
	//for (Type * p : params) label += p->getIden () + "-";
	//label += "_-";
	//for (Type * r : ret) label += r->getIden () + "-";
	label += "_" + std::to_string (params.size ());
	label += "_" + std::to_string (ret.size ());
	
	Function * func = new Function (params, ret, label, linkage);
	
	symbol s;
	if (iden == "") {
		//Anonymous
		s = module->newSymbol (iden);
	} else {
		auto & fSet = functions [iden];
		if (!fSet) fSet = new DynArray <Function *>;
		fSet->push_back (func);
		s = module->newSymbol (iden);
	}
	if (not s) {
		assert (false);
		return nullptr;
	}
	module->data->functions [s] = func;
	Scope * scope = new Scope (module, func->getEntryPoint (), func);
	scope->parent = this;
	scopes.insert (scope);
	return scope;
	
}

const Type * promoteArithmatic (const Type * lhs, const Type * rhs) {
	//TODO: Deal with edge cases
	switch (lhs->getArithmaticType ()) {
		case Type::DECINT:
			return lhs;
		case Type::FPINT:
			switch (rhs->getArithmaticType ()) {
				case Type::DECINT:
					return rhs;
				case Type::FPINT:
				case Type::INT:
				case Type::UINT:
					return lhs;
				default:
					report::logMessage (report::ERROR, "'" + rhs->toString (text::PLAIN) + "' is not a numeric type");
			}
		case Type::INT:
			switch (rhs->getArithmaticType ()) {
				case Type::DECINT:
				case Type::FPINT:
					return rhs;
				case Type::INT:
				case Type::UINT:
					return lhs;
				default:
					report::logMessage (report::ERROR, "'" + rhs->toString (text::PLAIN) + "' is not a numeric type");
			}
		case Type::UINT:
			return rhs;
		default:
			report::logMessage (report::ERROR, "'" + lhs->toString (text::PLAIN) + "' is not a numeric type");
	}
	return lhs;
}


const Type * Scope::resolveType (const string & iden) const {
	
	auto titt = types.find (iden);
	if (titt != types.end ()) {
		return titt->second;
	}
	auto vitt = variables.find (iden);
	if (vitt != variables.end ()) {
		return vitt->second->getType ();
	}
	if (parent) {
		return parent->resolveType (iden);
	}
	auto itt = module->data->types.find (module->findSymbol (iden));
	if (itt != module->data->types.end ()) {
		return itt->second;
	}
	
	report::logMessage (report::ERROR, "Could not resolve '" + iden + "' as a type");
	return nullptr;
	
}

const Type * Scope::resolveType (Argument parent, const string & iden) const {
	
	const Type * type = parent.instr->getType ();
	if (typeid (*type) == typeid (Struct)) {
		
		report::logMessage (report::ERROR, "Nested structs are not yet supported");
		return nullptr;
		
	} else if (typeid (*type) == typeid (ImportHandle)) {
		
		const ImportHandle * imp = static_cast <const ImportHandle *> (type);
		const Scope * scope = imp->getScope ();
		return scope->resolveType (iden);
		
	}
	
	report::logMessage (report::ERROR, "'" + type->toString (text::PLAIN) + "' doesn't support nested types");
	return nullptr;
	
}

const Instruction * Scope::allocateArray (const Type * const type, Argument size, const string & iden) {
	
	Array * s = Array::arrayOf (type);
	const Instruction * ptr = allocateBytes (type, size, iden + "_ptr");
	const Instruction * var = allocateVariable (s, iden);
	const Instruction * val = createStructValue (s, {{ptr, ptr->getIden ()}, size});
	const Instruction * inst = createPut ({val, val->getIden ()}, {var, var->getIden ()});
	return inst;
	
}

const Instruction * Scope::allocateBytes (const Type * const type, Argument size, const string & iden) {
	
	Instruction * inst = new Alloc (type->getPointerTo (), size, module->newSymbol (iden));
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::allocateVariable (const Type * const type, const string & iden) {
	
	const Type * ctype = Number::getNumberType (Type::UINT, 32);
	auto * con = createConstant (ctype, "1");
	Argument amount = {con, con->getIden ()};
	Instruction * inst = new Alloc (type->getPointerTo (), amount, module->newSymbol (iden));
	return variables [iden] = insertionPoint->give (inst);
	
}

template <typename T>
const Instruction * Scope::createBinary (Argument lhs, Argument rhs, const string & iden) {
	Argument tlhs = lhs, trhs = rhs;
	if (lhs.instr->getType ()->getDereferenceType ()) {
		tlhs = createGet (lhs);
	}
	if (rhs.instr->getType ()->getDereferenceType ()) {
		trhs = createGet (rhs);
	}
	auto * t = promoteArithmatic (tlhs.instr->getType (), trhs.instr->getType ());
	Instruction * inst = new T (t, staticCast (tlhs, t), staticCast (trhs, t), module->newSymbol (iden));
	return insertionPoint->give (inst);
}

const Instruction * Scope::createAdd (Argument lhs, Argument rhs) {
	return createBinary <Add> (lhs, rhs, "add");
}

const Instruction * Scope::createAssign (Argument lhs, Argument rhs) {
	const Type * dref = lhs.instr->getType ()->getDereferenceType ();
	if (not dref) {
		report::logMessage (report::ERROR, "Can't assign to a constant");
		return nullptr;
	}
	Argument tsrc = rhs;
	if (rhs.instr->getType ()->getDereferenceType ()) {
		tsrc = createGet (rhs);
	}
	tsrc = staticCast (tsrc, dref);
	return createPut (tsrc, lhs);
}

const Instruction * Scope::createAutoReturn (const Instruction * instr) {
	
	if (owner->getRet ().empty ()) {
		Instruction * ret = new Ret ({});
		return insertionPoint->give (ret);
	} else {
		auto args = instr->getIdens ().map <Argument> ([=](symbol s){return Argument {instr, s};});
		Instruction * ret = new Ret (args);
		return insertionPoint->give (ret);
	}
	
}

const Instruction * Scope::createBitAnd (Argument lhs, Argument rhs) {
	return createBinary <BitAnd> (lhs, rhs, "bitand");
}

const Instruction * Scope::createBitNot (Argument arg) {
	return insertionPoint->give (new BitNot (arg, module->newSymbol ("bitnot")));
}

const Instruction * Scope::createBitOr (Argument lhs, Argument rhs) {
	return createBinary <BitOr> (lhs, rhs, "bitor");
}

const Instruction * Scope::createBitXor (Argument lhs, Argument rhs) {
	return createBinary <BitXor> (lhs, rhs, "bitxor");
}

const Instruction * Scope::createCall (const Function * func, const BasicArray <Argument> & args) {
	
	//TODO: appropriate casting
	BasicArray <symbol> idens (func->getRet ().size ());
	for (symbol & s : idens) {
		s = module->newSymbol ("");
	}
	BasicArray <Argument> svArgs (args.size ());
	std::copy (args.begin (), args.end (), svArgs.begin ());
	Instruction * inst = new DirectCall (func, svArgs, idens);
	return insertionPoint->give (inst);
	
}

template <typename T>
const Instruction * Scope::createCmp (Argument lhs, Argument rhs, const string & iden) {
	Argument tlhs = lhs, trhs = rhs;
	if (lhs.instr->getType ()->getDereferenceType ()) {
		tlhs = createGet (lhs);
	}
	if (rhs.instr->getType ()->getDereferenceType ()) {
		trhs = createGet (rhs);
	}
	auto * t = promoteArithmatic (tlhs.instr->getType (), trhs.instr->getType ());
	auto * b = resolveType ("bool");
	Instruction * inst = new T (b, staticCast (tlhs, t), staticCast (trhs, t), module->newSymbol (iden));
	return insertionPoint->give (inst);
}

const Instruction * Scope::createCmpEQ (Argument lhs, Argument rhs) {
	return createCmp <Equal> (lhs, rhs, "equal");
}

const Instruction * Scope::createCmpGT (Argument lhs, Argument rhs) {
	return createCmp <Less> (lhs, rhs, "greater");
}

const Instruction * Scope::createCmpGTE (Argument lhs, Argument rhs) {
	return createCmp <LessEqual> (lhs, rhs, "greaterequal");
}

const Instruction * Scope::createCmpLT (Argument lhs, Argument rhs) {
	return createCmp <Less> (lhs, rhs, "less");
}

const Instruction * Scope::createCmpLTE (Argument lhs, Argument rhs) {
	return createCmp <LessEqual> (lhs, rhs, "lessequal");
}

const Instruction * Scope::createCmpNE (Argument lhs, Argument rhs) {
	return createCmp <NEqual> (lhs, rhs, "nequal");
}

const Instruction * Scope::createConstant (const Type * type, const string & val, const string & iden) {
	
	//TODO: Ensure the type is sensible
	BasicArray <Value> values (1);
	assert (type);
	errno = 0;
	int err = 0;
	char * end=nullptr;
	switch (type->getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			assert (false);
		case Type::FPINT: {
			double d = std::strtod (val.c_str (), &end);
			err = errno;
			values [0] = Value (d);
			break;
		}
		case Type::INT: {
			int64_t i = std::strtoll (val.c_str (), &end, 0);
			err = errno;
			values [0] = Value (i);
			break;
		}
		case Type::UINT: {
			uint64_t u = std::strtoull (val.c_str (), &end, 0);
			err = errno;
			values [0] = Value (u);
			break;
		}
	}
	
	if (err == ERANGE) {
		
		report::logMessage (report::WARNING, "'" + val + "' is out of range, actual value will be capped at the maximum");
		
	} else if (*end) {
		report::logMessage (report::ERROR, "'" + val + "' is not a valid number");
	}
	
	Constant * inst = new Constant ({type}, values, {module->newSymbol (iden)});
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::createDiv (Argument lhs, Argument rhs) {
	return createBinary <Div> (lhs, rhs, "div");
}

Argument Scope::createGet (Argument src) {
	auto * instr = new Get (src.instr->getType ()->getDereferenceType (), src, module->newSymbol (src.instr->printIden ()));
	return {insertionPoint->give (instr), nullptr};
	
}

const Instruction * Scope::createJump (symbol block) {return createJump ({nullptr, nullptr}, block);}

const Instruction * Scope::createJump (Argument cond, symbol block) {
	
	if (cond.instr and cond.instr->getType ()->getDereferenceType ()) {
		cond = createGet (cond);
	}
	
	const Block * b = blocks [block];
	auto * instr = new Jump (cond, b);
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createImportHandle (const Scope * scope, const string & iden) {
	Type * type = new ImportHandle (scope);
	Instruction * inst = new Constant ({type}, {}, {module->newSymbol (iden)});
	return variables [iden] = inst;
}

const Instruction * Scope::createLNot (Argument arg) {
	return insertionPoint->give (new BitNot (resolveType ("bool"), arg, module->newSymbol ("bitnot")));
}

const Instruction * Scope::createMul (Argument lhs, Argument rhs) {
	return createBinary <Mul> (lhs, rhs, "mul");
}

const Instruction * Scope::createNeg (Argument arg) {
	
	Instruction * instr = new Neg (arg, module->newSymbol ("neg"));
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createParameter (const Type * const type, Argument init, const string & iden) {
	
	Parameter * param=nullptr;
	if (init.instr) {
		auto t = staticCast (init, type);
		if (t.instr) {
			param = new Parameter (t, iden);
		}
		
	} else {
		param = new Parameter (type, iden);
	}
	return param;
	
}

const Instruction * Scope::createPointerAdd (const Type * const type, Argument ptr, Argument offset, const std::string & iden) {
	
	auto * add = new PtrAdd (type, ptr, offset, module->newSymbol (iden));
	return insertionPoint->give (add);
	
}

const Instruction * Scope::createPut (Argument src, Argument dest) {
	
	auto * instr = new Put (dest.instr->getType ()->getDereferenceType (), src, dest, module->newSymbol ("put"));
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createReinterpret (Argument ptr, const Type * type, const string & iden) {
	return insertionPoint->give (new Reinterpret (type, ptr, module->newSymbol (iden)));
}

const Instruction * Scope::createRem (Argument lhs, Argument rhs) {
	return createBinary <Rem> (lhs, rhs, "rem");
}

const Instruction * Scope::createStructValue (const Type * const type, BasicArray <Argument> vals, const string & iden) {
	
	const Instruction * inst = new Composite (type, {vals.begin (), vals.end ()}, module->newSymbol (iden));
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::createSub (Argument lhs, Argument rhs) {
	if (typeid (*lhs.instr) == typeid (Constant) and typeid (*rhs.instr) == typeid (Constant) and lhs.instr->getType ()->getArithmaticType () == Type::UINT) {
		Instruction * inst = new Sub (resolveType ("int64"), lhs, rhs, module->newSymbol ("sub"));
		return insertionPoint->give (inst);
	} else {
		return createBinary <Sub> (lhs, rhs, "sub");
	}
}

const Instruction * Scope::getFunctionPointer () {
	
	auto itt = variables.find ("__entry__");
	if (itt != variables.end ()) {
		return itt->second;
	}
	if (!owner) {
		assert (false);
		return nullptr;
	}
	return variables ["__entry__"] = new FunctionPointer (owner);
	
}

const Instruction * Scope::resolve (Argument parent, const string & iden) {
	
	const Type * type = parent.instr->getType ();
	if (typeid (*type) == typeid (Struct)) {
		
		const Struct * str = static_cast <const Struct *> (type);
		const auto & memberArr = str->getMemberArr ();
		
		for (size_t i=0; i<memberArr.size (); ++i) {
			const Parameter * param = memberArr [i];
			if (*param->getIden () == iden) {
				return insertionPoint->give (new PickStructMember (param->getType (), parent, i, iden, module->newSymbol (iden)));
			}
		}
		
	} else if (typeid (*type) == typeid (ImportHandle)) {
		
		const ImportHandle * imp = static_cast <const ImportHandle *> (type);
		const Scope * scope = imp->getScope ();
		return scope->resolve (iden, insertionPoint);
		
	} else if (typeid (*type) == typeid (PointerType)) {
		
		const Parameter * param = type->getDereferenceType ()->getParam (iden);
		if (not param) return nullptr;
		auto * add = new PtrAdd (param->getType ()->getPointerTo (), parent, param, module->newSymbol (iden));
		return insertionPoint->give (add);
		
	}
	
	report::logMessage (report::ERROR, "Can't resolve '" + iden + "'");
	return nullptr;
	
}

const Instruction * Scope::resolve (const string & iden) {
	return resolve (iden, insertionPoint);
}

const Instruction * Scope::resolve (const string & iden, Block * insertionPoint) const {
	
	auto itt = variables.find (iden);
	if (itt != variables.end ()) {
		return itt->second;
	}
	
	auto fitt = functions.find (iden);
	if (fitt != functions.end ()) {
		if (fitt->second->size () == 1) {
			return insertionPoint->give (new FunctionPointer (fitt->second->front ()));
		}
	}
	
	if (owner) {
		for (const Parameter * param : owner->getArgs ()) {
			if (iden == *param->getIden ()) {
				return param;
			}
		}
	}
	
	if (parent) return parent->resolve (iden);
	
	report::logMessage (report::ERROR, "Could not resolve '" + iden + "'");
	return nullptr;
	
}

const Instruction * Scope::staticCast (const Instruction * src, const Type * const target, const string & iden) {
	
	if (src->getType () == target) return src;
	
	if (typeid (*src->getType ()) == typeid (Number) and typeid (*target) == typeid (Number)) {
		
		//Cast is trivial, the assembler can handle it
		return src;
		
	} else {
		
		//TODO: complex casting
		report::logMessage (report::ERROR, "Non trivial casting is not yet supported");
		return nullptr;
		
	}
	
}

Argument Scope::staticCast (Argument src, const Type * const target, const string & iden) {
	
	const Instruction * instr = staticCast (src.instr, target, iden);
	if (instr == src.instr) return src;
	return {instr, nullptr};
	
}


Struct * Scope::registerStruct (const string & iden) {
	
	Struct * s = nullptr;
	if (iden != "") {
		if (module->data->types.find (module->findSymbol (iden)) == module->data->types.end ()) {
			Type *& t = types [iden];
			if (!t) {
				t = s = new Struct;
			} else {
				report::logMessage (report::ERROR, "Unable to override '" + iden + "'");
			}
		} else {
			report::logMessage (report::ERROR, "'" + iden + "' is a reserved type");
		}
	} else {
		report::logMessage (report::ERROR, "Structs must have a name");
	}
	return s;
	
}


size_t Scope::changeActiveBlock (symbol block) {
	
	insertionPoint = blocks [block];
	assert (insertionPoint);
	return 0;
	
}

symbol Scope::createBlock (const string & iden) {
	
	symbol s = module->newSymbol (iden);
	Block * b = new Block (s);
	owner->pushBlock (b);
	blocks [s] = b;
	return s;
	
}

};
