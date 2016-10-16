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
#include <nir/inst/memcpy.hpp>
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
	
	symbol s = module->newSymbol (iden);
	
	Function * func = new Function (params, ret, s, linkage);
	if (iden == "") {
		//Anonymous
	} else {
		auto & fSet = functions [iden];
		if (!fSet) fSet = new DynArray <Function *>;
		fSet->push_back (func);
	}
	if (not s) {
		assert (false);
		return nullptr;
	}
	module->data->functions [s] = func;
	Scope * scope = new Scope (module, func->getEntryPoint (), func, sourceFile);
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


const Type * Scope::resolveType (const string & iden, numbat::lexer::position pos) const {
	
	auto titt = types.find (iden);
	if (titt != types.end ()) {
		return titt->second;
	}
	auto vitt = variables.find (iden);
	if (vitt != variables.end ()) {
		return vitt->second->getType ();
	}
	if (parent) {
		return parent->resolveType (iden, pos);
	}
	auto itt = module->data->types.find (module->findSymbol (iden));
	if (itt != module->data->types.end ()) {
		return itt->second;
	}
	
	report::logMessage (report::ERROR, sourceFile, pos, "Could not resolve '" + iden + "' as a type");
	return nullptr;
	
}

const Type * Scope::resolveType (Argument parent, const string & iden, numbat::lexer::position pos) const {
	
	const Type * type = parent.type;
	if (typeid (*type) == typeid (Struct)) {
		
		report::logMessage (report::ERROR, sourceFile, pos, "Nested structs are not yet supported");
		return nullptr;
		
	} else if (typeid (*type) == typeid (ImportHandle)) {
		
		const ImportHandle * imp = static_cast <const ImportHandle *> (type);
		const Scope * scope = imp->getScope ();
		return scope->resolveType (iden, pos);
		
	}
	
	report::logMessage (report::ERROR, sourceFile, pos, "'" + type->toString (text::PLAIN) + "' doesn't support nested types");
	return nullptr;
	
}


Argument Scope::loadReference (Argument arg) {
	if (not arg.type->getDereferenceType ()) {
		report::logMessage (report::ERROR, "Reference required");
		return nullptr;
	}
	if (arg.type->getDereferenceType ()->getDereferenceType ()) {
		return createGet (arg);
	}
	return arg;
}

Argument Scope::loadValue (Argument arg) {
	if (arg.type->getDereferenceType ()) {
		arg = createGet (arg);
	} else {
		return arg;
	}
	if (arg.type->getDereferenceType ()) {
		arg = createGet (arg);
	}
	return arg;
}


const Instruction * Scope::allocateArray (const Type * const type, Argument size, const string & iden) {
	
	Array * s = Array::arrayOf (type);
	const Instruction * ptr = allocateBytes (type, size, iden + "_ptr");
	const Instruction * var = allocateVariable (s, iden);
	const Instruction * val = createStructValue (s, {{ptr, ptr->getIden ()}, size});
	createPut ({val, val->getIden ()}, {var, var->getIden ()});
	return var;
	
}

