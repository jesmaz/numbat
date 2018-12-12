#include <ast/context.hpp>
#include <ast/operation.hpp>
#include <ast/passes/stackmachine.hpp>
#include <ast/variable.hpp>
#include <file.hpp>
#include <memory>
#include <gtest/gtest.h>
#include <sstream>
#include <stackmachine/interpreter.hpp>
#include <stackmachine/printer.hpp>

#include "test.hpp"


TEST (stackMachine, arithmatic) {
	
	auto chunk = stackMachinePass (std::shared_ptr <AST::Basic_Operation> (new AST::Basic_Operation (
		{}, numbat::File::builtIn (), " + ", {
			AST::StaticValue::parseNumber ({}, numbat::File::builtIn (), "1"), AST::StaticValue::parseNumber ({}, numbat::File::builtIn (), "1")
		}, parser::OPERATION::ADD))
	);
	
	stackmachine::StackInterpreter interpreter;
	stackmachine::StackPrinter printer;
	std::ostringstream str;
	printer.run (chunk, std::cout);
	std::cout << std::flush;
	interpreter.run (chunk, str);
	EXPECT_EQ (interpreter.getTop (), Literal (2));
	
}

TEST (stackMachine, numericLiterals) {
	
	auto chunk = stackMachinePass (AST::StaticValue::parseNumber ({}, numbat::File::builtIn (), "1"));
	ASSERT_FALSE (chunk.data.empty ());
	ASSERT_FALSE (chunk.text.empty ());
	EXPECT_EQ (chunk.text.front ().opcode, stackmachine::OP_CODE::LOAD_GLOBAL_ADDR);
	
	stackmachine::StackInterpreter interpreter;
	std::ostringstream str;
	interpreter.run (chunk, str);
	EXPECT_EQ (interpreter.getTop (), Literal (1));
	
}

class StackMachine : public ::testing::TestWithParam <string> {};

TEST_P (StackMachine, interpreter) {
	
	auto * f = numbat::File::compile (GetParam ());
	
	auto transformed = AST::transform (f->getAST ());
	
	auto chunk = stackMachinePass (transformed);
	for (auto f : AST::Context::getAllFunctions ()) {
		chunk += stackMachinePass (f);
	}
	
	std::ostringstream ostr;
	
	stackmachine::StackPrinter printer;
	printer.run (chunk, ostr);
	stackmachine::StackInterpreter interpreter;
	interpreter.run (chunk, ostr);
	EXPECT_EQ (0, interpreter.getExitCode ());
	
}

INSTANTIATE_TEST_CASE_P (stackmachine, StackMachine, testing::ValuesIn (executeFiles.begin (), executeFiles.end ()));
