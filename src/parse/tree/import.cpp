#include "../../../include/parse/tree/import.hpp"
#include "../../../include/module.hpp"
#include "../../../include/ast/astmodule.hpp"

#define PATH_JOIN "/"

string joinPath (const std::vector <PTNode> & path) {
	string relPath = path.front ()->getIden ();
	for (size_t i=1; i<path.size (); ++i) {
		relPath += PATH_JOIN + path [i]->getIden ();
	}
	return relPath;
}

string ParseTreeImport::strDump (text::PrintMode mode) {
	return "import " + path->toString (mode) + (iden ? " as " + iden->toString (mode) : "");
}

string ParseTreeImportPath::strDump (text::PrintMode mode) {
	return joinPath (path);
}

void ParseTreeImport::declare (numbat::parser::NumbatScope * scope) {
	
	using namespace numbat::parser;
	
	auto & args =path->getArgs ();
	string relPath = joinPath (args);
	
	auto module = Module::import (getContext (scope)->path, relPath);
	auto type = ASTnode (new ASTmodule (module));
	
	string modIden = iden ? iden->getIden () : args.back ()->getIden ();
	
	createVariable (scope, type, nullptr, modIden, false, false);
	
}