#include "../include/numbat.hpp"

namespace numbat {
using namespace llvm;

void Numbat::loadFromCode (const std::string & code) {
	lexer::tkstring tks = lexer::lexFile (code);
	loadFromTokenStr (tks);
}

void Numbat::loadFromFile (const std::string & file) {
	std::ifstream fin (file);
	if (fin.is_open ()) {
		loadFromStream (fin);
		fin.close ();
	}
}

void Numbat::loadFromModule (const shared_ptr <parser::Module> & mod) {
	FunctionPassManager fpm (module);
	fpm.add (new DataLayout(*engine->getDataLayout()));
	fpm.add (createBasicAliasAnalysisPass ());
	fpm.add (createReassociatePass ());
	fpm.add (createGVNPass ());
	fpm.add (createCFGSimplificationPass ());
	fpm.add (createInstructionCombiningPass ());
	fpm.doInitialization ();
	parser::BodyGenerator generator (module);
	generator.visit (mod);
	PassManager mpm;
	mpm.add (new DataLayout(*engine->getDataLayout()));
	mpm.add (createFunctionInliningPass ());
	mpm.run (*module);
	std::cerr << "Post Inline:\n\n\n";
	module->dump ();
	for (Function & func : module->getFunctionList ()) {
		fpm.run (func);
	}
	std::cerr << "Second optimise pass:\n\n\n";
	module->dump ();
}

void Numbat::loadFromStream (std::istream & is) {
	std::string out;
	std::string buffer;
	while (std::getline (is, buffer))
		out += buffer + "\n";
	loadFromCode (out);
}

void Numbat::loadFromTokenStr (const tkstring & tks) {
	parser::AbstractSyntaxTree ast (tks.begin (), lexer::findEOF (tks.begin (), tks.end ()));
	std::cerr << ast.toString () << std::endl;
	FunctionPassManager fpm (module);
	fpm.add (new DataLayout(*engine->getDataLayout()));
	fpm.add (createBasicAliasAnalysisPass ());
	fpm.add (createReassociatePass ());
	fpm.add (createGVNPass ());
	fpm.add (createCFGSimplificationPass ());
	fpm.add (createInstructionCombiningPass ());
	fpm.doInitialization ();
	parser::BodyGenerator generator (module);
	generator.visit (ast);
	PassManager mpm;
	mpm.add (new DataLayout(*engine->getDataLayout()));
	mpm.add (createFunctionInliningPass ());
	mpm.run (*module);
	std::cerr << "Post Inline:\n\n\n";
	module->dump ();
	for (Function & func : module->getFunctionList ()) {
		fpm.run (func);
	}
	std::cerr << "Second optimise pass:\n\n\n";
	module->dump ();
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