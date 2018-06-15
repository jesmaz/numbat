#include <ast/passes/execute.hpp>
#include <parse/handparser.hpp>
#include <typeinfo>
#include <utility/config.hpp>
#include <utility/report.hpp>

#include "interpretertest.hpp"

string InterpreterTest::interpret (const std::string & str) {
	
	auto parseTree = parser::parse (str, &dummyFile);
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		return "ERROR";
	}
	
	auto ast = parseTree->extendAST (context);
	delete parseTree;
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		return "ERROR";
	}
	
	ast = AST::transform (ast);
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		return "ERROR";
	}
	
	auto val = AST::ExecutePass () (ast);
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		return "ERROR";
	}
	
	return val->toString (text::PrintMode::PLAIN);
	
}

InterpreterTest::InterpreterTest () : dummyFile (), context (&dummyFile) {
	Config::mutableGlobalConfig ().const_folding = false;
	Config::mutableGlobalConfig ().prune_dead_code = false;
}
