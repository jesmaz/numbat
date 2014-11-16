#include "../include/ast.hpp"
#include "../include/core.hpp"

namespace numbat {
using namespace lexer;
namespace parser {


ASTnode AbstractSyntaxTree::resolveSymbol (const string & iden) const {
	ASTnode symb = NumbatScope::resolveSymbol (iden);
	if (!symb->isValid ()) {
		auto var = extVariables.find (iden);
		if (var != extVariables.end ()) {
			return ASTnode (new ASTvariable (0, var->second));
		}
		auto typ = extTypes.find (iden);
		if (typ != extTypes.end ()) {
			return ASTnode (new ASTtype (0, false, false, typ->second));
		}
		//auto funcs = findFunctions (this, iden);
		auto func_beg = extFunctions.lower_bound (iden);
		auto func_end = extFunctions.upper_bound (iden);
		std::vector <FunctionDecleration *> funcs;
		while (func_beg != func_end) {
			funcs.push_back (func_beg->second);
			++func_beg;
		}
		if (!funcs.empty ()) {
			return ASTnode (new ASTfunctionlist (0, iden, funcs));
		}
	}
	return symb;
}


const NumbatType * AbstractSyntaxTree::getType (const string & iden) {
	const NumbatType * type = NumbatScope::getType (iden);
	if (!type) {
		auto itt = extTypes.find (iden);
		if (itt != extTypes.end ()) {
			type = itt->second;
		}
	}
	return type;
}


void AbstractSyntaxTree::addOperator (const string & pattern, const OperatorDecleration & oppdec) {
	
	if (context.operators.find (pattern) != context.operators.end ()) {
		return;
	}
	
	shared_ptr <OperatorDecleration> opp (new OperatorDecleration (oppdec));
	
	context.operators [pattern] = opp;
	
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			context.operatorsByFirstToken.insert (std::make_pair (s.substr(0,1), opp));
			break;
		}
	}
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			context.keywords.insert(s);
		}
	}
	
}

void AbstractSyntaxTree::importModule (const shared_ptr <Module> & module, bool extention) {
	
	dependencies.insert (module);
	for (auto func : module->getAST ()->functions) {
		if (extention or func.second->hasTag ("export") or func.second->hasTag ("global")) {
			extFunctions.insert (std::make_pair (func.first, func.second.get ()));
		}
	}
	for (auto type : module->getAST ()->types) {
		if (extention or type.second->hasTag ("export") or type.second->hasTag ("global")) {
			extTypes [type.first] = type.second.get ();
		}
	}
	if (context.operators.empty ()) {
		string file = context.file;
		string path = context.path;
		context = module->getAST ()->context;
		context.file = file;
		context.path = path;
	}
	
}


};
};
