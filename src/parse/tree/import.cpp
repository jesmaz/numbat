#include <nir/scope.hpp>
#include <parse/tree/import.hpp>
#include <utility/report.hpp>


namespace parser {


#define PATH_JOIN "/"

string joinPath (const BasicArray <PTNode> & path) {
	string relPath = path.front ()->getIden ();
	for (size_t i=1; i<path.size (); ++i) {
		relPath += PATH_JOIN + path [i]->getIden ();
	}
	return relPath;
}


AST::NodePtr ParseTreeImport::createAST (AST::Context & ctx) {
	
	auto & args = path->getArgs ();
	string relPath = joinPath (args);
	
	const numbat::File * parent = ctx.getSourceFile ();
	if (parent) {
		//sourceFile = numbat::File::import (parent->getDirectory (), relPath, scope->getModule ());
	} else {
		//sourceFile = numbat::File::import (relPath, scope->getModule ());
	}
	
	if (not sourceFile) {
		//report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Failed to import '" + relPath + "'");
	}
	
	//TODO: return an import handle
	return nullptr;
	
}


string ParseTreeImport::strDump (text::PrintMode mode) {
	return "import " + path->toString (mode) + (iden ? " as " + iden->toString (mode) : "");
}

string ParseTreeImportPath::strDump (text::PrintMode mode) {
	return "'" + joinPath (path) + "'";
}


}
