#include <ast/operation.hpp>
#include <ast/type.hpp>
#include <parse/tree/index.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/type/array.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeIndex::createAST (AST::Context & ctx) {
	
	BasicArray <AST::NodePtr> oppArgs (args.size () + 1);
	oppArgs [0] = index->createAST (ctx);
	for (size_t i=1; i<oppArgs.size (); ++i)  {
		oppArgs [i] = args [i-1]->createAST (ctx);
	}
	return std::make_shared <AST::Unresolved_Operation> (getPos (), " [ ]", oppArgs, OPERATION::INDEX);
	
}

AST::TypePtr ParseTreeIndex::createASTtype (AST::Context & ctx) {
	return std::make_shared <AST::ArrayInit> (getPos (), index->createASTtype (ctx), args [0]->createAST (ctx), nullptr);
}

const nir::Instruction * ParseTreeIndex::build (nir::Scope * scope) {
	
	const nir::Instruction * inst = index->build (scope);
	const nir::Instruction * s = nullptr;
	for (PTNode node : args) {
		const nir::Instruction * r = node->build (scope);
		if (s) {
			s = scope->createMul ({s, s->getIden ()}, {r, r->getIden ()});
		} else {
			s = r;
		}
	}
	
	if (not inst or not s) return nullptr;
	if (inst->getResults ().empty ()) {
		report::logMessage (report::ERROR, scope->getSourceFile (), index->getPos (), "'" + inst->toString (text::PLAIN) + "' can't be indexed");
		return nullptr;
	}
	if (s->getResults ().empty ()) {
		report::logMessage (report::ERROR, scope->getSourceFile (), index->getPos (), "'" + args [0]->toString (text::PLAIN) + "' is not a value");
		return nullptr;
	}
	
	nir::Argument array = {inst, inst->getIden ()};
	auto * dat = scope->resolve (array, "data", index->getPos ());
	if (not dat) return nullptr;
	auto ptr = scope->createGet ({dat, dat->getIden ()});
	return scope->createPointerAdd (ptr->getType (), {ptr, ptr->getIden ()}, {s, s->getIden ()}, "index");
	
}

const nir::Instruction * ParseTreeIndex::buildAllocate (nir::Scope * scope, const string & iden) {
	
	const nir::Instruction * s = nullptr;
	for (PTNode node : args) {
		const nir::Instruction * r = node->build (scope);
		if (s) {
			s = scope->createMul ({s, s->getIden ()}, {r, r->getIden ()});
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


}
