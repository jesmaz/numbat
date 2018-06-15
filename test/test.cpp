#include <gtest/gtest.h>

int main (int argl, char ** args) {
	::testing::InitGoogleTest (&argl, args);
	return RUN_ALL_TESTS ();
}
