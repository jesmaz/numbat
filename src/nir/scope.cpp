#include "../../include/nir/block.hpp"
#include "../../include/nir/module.hpp"
#include "../../include/nir/parameter.hpp"
#include "../../include/nir/scope.hpp"
#include "../../include/nir/inst/add.hpp"
#include "../../include/nir/inst/alloc.hpp"
#include "../../include/nir/inst/call.hpp"
#include "../../include/nir/inst/constant.hpp"
#include "../../include/nir/inst/div.hpp"
#include "../../include/nir/inst/functionPointer.hpp"
#include "../../include/nir/inst/get.hpp"
#include "../../include/nir/inst/jump.hpp"
#include "../../include/nir/inst/less.hpp"
#include "../../include/nir/inst/mul.hpp"
#include "../../include/nir/inst/neg.hpp"
#include "../../include/nir/inst/put.hpp"
#include "../../include/nir/inst/ret.hpp"
#include "../../include/nir/inst/sub.hpp"
#include "../../include/nir/type/importHandle.hpp"
#include "../../include/nir/type/number.hpp"

#include <iostream>
#include <map>


namespace nir {


Scope * Scope::declareFunction (const std::vector <const Parameter *> params, const std::vector <const Parameter *> ret, const string iden) {
	
	string label = "nbt_" + iden;// + "_-";
	//for (Type * p : params) label += p->getIden () + "-";
	//label += "_-";
	//for (Type * r : ret) label += r->getIden () + "-";
	label += "_" + std::to_string (params.size ());
	label += "_" + std::to_string (ret.size ());
	
	Function * func = new Function (params, ret, label);
	
	symbol s;
	if (iden == "") {
		//Anonymous
		s = module->newSymbol (iden);
	} else {
		auto & fSet = functions [iden];
		if (!fSet) fSet = new std::vector <Function *>;
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
					std::cerr << "Unknown arithmetic type" << std::endl;
					abort ();
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
					std::cerr << "Unknown arithmetic type" << std::endl;
					abort ();
			}
		case Type::UINT:
			return rhs;
		default:
			std::cerr << "Unknown arithmetic type" << std::endl;
			abort ();
	}
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
	assert (nullptr);
	return nullptr;
	
}

const Instruction * Scope::allocateVariable (const Type * const type, const string & iden) {
	
	Type * ctype = new Number (Type::UINT, 32);
	Argument amount = {createConstant (ctype, "1"), nullptr};
	Instruction * inst = new Alloc (type->getPointerTo (), amount, module->newSymbol (iden));
	return variables [iden] = insertionPoint->give (inst);
	
}

