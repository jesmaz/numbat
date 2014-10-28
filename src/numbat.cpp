#include "../include/numbat.hpp"

namespace numbat {
using namespace llvm;


void Numbat::loadFromFile (const std::string & file) {
	loadFromModule (parser::Module::createFromFile (file));
}

void Numbat::loadFromModule (const shared_ptr <parser::Module> & mod) {
	FunctionPassManager fpm (module);
	fpm.add (new DataLayoutPass(*engine->getDataLayout()));
	fpm.add (createBasicAliasAnalysisPass ());
	fpm.add (createReassociatePass ());
	fpm.add (createGVNPass ());
	fpm.add (createCFGSimplificationPass ());
	fpm.add (createInstructionCombiningPass ());
	fpm.doInitialization ();
	parser::BodyGenerator generator (module, engine->getDataLayout());
	generator.visit (mod);
	PassManager mpm;
	mpm.add (new DataLayoutPass(*engine->getDataLayout()));
	mpm.add (createFunctionInliningPass ());
	mpm.run (*module);
	for (Function & func : module->getFunctionList ()) {
		//Commented out due to bug
		//fpm.run (func);
	}
}

Numbat::Numbat() : module (new Module ("main", getGlobalContext ())) {
	InitializeNativeTarget ();
	std::string errStr;
	engine = EngineBuilder (module).setErrorStr (&errStr).create ();
	if (!engine) {
		std::cerr << errStr << '\n';
	}
}

/*Numbat::Numbat (const Numbat & other) {

}

Numbat::~Numbat() {

}

Numbat & Numbat::operator= (const Numbat & other) {
	return *this;
}

bool Numbat::operator== (const Numbat & other) const {
///TODO: return ...;
}*/


};