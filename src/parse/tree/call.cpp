#include <ast/call.hpp>
#include <nir/inst/functionPointer.hpp>
#include <nir/scope.hpp>
#include <parse/tree/call.hpp>
#include <utility/array.hpp>
#include <utility/report.hpp>


namespace parser {


auto buildArgs = [](const BasicArray <PTNode> & args, nir::Scope * scope) {
	DynArray <nir::Argument> conv;
	for (auto & arg : args) {
		assert (arg);
		const nir::Instruction * instr = arg->build (scope);
		if (instr) {
			for (auto & r : instr->getResults ()) {
				if (r.iden) conv.push_back ({instr, r.iden});
			}
		}
	}
	return conv;
};


AST::NodePtr ParseTreeCall::createAST (AST::Context & ctx) {
	
	auto callee = iden->createAST (ctx);
	auto params = args.map <AST::NodePtr> ([&](auto & a){return a->createAST (ctx);});
	return std::make_shared <AST::Unresolved_Call> (getPos (), ctx.getSourceFile (), callee, params);
	
}


const nir::Instruction * ParseTreeCall::build (nir::Scope * scope) {
	
	const nir::Instruction * callee = iden->build (scope);
	if (not callee) return nullptr;
	auto & typeId = typeid (*callee);
	
	if (typeId == typeid (nir::FunctionPointer)) {
		const nir::FunctionPointer * fPtr = static_cast <const nir::FunctionPointer *> (callee);
		return scope->createCall (fPtr->getFunction (), buildArgs (args, scope));
	}
	
	report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Currently only calling functions is supported");
	return nullptr;
	
}

string ParseTreeCall::strDump (text::PrintMode mode) {
	string s = iden->toString (mode) + " (";
	if (not args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1, l=args.size (); i<l; ++i) {
			s += ", " + args [i]->toString (mode);
		}
	}
	return s + ")";
}


}
