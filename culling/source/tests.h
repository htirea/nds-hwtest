#ifndef TESTS_H
#define TESTS_H

enum {
	TEST_RESULT_WIDTH = 128,
	TEST_RESULT_HEIGHT = 128,
	TEST_RESULT_SIZE_BYTES = 2 * TEST_RESULT_WIDTH * TEST_RESULT_HEIGHT,
};

typedef void (*test_func)(int test_num);

void test0(int test_num);

#endif
