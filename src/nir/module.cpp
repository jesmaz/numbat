#include "../../include/nir/function.hpp"
#include "../../include/nir/module.hpp"
#include "../../include/nir/scope.hpp"
#include "../../include/nir/type/number.hpp"

#include <cassert>

namespace nir {


void Module::registerPrimative (Type::ArithmaticType arith, uint32_t width, const string & name) {
	
	if (symbol s = newSymbol (name, false)) {
		data->types [s] = new Number (arith, width);
	} else {
		abort ();
	}
	
}

Scope * Module::createRootScope() {
	
	Scope * global = getGlobalScope ();
	symbol sym = newSymbol ("__entry__");
	assert (sym);
	Function * func = new Function;
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
		Function * func = new Function;
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

void Module::build () {
	
	//TODO: pass target global variables
	for (auto & func : data->functions) {
		data->target->visit (func.second);
	}
	
}

Module::Module (codegen::Target * target) : data (new Module::Data) {
	data->target = target;
}


};