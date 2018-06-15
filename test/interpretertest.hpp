#include <ast/context.hpp>
#include <file.hpp>
#include <gtest/gtest.h>


class InterpreterTest : public ::testing::Test {
	
	public:
		
		string interpret (const std::string & str);
		
		InterpreterTest ();
		~InterpreterTest () {}
		
	private:
		
		numbat::File dummyFile;
		AST::Context context;
		
};

