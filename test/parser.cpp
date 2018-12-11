#include <gtest/gtest.h>
#include <fstream>
#include <file.hpp>
#include <iostream>
#include <parse/handparser.hpp>
#include <parse/tree.hpp>
#include <utility/config.hpp>


BasicArray <string> parseSrc = {
	"parser/arithmatic.nbt",
	"parser/arrays.nbt",
	"parser/basic.nbt",
	"parser/bracket.nbt",
	"parser/control.nbt",
	"parser/import.nbt",
	"parser/logic.nbt",
	"parser/slice.nbt"
};


class Parser : public ::testing::TestWithParam <string> {};

TEST_P (Parser, parseCorrectOutput) {
	
	numbat::File dummyFile;
	std::ifstream fin (GetParam ());
	string buff, prog;
	while (std::getline (fin, buff)) prog += buff + "\n";
	auto n = parser::parse (prog, &dummyFile);
	parser::ParseTree * ptree = dynamic_cast <parser::ParseTree *> (n);
	ASSERT_NE (ptree, nullptr);
	
	DynArray <string> output, expected;
	for (parser::PTNode n : ptree->getBody ()) {
		output.push_back (n->toString (text::COLOUR));
	}
	
	std::ifstream expectedOutput (GetParam () + ".out");
	while (std::getline (expectedOutput, buff)) expected.push_back (buff);
	
	EXPECT_EQ (output, expected);
	
	delete n;
	
}

INSTANTIATE_TEST_CASE_P (stackmachine, Parser, testing::ValuesIn (parseSrc.begin (), parseSrc.end ()));
