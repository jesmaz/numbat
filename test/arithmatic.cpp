#include "interpretertest.hpp"

TEST_F (InterpreterTest, integer) {
	
	interpret ("1", "1");
	interpret ("01", "1");
	interpret ("10", "10");
	interpret ("56", "56");
	interpret ("0000000000012345", "12345");
	
}

TEST_F (InterpreterTest, integer_addition) {
	
	interpret ("1 + 1", "2");
	interpret ("1 + 1", "2");
	interpret ("13 + 8", "21");
	interpret ("1398231 + 3890278", "5288509");
	
}

TEST_F (InterpreterTest, integer_bodmas) {
	
	interpret ("1 + 1 - 2", "0");
	interpret ("1 + 1 * 2", "3");
	interpret ("2 * 1 + 1", "3");
	interpret ("2 * (1 + 1)", "4");
	interpret ("2 * (1 + 1) / 2", "2");
	interpret ("2 * (1 + 4 / 2)", "6");
	
}

TEST_F (InterpreterTest, integer_subtraction) {
	
	interpret ("1 - 1", "0");
	interpret ("2 - 1", "1");
	interpret ("2 - 0", "2");
	interpret ("0 - 2", "-2");
	interpret ("4 - 2 - 3", "-1");
	
}
