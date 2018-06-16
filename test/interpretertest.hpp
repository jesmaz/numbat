#include <ast/context.hpp>
#include <file.hpp>
#include <gtest/gtest.h>


class InterpreterTest : public ::testing::Test {
	
	public:
		
		void interpret (const string & str, const string & expected);
		
		InterpreterTest ();
		~InterpreterTest () {}
		
	private:
		
		numbat::File dummyFile;
		AST::Context context;
		
};

