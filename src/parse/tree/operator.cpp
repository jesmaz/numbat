#include "../../../include/core.hpp"
#include "../../../include/parse/tree/operator.hpp"

std::map <string, numbat::parser::ASTnode (*) (numbat::parser::NumbatScope *, const string &, const std::vector <numbat::parser::ASTnode> &)> defImp = {
	//{"++ ", numbat::parser::defArithmetic},
	//{"-- ", numbat::parser::defArithmetic},
	{"- ", numbat::parser::defNegation},
	{"! ", numbat::parser::defNegation},
	{"not ", numbat::parser::defNegation},
	{"~ ", numbat::parser::defNegation},
	
	//{" ++", numbat::parser::defArithmetic},
	//{" --", numbat::parser::defArithmetic},
	
	{" as ", numbat::parser::defAs},
	
	{" * ", numbat::parser::defArithmetic},
	{" / ", numbat::parser::defArithmetic},
	{" % ", numbat::parser::defArithmetic},
	
	{" + ", numbat::parser::defArithmetic},
	{" - ", numbat::parser::defArithmetic},
	{" ~ ", numbat::parser::defConcat},
	
	{" << ", numbat::parser::defArithmetic},
	{" >> ", numbat::parser::defArithmetic},
	
	{" & ", numbat::parser::defArithmetic},
	
	{" ^ ", numbat::parser::defArithmetic},
	
	{" | ", numbat::parser::defArithmetic},
	
	{" < ", numbat::parser::defCompare},
	{" <= ", numbat::parser::defCompare},
	{" > ", numbat::parser::defCompare},
	{" >= ", numbat::parser::defCompare},
	
	{" == ", numbat::parser::defCompare},
	{" != ", numbat::parser::defCompare},
};

numbat::parser::ASTnode ParseTreeOperator::build (numbat::parser::NumbatScope * scope) {
	std::vector <numbat::parser::ASTnode> nodes;
	nodes.resize (args.size ());
	std::vector <numbat::parser::FunctionDecleration *> candidates;
	for (size_t i=0; i<args.size(); ++i) {
		assert (args[i]);
		nodes[i] = args[i]->build (scope);
		assert (nodes[i]);
		assert (nodes[i]->getType ());
		auto cand = nodes[i]->getType ()->getMethods (iden);
		std::copy (cand.begin (), cand.end (), std::back_inserter (candidates));
	}
	
	auto callable = numbat::parser::findBestMatch (nodes, candidates);
	assert (callable);
	if (callable->isValid ()) {
		return numbat::parser::makeFunctionCall (scope, callable);
	} else {
		assert (defImp [iden]);
		return defImp [iden] (scope, iden, nodes);
	}
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