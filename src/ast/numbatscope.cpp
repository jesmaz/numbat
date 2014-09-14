#include "../../include/ast/functiondecleration.hpp"
#include "../../include/ast/numbatscope.hpp"

namespace numbat {
namespace parser {


ASTnode NumbatScope::resolveSymbol (const string & iden) const {
	
	auto var = variables.find (iden);
	if (var != variables.end ()) {
		return ASTnode (new ASTvariable (var->second));
	}
	auto typ = types.find (iden);
	if (typ != types.end ()) {
		return ASTnode (new ASTtype (false, false, typ->second));
	}
	//auto funcs = findFunctions (this, iden);
	auto func_beg = functions.lower_bound (iden);
	auto func_end = functions.upper_bound (iden);
	std::vector <shared_ptr <FunctionDecleration>> funcs;
	while (func_beg != func_end) {
		funcs.push_back (func_beg->second);
		++func_beg;
	}
	if (!funcs.empty ()) {
		return ASTnode (new ASTfunctionlist (iden, funcs));
	}
	
	return ASTbase::resolveSymbol (iden);
	
}

ASTnode resolveSymbol (const NumbatScope* scope, const string & iden, ASTnode parent, bool cascade, bool local) {
	
	ASTnode ret;
	if (parent) {
		ret = parent->resolveSymbol (iden);
	} else {
		ret = scope->resolveSymbol (iden);
		if (!ret->isValid () and scope->parent and cascade) {
			return resolveSymbol (scope->parent, iden, parent, cascade, local);
		}
	}
	if (!ret) {
		ret = ASTnode (new ASTerror ("'" + iden + "' is undefined within this scope."));
	}
	return ret;
	
}

bool NumbatScope::isValid () const {
	if (0 > valid) {
		valid = 1;
		for (const auto & n : body) {
			valid &= n->isValid ();
		}
		for (const auto & n : children) {
			valid &= n->isValid ();
		}
	} else {
		return 0 < valid;
	}
	bool v = 0 < valid;
	valid = -1;
	return v;
}

size_t NumbatScope::calculateWeight() const {
	return 0;
}

string NumbatScope::toString (const string & indent) const {
	string s=indent + "{\n";
	for (const ASTnode & n : body) {
		s += n->toString (indent + "    ") + "\n";
	}
	return s + "}";
}


bool NumbatScope::registerSymbol (const string& iden, FunctionDecleration * func) {
	
	auto itt = functions.lower_bound (iden);
	auto end = functions.upper_bound (iden);
	
	for (; itt != end; ++itt) {
		if (*(itt->second) == *func) {
			return false;
		}
	}
	functions.insert (std::make_pair (iden, unique_ptr <FunctionDecleration> (func)));
	return true;
	
}

bool NumbatScope::registerSymbol (const string & iden, NumbatVariable * var) {
	
	if (symbolRegisted (iden)) {
		return false;
	}
	if (!parent) {
		var->isGlobal (true);
	}
	variables [iden] = unique_ptr <NumbatVariable> (var);
	return true;
	
}

bool NumbatScope::registerSymbol (const string & iden, NumbatType * type) {
	
	if (symbolRegisted (iden)) {
		return false;
	}
	types [iden] = unique_ptr <NumbatType> (type);
	return true;
	
}

bool NumbatScope::symbolRegisted (const string & iden) {
	
	return (types.find (iden) != types.end ()) or (variables.find (iden) != variables.end ()) or (functions.find (iden) != functions.end ());
	
}

NumbatScope * createChild (NumbatScope * scope) {
	NumbatScope * child = new NumbatScope (scope->context);
	child->parent = scope; 
	scope->children.insert (unique_ptr <NumbatScope> (child));
	return child;
}

const NumbatType * getType (NumbatScope * scope, const string & iden) {
	
	auto itt = scope->types.find (iden);
	if (itt != scope->types.end ()) {
		return itt->second.get ();
	} else if (scope->parent) {
		return getType (scope->parent, iden);
	} else {
		return nullptr;
	}
	
}

FunctionDecleration * createFunctionDecleration (NumbatScope * scope, const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type, const std::set <string> metaTags) {
	
	FunctionDecleration * func = new FunctionDecleration (iden, args, type, metaTags);
	if (!scope->registerSymbol (iden, func)) {
		delete func;
		func = nullptr;
	}
	return func;
	
}

NumbatType * createRawType (NumbatScope * scope, const string & iden, size_t size, NumbatRawType::Type type) {
	
	NumbatType * nt = new NumbatRawType (iden, size, type);
	if (!scope->registerSymbol (iden, nt)) {
		delete nt;
		nt = nullptr;
	}
	return nt;
	
}

NumbatVariable * NumbatScope::createVariable (const ASTnode & type, const ASTnode & init, const string & iden, bool global, bool temp) {
	
	NumbatVariable * var = new NumbatVariable (type, init, iden, global, temp);
	if (!registerSymbol (iden, var)) {
		delete var;
		var = nullptr;
	}
	return var;
	
}


}
}