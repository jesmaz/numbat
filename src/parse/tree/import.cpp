#include "../../../include/parse/tree/import.hpp"
#include "../../../include/module.hpp"
#include "../../../include/ast/astmodule.hpp"
#include <nir/scope.hpp>

#define PATH_JOIN "/"

const nir::Instruction * ParseTreeImport::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	if (not sourceFile) declare (scope);
	if (iden) {
		return scope->createImportHandle (sourceFile->getScope (), iden->getIden ());
	} else {
		return scope->createImportHandle (sourceFile->getScope ());
	}
	
}

string joinPath (const std::vector <PTNode> & path) {
	string relPath = path.front ()->getIden ();
	for (size_t i=1; i<path.size (); ++i) {
		relPath += PATH_JOIN + path [i]->getIden ();
	}
	return relPath;
}

numbat::parser::ASTnode ParseTreeImport::build (numbat::parser::NumbatScope * scope) {
	if (!var) declare (scope);
	return numbat::parser::ASTnode (new numbat::parser::ASTvariable (getLine (), var));
}

string ParseTreeImport::strDump (text::PrintMode mode) {
	return "import " + path->toString (mode) + (iden ? " as " + iden->toString (mode) : "");
}

string ParseTreeImportPath::strDump (text::PrintMode mode) {
	return "'" + joinPath (path) + "'";
}

void ParseTreeImport::declare (nir::Scope * scope) {
	
	using namespace numbat::parser;
	
	auto & args = path->getArgs ();
	string relPath = joinPath (args);
	
	const numbat::File * parent = scope->getSourceFile ();
	if (parent) {
		sourceFile = numbat::File::import (parent->getDirectory (), relPath, scope->getModule ());
	} else {
		sourceFile = numbat::File::import (relPath, scope->getModule ());
	}
	
	if (not sourceFile) {
		std::cerr << "Failed to import '" << relPath << "'" << std::endl;
		abort ();
	}
	
}

void ParseTreeImport::declare (numbat::parser::NumbatScope * scope) {
	
	using namespace numbat::parser;
	
	auto & args = path->getArgs ();
	string relPath = joinPath (args);
	
	auto module = Module::import (getContext (scope)->path, relPath);
	auto type = ASTnode (new ASTmodule (module));
	
	string modIden = iden ? iden->getIden () : args.back ()->getIden ();
	
	var = createVariable (scope, type, nullptr, modIden, false, false);
	
}