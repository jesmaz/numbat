#include "interpretertest.hpp"

TEST_F (InterpreterTest, integer) {
	
	EXPECT_EQ (interpret ("1"), "1");
	EXPECT_EQ (interpret ("01"), "1");
	EXPECT_EQ (interpret ("10"), "10");
	EXPECT_EQ (interpret ("56"), "56");
	EXPECT_EQ (interpret ("0000000000012345"), "12345");
	
}

TEST_F (InterpreterTest, integer_addition) {
	
	EXPECT_EQ (interpret ("1 + 1"), "2");
	EXPECT_EQ (interpret ("1 + 1"), "2");
	
	EXPECT_EQ (interpret ("13 + 8"), "21");
	
	EXPECT_EQ (interpret ("1398231 + 3890278"), "5288509");
	
}
