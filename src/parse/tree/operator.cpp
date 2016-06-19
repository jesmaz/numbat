#include "../../../include/core.hpp"
#include "../../../include/nir/scope.hpp"
#include "../../../include/parse/tree/operator.hpp"


std::map <string, const nir::Instruction * (nir::Scope::*) (const std::vector <const nir::Instruction *> &)> nirDefImp = {
// 	{"- ", &nir::Scope::createNeg},
// 	{"! ", &nir::Scope::createLNot},
// 	{"not ", &nir::Scope::createLNot},
// 	{"~ ", &nir::Scope::createBitNot},
// 	
// 	{" as ", &nir::Scope::createAs},
// 	
// 	{" in ", &nir::Scope::createIn},
	
	{" * ", &nir::Scope::createMul},
	{" / ", &nir::Scope::createDiv},
// 	{" % ", &nir::Scope::createRem},
	
	{" + ", &nir::Scope::createAdd},
	{" - ", &nir::Scope::createSub},
//	{" ~ ", &nir::Scope::createConcat},
	
// 	{" << ", &nir::Scope::createShiftLeft},
// 	{" >> ", &nir::Scope::createShiftRight},
	
// 	{" & ", &nir::Scope::createBitAnd},
	
// 	{" ^ ", &nir::Scope::createBitXor},
	
// 	{" | ", &nir::Scope::createBitOr},
	
	{" < ", &nir::Scope::createCmpLT},
// 	{" <= ", &nir::Scope::createCmpLTE},
// 	{" > ", &nir::Scope::createCmpGT},
// 	{" >= ", &nir::Scope::createCmpGTE},
	
// 	{" == ", &nir::Scope::createCmpEQ},
// 	{" != ", &nir::Scope::createCmpNE},
	
// 	{" and ", &nir::Scope::createLAnd},
	
// 	{" or ", &nir::Scope::createLOr},
	
	{" = ", &nir::Scope::createAssign}
};


const nir::Instruction * ParseTreeOperator::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	std::vector <const nir::Instruction *> nodes;
	nodes.resize (args.size (), nullptr);
	//std::vector <numbat::parser::FunctionDecleration *> candidates;
	for (size_t i=0; i<args.size(); ++i) {
		assert (args[i]);
		nodes [i] = args[i]->build (scope, mode);
		assert (nodes [i]);
	}
	
	assert (nirDefImp [iden]);
	return (scope->*nirDefImp [iden]) (nodes);
	
}


string ParseTreeOperator::strDump (text::PrintMode mode) {
	if (iden.empty () and not args.empty ()) {
		string s = "(";
		for (size_t i=0; i+1<args.size (); ++i) s += args [i]->toString (mode) + " ";
		return s + args.back ()->toString (mode) + ")";
	} else if (not args.empty ()) {
		string s = "(";
		size_t i=0;
		for (char c : iden) {
			if (c == ' ') {
				if (isalnum (s.back ())) s += " ";
				assert (i < args.size ());
				s += args [i]->toString (mode) + " ";
				++i;
			} else {
				s.push_back (c);
				if (!isalnum (c)) s += " ";
			}
		}
		s.back () = ')';
		return s;
	} else {
		return "()";
	}
}