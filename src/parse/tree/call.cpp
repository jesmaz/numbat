#include "../../../include/core.hpp"
#include "../../../include/nir/inst/functionPointer.hpp"
#include "../../../include/nir/scope.hpp"
#include "../../../include/parse/tree/call.hpp"

auto convArgs = [](const std::vector <PTNode> & args, numbat::parser::NumbatScope * scope){
	std::vector <numbat::parser::ASTnode> conv;
	conv.reserve (args.size ());
	for (auto & arg : args) {
		assert (arg);
		conv.push_back (arg->build (scope));
		assert (conv.back ());
	}
	return conv;
};

auto buildArgs = [](const std::vector <PTNode> & args, nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	std::vector <const nir::Instruction *> conv;
	conv.reserve (args.size ());
	for (auto & arg : args) {
		assert (arg);
		conv.push_back (arg->build (scope, mode));
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