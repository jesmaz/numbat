#include <cassert>
#include <nir/function.hpp>
#include <nir/module.hpp>
#include <nir/scope.hpp>
#include <nir/type/number.hpp>

namespace nir {


bool Module::validate () const {
	
	bool valid = true;
	for (auto func : data->functions) {
		valid &= func.second->validate ();
	}
	
	for (auto type : data->types) {
		valid &= type.second->validate ();
	}
	
	return valid;
	
}

const Block * Module::getEntry () const {
	return data->globalScope->getOwner ()->getEntryPoint ();
}

void Module::registerPrimitive (Type::ArithmaticType arith, uint32_t width, const string & name) {
	
	if (symbol s = newSymbol (name, false)) {
		data->types [s] = Number::getNumberType (arith, width);
	} else {
		abort ();
	}
	
}

Scope * Module::createRootScope () {
	
	Scope * global = getGlobalScope ();
	symbol sym = newSymbol ("__entry__");
	assert (sym);
	Function * func = new Function (*sym, LINKAGE::LOCAL);
	data->functions [sym] = func;
	global->createCall (func, {});
	Scope * scope = new Scope (this, func->getEntryPoint (), func);
	data->scopes.insert (scope);
	return scope;
	
}

Scope * Module::getGlobalScope () {
	
	if (not data->globalScope) {
		symbol sym = newSymbol ("__entry__", true);
		assert (sym);
		Function * func = new Function (*sym, LINKAGE::LOCAL);
		data->functions [sym] = func;
		data->globalScope = new Scope (this, func->getEntryPoint (), func);
	}
	return data->globalScope;
	
}

symbol Module::findSymbol (const string & iden) {
	auto & sym = data->symbols;
	auto itt = sym.find (&iden);
	if (itt != sym.end ()) {
		return *itt;
	}
	return nullptr;
}

symbol Module::newSymbol (const string & iden, bool force) {
	
	auto & sym = data->symbols;
	if (iden != "") {
		auto itt = sym.find (&iden);
		if (itt == sym.end ()) {
			symbol s = new string (iden);
			sym.insert (s);
			return s;
		}
	}
	if (not force) return nullptr;
	symbol s = new string (iden + "%" + std::to_string (data->n));
	data->n += 1;
	sym.insert (s);
	return s;
	
}

void Module::build (codegen::Target * target) {
	
	//TODO: pass target global variables
	for (auto & func : data->functions) {
		target->visit (func.second);
	}
	
}

Module::Module () : data (new Module::Data) {
	registerPrimitive (nir::Type::UINT, 1, "bool");
	
	registerPrimitive (nir::Type::UINT, 8, "uint8");
	registerPrimitive (nir::Type::UINT, 16, "uint16");
	registerPrimitive (nir::Type::UINT, 32, "uint32");
	registerPrimitive (nir::Type::UINT, 64, "uint64");
	
	registerPrimitive (nir::Type::INT, 8, "int8");
	registerPrimitive (nir::Type::INT, 16, "int16");
	registerPrimitive (nir::Type::INT, 32, "int32");
	registerPrimitive (nir::Type::INT, 64, "int64");
	
	registerPrimitive (nir::Type::FPINT, 16, "half");
	registerPrimitive (nir::Type::FPINT, 32, "float");
	registerPrimitive (nir::Type::FPINT, 64, "double");
	registerPrimitive (nir::Type::FPINT, 128, "quad");
}

Module::Data::~Data () {
	for (symbol s : symbols) {
		delete s;
	}
	delete globalScope;
	for (const Scope * s : scopes) {
		delete s;
	}
}

};
