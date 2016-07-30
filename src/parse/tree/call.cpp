#include <nir/inst/functionPointer.hpp>
#include <nir/scope.hpp>
#include <parse/tree/call.hpp>

auto buildArgs = [](const std::vector <PTNode> & args, nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	std::vector <nir::Argument> conv;
	conv.reserve (args.size ());
	for (auto & arg : args) {
		assert (arg);
		const nir::Instruction * instr = arg->build (scope, mode);
		for (nir::symbol iden : instr->getIdens ()) {
			conv.push_back ({instr, iden});
		}
	}
	return conv;
};

const nir::Instruction * ParseTreeCall::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	const nir::Instruction * callee = iden->build (scope, mode);
	auto & typeId = typeid (*callee);
	if (typeId == typeid (nir::FunctionPointer)) {
		const nir::FunctionPointer * fPtr = static_cast <const nir::FunctionPointer *> (callee);
		return scope->createCall (fPtr->getFunction (), buildArgs (args, scope, mode));
	}
	
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