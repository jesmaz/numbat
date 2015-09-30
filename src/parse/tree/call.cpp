#include "../../../include/core.hpp"
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

numbat::parser::ASTnode ParseTreeCall::build (numbat::parser::NumbatScope * scope) {
	
	numbat::parser::ASTnode callee = iden->build (scope);
	
	auto & idenType = typeid (*callee.get ());
	if (idenType == typeid (numbat::parser::ASTfunctionlist)) {
		
		auto callable = numbat::parser::findBestMatch (convArgs (args, scope), static_cast <numbat::parser::ASTfunctionlist *> (callee.get ())->getElements ());
		return numbat::parser::makeFunctionCall (scope, callable);
		
	} else if (idenType == typeid (numbat::parser::ASTtype)) {
		
		auto & ctrs = static_cast <numbat::parser::ASTtype *> (callee.get ())->getType ()->getConstructors ();
		std::vector <numbat::parser::FunctionDecleration*> cands;
		cands.reserve (ctrs.size ());
		for (auto & ctr : ctrs) {
			cands.push_back (ctr.get ());
		}
		auto callable = numbat::parser::findBestMatch (convArgs (args, scope), cands);
		return numbat::parser::makeFunctionCall (scope, callable);
		
	} else if (idenType == typeid (numbat::parser::ASTvariable)) {
		
		//TODO: Implement
		assert (0);
		
	} else {
		
		//TODO: Fail gracefully
		assert (0);
		
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