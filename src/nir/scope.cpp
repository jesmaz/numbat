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
#include "../../include/nir/inst/mul.hpp"
#include "../../include/nir/inst/put.hpp"
#include "../../include/nir/inst/sub.hpp"
#include "../../include/nir/type/number.hpp"

#include <iostream>
#include <map>


namespace nir {


Scope * Scope::declareFunction (const std::vector <const Parameter *> params, const std::vector <const Parameter *> ret, const string iden) {
	
	Function * func = new Function (params, ret);
	
	string label = "nbt_" + iden;// + "_-";
	//for (Type * p : params) label += p->getIden () + "-";
	//label += "_-";
	//for (Type * r : ret) label += r->getIden () + "-";
	label += "_" + std::to_string (params.size ());
	label += "_" + std::to_string (ret.size ());
	
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
			}
		case Type::INT:
			switch (rhs->getArithmaticType ()) {
				case Type::DECINT:
				case Type::FPINT:
					return rhs;
				case Type::INT:
				case Type::UINT:
					return lhs;
			}
		case Type::UINT:
			return rhs;
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
	const Instruction * amount = createConstant (ctype, "1");
	Instruction * inst = new Alloc (type->getPointerTo (), amount, module->newSymbol (iden));
	return variables [iden] = insertionPoint->give (inst);
	
}

template <typename T>
const Instruction * Scope::createBinary (const Instruction * lhs, const Instruction * rhs, const string & iden) {
	std::cerr << lhs->toString () << std::endl;
	std::cerr << rhs->toString () << std::endl;
	const Instruction * tlhs = lhs, * trhs = rhs;
	if (lhs->getType ()->getDereferenceType ()) {
		tlhs = createGet (lhs);
	}
	if (rhs->getType ()->getDereferenceType ()) {
		trhs = createGet (rhs);
	}
	auto * t = promoteArithmatic (tlhs->getType (), trhs->getType ());
	Instruction * inst = new T (t, staticCast (tlhs, t), staticCast (trhs, t), module->newSymbol (iden));
	return insertionPoint->give (inst);
}

const Instruction * Scope::createAdd (const std::vector <const Instruction *> & args) {
	return createBinary <Add> (args [0], args [1], "add");
}

const Instruction * Scope::createAssign (const std::vector <const Instruction *> & args) {
	//TODO: assert arg [0] is pointer
	const Instruction * tsrc = args [1];
	if (args [1]->getType ()->getDereferenceType ()) {
		tsrc = createGet (args [1]);
	}
	tsrc = staticCast (tsrc, args [0]->getType ()->getDereferenceType ());
	return createPut (args [1], args [0]);
}

const Instruction * Scope::createCall (const Function * func, const std::vector <const Instruction *> & args) {
	
	//TODO: appropriate casting
	std::vector <symbol> idens (func->getRet ().size (), nullptr);
	for (symbol & s : idens) {
		s = module->newSymbol ("");
	}
	Instruction * inst = new DirectCall (func, args, idens);
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::createConstant (const Type * type, const string & val, const string & iden) {
	
	//TODO: Ensure the type is sensible
	Instruction * inst = new Constant (type, val);
	assert (type);
	return insertionPoint->give (inst);
	
}

const Instruction * Scope::createDiv (const std::vector <const Instruction *> & args) {
	return createBinary <Div> (args [0], args [1], "div");
}

const Instruction * Scope::createGet (const Instruction * src) {
	std::cerr << src->toString () << std::endl;
	std::cerr << src->getType ()->toString () << std::endl;
	std::cerr << src->getType ()->getDereferenceType ()->toString () << std::endl;
	std::cerr << src->getType ()->getPointerTo ()->toString () << std::endl;
	auto * instr = new Get (src->getType ()->getDereferenceType (), src, module->newSymbol (src->printIden ()));
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createMul (const std::vector <const Instruction *> & args) {
	return createBinary <Mul> (args [0], args [1], "mul");
}

const Instruction * Scope::createParameter (const Type * const type, const Instruction * init, const string & iden) {
	
	Parameter * param;
	if (init) {
		param = new Parameter (staticCast (init, type), iden);
	} else {
		param = new Parameter (type, iden);
	}
	return param;
	
}

const Instruction * Scope::createPut (const Instruction * src, const Instruction * dest) {
	
	auto * instr = new Put (dest->getType ()->getDereferenceType (), src, dest);
	return insertionPoint->give (instr);
	
}

const Instruction * Scope::createSub (const std::vector <const Instruction *> & args) {
	if (typeid (*args [0]) == typeid (Constant) and typeid (*args [1]) == typeid (Constant) and args [0]->getType ()->getArithmaticType () == Type::UINT) {
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


};