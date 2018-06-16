#include <gtest/gtest.h>
#include <iostream>
#include <lexer.hpp>

TEST (lexer, negative_int) {
	
	auto negInt = numbat::lexer::lex ("-1");
	ASSERT_EQ (negInt.size (), 3);
	EXPECT_TRUE (negInt [0].iden == "-");
	EXPECT_TRUE (negInt [1].iden == "1");
	EXPECT_EQ (negInt [0].type, numbat::lexer::TOKEN::symbol);
	EXPECT_EQ (negInt [1].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (negInt [2].type, numbat::lexer::TOKEN::semicolon);
	
}

TEST (lexer, positive_int) {
	
	auto posInt = numbat::lexer::lex ("10");
	ASSERT_EQ (posInt.size (), 2);
	EXPECT_TRUE (posInt [0].iden == "10");
	EXPECT_EQ (posInt [0].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (posInt [1].type, numbat::lexer::TOKEN::semicolon);
	
	posInt = numbat::lexer::lex ("09");
	ASSERT_EQ (posInt.size (), 2);
	EXPECT_TRUE (posInt [0].iden == "09");
	EXPECT_EQ (posInt [0].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (posInt [1].type, numbat::lexer::TOKEN::semicolon);
	
	posInt = numbat::lexer::lex ("09 35");
	ASSERT_EQ (posInt.size (), 3);
	EXPECT_TRUE (posInt [0].iden == "09");
	EXPECT_TRUE (posInt [1].iden == "35");
	EXPECT_EQ (posInt [0].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (posInt [1].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (posInt [2].type, numbat::lexer::TOKEN::semicolon);
	
}

TEST (lexer, single_digits) {
	
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
	
	auto zeroToNine  = numbat::lexer::lex ("0 1 2 3 4 5 6 7 8 9");
	ASSERT_EQ (zeroToNine.size (), 11);
	EXPECT_TRUE (zeroToNine [0].iden == "0");
	EXPECT_TRUE (zeroToNine [1].iden == "1");
	EXPECT_TRUE (zeroToNine [2].iden == "2");
	EXPECT_TRUE (zeroToNine [3].iden == "3");
	EXPECT_TRUE (zeroToNine [4].iden == "4");
	EXPECT_TRUE (zeroToNine [5].iden == "5");
	EXPECT_TRUE (zeroToNine [6].iden == "6");
	EXPECT_TRUE (zeroToNine [7].iden == "7");
	EXPECT_TRUE (zeroToNine [8].iden == "8");
	EXPECT_TRUE (zeroToNine [9].iden == "9");
	EXPECT_EQ (zeroToNine [0].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [1].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [2].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [3].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [4].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [6].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [7].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [8].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [9].type, numbat::lexer::TOKEN::numericliteral);
	EXPECT_EQ (zeroToNine [10].type, numbat::lexer::TOKEN::semicolon);
	
}
