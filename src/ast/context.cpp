#include <ast/context.hpp>
#include <ast/function.hpp>
#include <ast/type.hpp>
#include <iostream>
#include <utility/report.hpp>


namespace AST {


NodePtr Context::resolve (const string & str) const {
	
	auto itt = variables->find (str);
	if (itt != variables->end ()) {
		return itt->second->var;
	}
	std::cerr << str << " not found " << std::hex << (void*)this << std::endl;
	return nullptr;
	
}

TypePtr Context::resolveType (const string & str) const {
	
	auto itt = types->find (str);
	if (itt != types->end ()) {
		return itt->second->type;
	}
	return nullptr;
	
}


void Context::func (const string & str, FuncPtr func) {
	
	std::cerr << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ " << str << std::endl;
	
	if (not ownFunc) {
		functions = std::make_shared <std::map <string, std::shared_ptr <function_t>>> (*functions);
		ownFunc = true;
	}
	
	if (not ownVar) {
		variables = std::make_shared <std::map <string, std::shared_ptr <variable_t>>> (*variables);
		ownVar = true;
	}
	
	auto ptr = (*functions) [str];
	if (not ptr) {
		ptr = std::make_shared <function_t> (function_t {{}, this});
		
	} else if (ptr->owner != this) {
		ptr = std::make_shared <function_t> (function_t {ptr->funcs, this});
		
	}
	
	auto var = variables->find (str);
	if (var == variables->end () or var->second->owner != this) {
		auto f = std::make_shared <variable_t> (variable_t {
			std::make_shared <AST::Function_Set> (func->getPos (), ptr),
			this
		});
		this->var (str, std::make_shared <AST::Function_Set> (func->getPos (), ptr));
		std::cerr << "RRRRRRRRRRRRRR " << str << " " << std::hex << (void*)this << std::endl;
		
	}
	
	auto & map = ptr->funcs;
	auto itt = map.find (func);
	if (itt == map.end ()) {
		map [func] = this;
		
	} else if (itt->second == this) {
		report::logMessage (
			report::ERROR,
			sourceFile,
			func->getPos (),
			"Identical definitions of '" + str + "'. First defined here: "//TODO: print old position
		);
		
	} else {
		map.erase (func);
		map [func] = this;
		
	}
	
	if (not func->getParams ().empty ()) {
		func->getParams () [0]->overloadFunc (str, func);
	}
	
}

void Context::import (const string & str, Context * ctx) {
	
	if (not ownImp) {
		imports = std::make_shared <std::map <string, Context *>> (*imports);
		ownImp = true;
	}
	
}

void Context::type (const string & str, TypePtr t) {
	
	if (not ownType) {
		types = std::make_shared <std::map <string, std::shared_ptr <type_t>>> (*types);
		ownType = true;
	}
	
}

void Context::var (const string & str, NodePtr v) {
	
	if (not ownVar) {
		variables = std::make_shared <std::map <string, std::shared_ptr <variable_t>>> (*variables);
		ownVar = true;
	}
	
	auto itt = variables->find (str);
	if (itt == variables->end () or itt->second->owner != this) {
		std::cerr << "Registered '" << str << "'" << std::endl;
		(*variables) [str] = std::make_shared <variable_t> (variable_t {v, this});
		
	} else {
		report::logMessage (
			report::ERROR,
			sourceFile,
			v->getPos (),
			"Duplicate variable '" + str + "'. First defined here: "//TODO: print old position
		);
		std::cerr << ("Duplicate variable '" + str + "'. First defined here: ") << std::endl;
		
	}
	
}

Context::Context (const numbat::File * sourceFile) : imports (new std::map <string, Context *> ()),
functions (new std::map <string, std::shared_ptr <function_t>> ()),
types (new std::map <string, std::shared_ptr <type_t>> ()),
variables (new std::map <string, std::shared_ptr <variable_t>> ()),
sourceFile (sourceFile),
ownImp (true),
ownFunc (true),
ownType (true),
ownVar (true),
rootContex (this) {
	
	type ("bool", Numeric::get (Numeric::ArithmaticType::UINT, 1));
	
	type ("int8", Numeric::get (Numeric::ArithmaticType::INT, 8));
	type ("int16", Numeric::get (Numeric::ArithmaticType::INT, 16));
	type ("int32", Numeric::get (Numeric::ArithmaticType::INT, 32));
	type ("int64", Numeric::get (Numeric::ArithmaticType::INT, 64));
	
	type ("uint8", Numeric::get (Numeric::ArithmaticType::UINT, 8));
	type ("uint16", Numeric::get (Numeric::ArithmaticType::UINT, 16));
	type ("uint32", Numeric::get (Numeric::ArithmaticType::UINT, 32));
	type ("uint64", Numeric::get (Numeric::ArithmaticType::UINT, 64));
	
	type ("half", Numeric::get (Numeric::ArithmaticType::FPINT, 16));
	type ("float", Numeric::get (Numeric::ArithmaticType::FPINT, 32));
	type ("double", Numeric::get (Numeric::ArithmaticType::FPINT, 64));
	type ("quad", Numeric::get (Numeric::ArithmaticType::FPINT, 128));
	
	type ("intptr", Numeric::get (Numeric::ArithmaticType::INT, 0));
	type ("uintptr", Numeric::get (Numeric::ArithmaticType::UINT, 0));
	
}

string Function_Set::toString (text::PrintMode mode) const {
	
	return getSet ().funcs.begin ()->first->getIden ();
	
}

}
