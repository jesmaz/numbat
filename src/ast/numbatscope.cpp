#include "../../include/ast/functiondecleration.hpp"
#include "../../include/ast/numbatscope.hpp"

namespace numbat {
namespace parser {


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
	return "NYI";
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
	NumbatScope * child = new NumbatScope;
	child->parent = scope; 
	scope->children.insert (unique_ptr <NumbatScope> (child));
	return child;
}


}
}