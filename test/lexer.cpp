#include <gtest/gtest.h>
#include <iostream>
#include <lexer.hpp>

TEST (lexer, numbers) {
	
	auto zeroTkns = numbat::lexer::lex ("0");
	ASSERT_EQ (zeroTkns.size (), 2);
	EXPECT_TRUE (zeroTkns [0].iden == "0");
	EXPECT_EQ (zeroTkns [0].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroTkns [1].type, numbat::lexer::TOKEN::semicolon);
	
	zeroTkns = numbat::lexer::lex ("0 0");
	ASSERT_EQ (zeroTkns.size (), 3);
	EXPECT_TRUE (zeroTkns [0].iden == "0");
	EXPECT_TRUE (zeroTkns [1].iden == "0");
	EXPECT_EQ (zeroTkns [0].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroTkns [1].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroTkns [2].type, numbat::lexer::TOKEN::semicolon);
	
}
