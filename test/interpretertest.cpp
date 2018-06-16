#include <ast/passes/execute.hpp>
#include <parse/handparser.hpp>
#include <stackmachine/interpreter.hpp>
#include <typeinfo>
#include <utility/report.hpp>

#include "interpretertest.hpp"

void InterpreterTest::interpret (const string & str, const string & expected) {
	
	auto parseTree = parser::parse (str, &dummyFile);
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		FAIL ();
	}
	
	auto ast = parseTree->extendAST (context);
	delete parseTree;
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		FAIL ();
	}
	
	ast = AST::transform (ast);
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		FAIL ();
	}
	
	auto val = AST::ExecutePass () (ast);
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		report::reset ();
		FAIL ();
	}
	
	EXPECT_EQ (val->toString (text::PrintMode::PLAIN), expected) << "Input for this operation was: " << str;
	
}

InterpreterTest::InterpreterTest () : dummyFile (), context (&dummyFile) {
}
