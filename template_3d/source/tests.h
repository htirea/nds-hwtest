#ifndef TESTS_H
#define TESTS_H

enum {
	TEST_RESULT_WIDTH = 128,
	TEST_RESULT_HEIGHT = 128,
	TEST_RESULT_SIZE_BYTES = 2 * TEST_RESULT_WIDTH * TEST_RESULT_HEIGHT,
};

typedef void (*test_func)(void);

void test0(void);
void test1(void);

#endif
