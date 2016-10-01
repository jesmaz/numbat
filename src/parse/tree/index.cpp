#include <parse/tree/index.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/type/array.hpp>

const nir::Instruction * ParseTreeIndex::build (nir::Scope * scope) {
	
	const nir::Instruction * inst = index->build (scope);
	const nir::Instruction * s = nullptr;
	for (PTNode node : args) {
		const nir::Instruction * r = node->build (scope);
		if (s) {
			s = scope->createMul ({{s, s->getIden ()}, {r, r->getIden ()}});
		} else {
			s = r;
		}
	}
	
	if (not inst) return nullptr;
	
	nir::Argument array = {inst, inst->getIden ()};
	auto * dat = scope->resolve (array, "data");
	auto ptr = scope->createGet ({dat, dat->getIden ()});
	return scope->createPointerAdd (ptr.instr->getType (), ptr, {s, s->getIden ()}, "index");
	
}

const nir::Instruction * ParseTreeIndex::buildAllocate (nir::Scope * scope, const string & iden) {
	
	const nir::Instruction * s = nullptr;
	for (PTNode node : args) {
		const nir::Instruction * r = node->build (scope);
		if (s) {
			s = scope->createMul ({{s, s->getIden ()}, {r, r->getIden ()}});
		} else {
			s = r;
		}
	}
	
	auto * t = index->resolveType (scope);
	if (not t or not s) return nullptr;
	return scope->allocateArray (t, {s, s->getIden ()}, iden);
	
}

const nir::Type * ParseTreeIndex::resolveType (nir::Scope * scope) {
	return nir::Array::arrayOf (index->resolveType (scope));
}

string ParseTreeIndex::strDump (text::PrintMode mode) {
	string s = "(" + index->toString (mode) + (text::PrintMode::COLOUR & mode ? text::red + " [" + text::reset : " [");
	if (!args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1; i<args.size (); ++i) s += args [i]->toString (mode);
	}
	return s + (text::PrintMode::COLOUR & mode ? text::red + "]" + text::reset : "]") + ")";
}
