#include <ast/variable.hpp>
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
	sourceFile = numbat::File::import (parent->getDirectory (), relPath);
	
	if (not sourceFile) {
		report::logMessage (report::ERROR, parent, getPos (), "Failed to import '" + relPath + "'");
		return nullptr;
	}
	
	auto importType = std::make_shared <AST::Import> (getPos (), parent, sourceFile);
	auto val = std::make_shared <AST::StaticValue> (getPos (), parent, importType, Literal ());
	
	if (iden) {
		ctx.var (iden->getIden (), val);
	} else {
		ctx.var (args.back ()->getIden (), val);
	}
	
	return val;
	
}


string ParseTreeImport::strDump (text::PrintMode mode) {
	return "import " + path->toString (mode) + (iden ? " as " + iden->toString (mode) : "");
}

string ParseTreeImportPath::strDump (text::PrintMode mode) {
	return "'" + joinPath (path) + "'";
}


}