template <typename T>
const Instruction * Scope::createBinary (Argument lhs, Argument rhs, const string & iden) {
	std::cerr << lhs.toString () << std::endl;
	std::cerr << rhs.toString () << std::endl;
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

const Instruction * Scope::createAdd (const std::vector <Argument> & args) {
	return createBinary <Add> (args [0], args [1], "add");
}

const Instruction * Scope::createAssign (const std::vector <Argument> & args) {
	//TODO: assert arg [0] is pointer
	Argument tsrc = args [1];
	if (args [1].instr->getType ()->getDereferenceType ()) {
		tsrc = createGet (args [1]);
	}
	tsrc = staticCast (tsrc, args [0].instr->getType ()->getDereferenceType ());
	return createPut (args [1], args [0]);
}

const Instruction * Scope::createAutoReturn (const Instruction * instr) {
	
	std::vector <Argument> args;
	for (symbol s : instr->getIdens ()) {
		args.push_back ({instr, s});
	}
	Instruction * ret = new Ret (args);
	return insertionPoint->give (ret);
	
}

const Instruction * Scope::createCall (const Function * func, const std::vector <Argument> & args) {
	
	//TODO: appropriate casting
	std::vector <symbol> idens (func->getRet ().size (), nullptr);
	for (symbol & s : idens) {
		s = module->newSymbol ("");
	}
	Instruction * inst = new DirectCall (func, args, idens);
	return insertionPoint->give (inst);
	
}

template <typename T>
const Instruction * Scope::createCmp (Argument lhs, Argument rhs, const string & iden) {
	std::cerr << lhs.toString () << std::endl;
	std::cerr << rhs.toString () << std::endl;
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

const Instruction * Scope::createCmpLT (const std::vector <Argument> & args) {
	return createCmp <Less> (args [0], args [1], "less");
}

const Instruction * Scope::createConstant (const Type * type, const string & val, const string & iden) {
	
	//TODO: Ensure the type is sensible
	Instruction * inst = new Constant (type, val);
	assert (type);
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::createDiv (const std::vector <Argument> & args) {
	return createBinary <Div> (args [0], args [1], "div");
}

Argument Scope::createGet (Argument src) {
	std::cerr << src.toString () << std::endl;
	std::cerr << src.instr->getType ()->toString () << std::endl;
	std::cerr << src.instr->getType ()->getDereferenceType ()->toString () << std::endl;
	std::cerr << src.instr->getType ()->getPointerTo ()->toString () << std::endl;
	auto * instr = new Get (src.instr->getType ()->getDereferenceType (), src, module->newSymbol (src.instr->printIden ()));
	return {insertionPoint->give (instr), nullptr};
	
}

const Instruction * Scope::createNeg (const std::vector <Argument> & args) {
	
	Instruction * instr = new Neg (args [0], module->newSymbol ("neg"));
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createJump (symbol block) {return createJump ({nullptr, nullptr}, block);}

const Instruction * Scope::createJump (Argument cond, symbol block) {
	
	std::cerr << "Creating Jump" << std::endl;
	const Block * b = blocks [block];
	auto * instr = new Jump (cond, b);
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createImportHandle (const Scope * scope, const string & iden) {
	Type * type = new ImportHandle (scope);
	Instruction * inst = new Constant (type, "", module->newSymbol (iden));
	return variables [iden] = inst;
}

const Instruction * Scope::createMul (const std::vector <Argument> & args) {
	return createBinary <Mul> (args [0], args [1], "mul");
}

const Instruction * Scope::createParameter (const Type * const type, Argument init, const string & iden) {
	
	Parameter * param;
	if (init.instr) {
		param = new Parameter (staticCast (init, type), iden);
	} else {
		param = new Parameter (type, iden);
	}
	return param;
	
}

const Instruction * Scope::createPut (Argument src, Argument dest) {
	
	auto * instr = new Put (dest.instr->getType ()->getDereferenceType (), src, dest, module->newSymbol ("put"));
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createSub (const std::vector <Argument> & args) {
	if (typeid (*args [0].instr) == typeid (Constant) and typeid (*args [1].instr) == typeid (Constant) and args [0].instr->getType ()->getArithmaticType () == Type::UINT) {
		Instruction * inst = new Sub (resolveType ("int64"), args [0], args [1], module->newSymbol ("sub"));
		return insertionPoint->give (inst);
	} else {
		return createBinary <Sub> (args [0], args [1], "sub");
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

const Instruction * Scope::resolve (const string & iden) {
	
	//TODO: log error
	
	auto itt = variables.find (iden);
	if (itt != variables.end ()) {
		return itt->second;
	}
	
// 	auto titt = types.find (iden);
// 	if (titt != types.end ()) {
// 		//TODO: get type id
// 	}
// 	
// 	auto fitt_beg = functions.lower_bound (iden);
// 	auto fitt_end = functions.upper_bound (iden);
// 	while (fitt_beg != fitt_end) {
// 		//TODO: get function pointers
// 	}
	
	auto fitt = functions.find (iden);
	if (fitt != functions.end ()) {
		if (fitt->second->size () == 1) {
			return insertionPoint->give (new FunctionPointer (fitt->second->front ()));
		}
	}
	
	for (auto & f : functions) {
		std::cerr << f.first;
		for (auto * fptr : *f.second) {
			std::cerr << '\t' << fptr->getType ()->toString ();
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
	std::cerr << "Could not resolve '" << iden << "'" << std::endl;
	assert (false);
	return nullptr;
	
}

const Instruction * Scope::staticCast (const Instruction * src, const Type * const target, const string & iden) {
	
	if (src->getType () == target) return src;
	
	std::cerr << src->toString () << std::endl;
	std::cerr << src->getType ()->toString () << std::endl;
	std::cerr << target->toString () << std::endl;
	if (typeid (*src->getType ()) == typeid (Number) and typeid (*target) == typeid (Number)) {
		
		//Cast is trivial, the assembler can handle it
		return src;
		
	} else {
		
		//TODO: complex casting
		assert (false);
	}
	
}

Argument Scope::staticCast (Argument src, const Type * const target, const string & iden) {
	
	const Instruction * instr = staticCast (src.instr, target, iden);
	if (instr == src.instr) return src;
	return {instr, nullptr};
	
}


size_t Scope::changeActiveBlock (symbol block) {
	
	insertionPoint = blocks [block];
	assert (insertionPoint);
	return 0;
	
}

symbol Scope::createBlock (const string & iden) {
	
	symbol s = module->newSymbol (iden);
	Block * b = new Block ();
	owner->pushBlock (b);
	blocks [s] = b;
	return s;
	
}

};