const Instruction * Scope::allocateBytes (const Type * const type, Argument size, const string & iden) {
	
	Instruction * inst = new Alloc (type->getPointerTo (), size, module->newSymbol (iden));
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::allocateVariable (const Type * const type, const string & iden) {
	
	const Type * ctype = Number::getNumberType (Type::UINT, 32);
	auto * con = createConstant (ctype, "1", {1,1});
	Argument amount = {con, con->getIden ()};
	Instruction * inst = new Alloc (type->getPointerTo (), amount, module->newSymbol (iden));
	return variables [iden] = insertionPoint->give (inst);
	
}

template <typename T>
const Instruction * Scope::createBinary (Argument lhs, Argument rhs, const string & iden) {
	Argument tlhs = loadValue (lhs), trhs = loadValue (rhs);
	
	auto * t = promoteArithmatic (tlhs.type, trhs.type);
	Instruction * inst = new T (t, staticCast (tlhs, t), staticCast (trhs, t), module->newSymbol (iden));
	return insertionPoint->give (inst);
}

const Instruction * Scope::createAdd (Argument lhs, Argument rhs) {
	return createBinary <Add> (lhs, rhs, "add");
}

const Instruction * Scope::createAssign (Argument lhs, Argument rhs) {
	const Type * dref = lhs.type->getDereferenceType ();
	if (not dref) {
		report::logMessage (report::ERROR, "Can't assign to a constant");
		return nullptr;
	}
	lhs = loadReference (lhs);
	dref = lhs.type->getDereferenceType ();
	rhs = staticCast (loadValue (rhs), dref);
	return createPut (rhs, lhs);
}

const Instruction * Scope::createAutoReturn (const Instruction * instr) {
	
	if (owner->getRet ().empty ()) {
		Instruction * ret = new Ret ({});
		return insertionPoint->give (ret);
	} else {
		auto args = instr->getResults ().map <Argument> ([=](auto & r){return Argument {instr, r.iden};});
		Instruction * ret = new Ret (args);
		return insertionPoint->give (ret);
	}
	
}

const Instruction * Scope::createBitAnd (Argument lhs, Argument rhs) {
	return createBinary <BitAnd> (lhs, rhs, "bitand");
}

const Instruction * Scope::createBitNot (Argument arg) {
	return insertionPoint->give (new BitNot (loadValue (arg), module->newSymbol ("bitnot")));
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
	Argument tlhs = loadValue (lhs), trhs = loadValue (rhs);
	
	auto * t = promoteArithmatic (tlhs.type, trhs.type);
	auto * b = resolveType ("bool", {1,1});
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

const Instruction * Scope::createConstant (const Type * type, const string & val, numbat::lexer::position pos, const string & iden) {
	
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
		
		report::logMessage (report::WARNING, sourceFile, pos, "'" + val + "' is out of range, actual value will be capped at the maximum");
		
	} else if (*end) {
		report::logMessage (report::ERROR, sourceFile, pos, "'" + val + "' is not a valid number");
	}
	
	Constant * inst = new Constant ({type}, values, {module->newSymbol (iden)});
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::createCopy (Argument src, Argument dest, Argument count) {
	
	return insertionPoint->give (new MemCpy (src, dest, count, module->newSymbol ("memcpy")));
	
}

const Instruction * Scope::createDiv (Argument lhs, Argument rhs) {
	return createBinary <Div> (lhs, rhs, "div");
}

const Instruction * Scope::createGet (Argument src) {
	auto * instr = new Get (src.type->getDereferenceType (), src, module->newSymbol (*src.sym));
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createJump (symbol block) {return createJump (Argument (), block);}

const Instruction * Scope::createJump (Argument cond, symbol block) {
	cond = loadValue (cond);
	
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
	return insertionPoint->give (new BitNot (resolveType ("bool", {1,1}), loadValue (arg), module->newSymbol ("bitnot")));
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
	if (init.sym) {
		auto t = staticCast (init, type);
		if (t.sym) {
			abort ();//param = new Parameter (t, iden);
		}
		
	} else {
		//param = new Parameter (type, iden);
	}
	abort ();//return param;
	
}

const Instruction * Scope::createPointerAdd (const Type * const type, Argument ptr, Argument offset, const std::string & iden) {
	
	auto * add = new PtrAdd (type, ptr, offset, module->newSymbol (iden));
	return insertionPoint->give (add);
	
}

const Instruction * Scope::createPut (Argument src, Argument dest) {
	
	auto * instr = new Put (dest.type->getDereferenceType (), src, dest, module->newSymbol ("put"));
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
	return createBinary <Sub> (lhs, rhs, "sub");
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

const Instruction * Scope::resolve (Argument parent, const string & iden, numbat::lexer::position pos) {
	
	const Type * type = parent.type;
	if (typeid (*type) == typeid (Struct)) {
		
		const Struct * str = static_cast <const Struct *> (type);
		const auto & memberArr = str->getMemberArr ();
		
		for (size_t i=0; i<memberArr.size (); ++i) {
			const Parameter * param = memberArr [i];
			if (param->getIden ()->iden == iden) {
				return insertionPoint->give (new PickStructMember (param->getType (), parent, i, iden, module->newSymbol (iden)));
			}
		}
		
	} else if (typeid (*type) == typeid (ImportHandle)) {
		
		const ImportHandle * imp = static_cast <const ImportHandle *> (type);
		const Scope * scope = imp->getScope ();
		return scope->resolve (iden, insertionPoint, pos);
		
	} else if (typeid (*type) == typeid (PointerType)) {
		
		const Parameter * param = type->getDereferenceType ()->getParam (iden);
		if (not param) return nullptr;
		auto * add = new PtrAdd (param->getType ()->getPointerTo (), parent, param, module->newSymbol (iden));
		return insertionPoint->give (add);
		
	}
	
	report::logMessage (report::ERROR, sourceFile, pos, "Can't resolve '" + iden + "'");
	return nullptr;
	
}

const Instruction * Scope::resolve (const string & iden, numbat::lexer::position pos) {
	return resolve (iden, insertionPoint, pos);
}

const Instruction * Scope::resolve (const string & iden, Block * insertionPoint, numbat::lexer::position pos) const {
	
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
			if (iden == param->getIden ()->iden) {
				abort ();
			}
		}
	}
	
	if (parent) return parent->resolve (iden, pos);
	
	report::logMessage (report::ERROR, sourceFile, pos, "Could not resolve '" + iden + "'");
	return nullptr;
	
}

const Instruction * Scope::staticCast (const Type * src, const Type * const target, const string & iden) {
	
	if (src == target) return nullptr;
	
	if (typeid (*src) == typeid (Number) and typeid (*target) == typeid (Number)) {
		
		//Cast is trivial, the assembler can handle it
		return nullptr;
		
	} else {
		
		//TODO: complex casting
		report::logMessage (report::ERROR, "Non trivial casting is not yet supported");
		return nullptr;
		
	}
	
}

Argument Scope::staticCast (Argument src, const Type * const target, const string & iden) {
	
	const Instruction * instr = staticCast (src.type, target, iden);
	if (not instr) return src;
	return {instr, nullptr};
	
}


Struct * Scope::registerStruct (const string & iden, numbat::lexer::position pos) {
	
	Struct * s = nullptr;
	if (iden != "") {
		if (module->data->types.find (module->findSymbol (iden)) == module->data->types.end ()) {
			Type *& t = types [iden];
			if (!t) {
				t = s = new Struct;
			} else {
				report::logMessage (report::ERROR, sourceFile, pos, "Unable to override '" + iden + "'");
			}
		} else {
			report::logMessage (report::ERROR, sourceFile, pos, "'" + iden + "' is a reserved type");
		}
	} else {
		report::logMessage (report::ERROR, sourceFile, pos, "Structs must have a name");
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
