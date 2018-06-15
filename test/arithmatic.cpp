#include "interpretertest.hpp"

TEST_F (InterpreterTest, integer) {
	
	EXPECT_EQ (interpret ("1"), "1");
	
}

TEST_F (InterpreterTest, integer_addition) {
	
	EXPECT_EQ (interpret ("1 + 1"), "2");
	
}
