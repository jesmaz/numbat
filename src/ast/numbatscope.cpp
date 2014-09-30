#include "../../include/ast.hpp"
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
		ret = ASTbase::resolveSymbol (iden, parent);
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

bool NumbatScope::isParsed () const {
	if (0 > valid) {
		valid = 1;
		for (const auto & n : body) {
			valid &= n->isParsed ();
		}
		for (const auto & n : children) {
			valid &= n->isParsed ();
		}
	} else {
		return 0 < valid;
	}
	bool v = 0 < valid;
	valid = -1;
	return v;
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
	for (auto type: types) {
		s += indent + "    " + type.second->toString () + "\n";
	}
	for (auto func : functions){
		s += indent + "    " + func.first + " " + func.second->toString ("    " + indent) + "\n";
	}
	for (const ASTnode & n : body) {
		s += n->toString (indent + "    ") + "\n";
	}
	return s + indent +  "}";
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
	
	const NumbatType * type = scope->getType (iden);
	if (!type and scope->parent) {
		type = getType (scope->parent, iden);
	}
	return type;
	
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
	
	NumbatType * nt = new NumbatRawType (iden, size, type, scope->getAST ()->MallocFunc (), scope->getAST ()->FreeFunc ());
	if (!scope->registerSymbol (iden, nt)) {
		delete nt;
		nt = nullptr;
	}
	return nt;
	
}

NumbatType * createStruct (NumbatScope * scope, const string & iden, const std::set< string > & meta) {
	
	NumbatType * nt = new NumbatType (iden, meta, scope->getAST ()->MallocFunc (), scope->getAST ()->FreeFunc ());
	if (!scope->registerSymbol (iden, nt)) {
		delete nt;
		nt = nullptr;
	}
	return nt;
	
}

NumbatType * getArrayType (NumbatScope * scope, const NumbatType * type, size_t dimentions) {
	NumbatPointerType * ptrType;
	while (type->arrayType.size () < dimentions) {
		type->arrayType.push_back (nullptr);
	}
	if ((ptrType = type->arrayType [dimentions - 1])) {
		return ptrType;
	}
	ASTnode rawDataType = ASTnode (new ASTtype (false, false, type));
	ptrType = type->arrayType [dimentions - 1] = new NumbatPointerType (type->getIden () + "[]", rawDataType, scope->getAST ()->MallocFunc (), scope->getAST ()->FreeFunc ());
	std::vector <ASTnode> params (dimentions, nullptr);
	ASTnode indexType (ASTnode (new ASTtype (false, false, getType (scope, "uint64"))));
	for (size_t i=0; i<dimentions-1; ++i) {
		//strig iden = 
		std::ostringstream ss;
		ss << dimentions;
		params [i] = ASTnode (new ASTmember (ss.str (), indexType));
	}
	params [dimentions-1] = ASTnode (new ASTmember ("length", indexType));
	ptrType->buildData (params);
	return ptrType;
}

const NumbatType * NumbatScope::getType (const string & iden) {
	
	auto itt = types.find (iden);
	if (itt != types.end ()) {
		return itt->second.get ();
	} else {
		return nullptr;
	}
	
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