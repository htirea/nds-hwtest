#include "tests.h"
#include <nds.h>
#include <stdio.h>

#define COLOR_555(_r, _g, _b) ((_b) << 10 | (_g) << 5 | (_r))

void
setup_test()
{
	glClearColor(0, 0, 0, 0);
	glClearPolyID(0);
	glClearDepth(0x7FFF);
	MATRIX_CONTROL = 0;
	MATRIX_IDENTITY = 0;
	MATRIX_CONTROL = 1;
	MATRIX_IDENTITY = 0;
	MATRIX_CONTROL = 2;
	MATRIX_IDENTITY = 0;
	MATRIX_CONTROL = 3;
	MATRIX_IDENTITY = 0;
	glViewport(64, 32, 191, 159);
}

/*
 * Basic triangle
 */
void
test0(void)
{
	setup_test();

	GFX_POLY_FORMAT = 1 << 24 | 31 << 16 | (1 << 6) | (1 << 7);
	GFX_BEGIN = 0;
	GFX_COLOR = COLOR_555(0, 0, 31);
	GFX_VERTEX16 = (4096 << 16) | 0;
	GFX_VERTEX16 = 0;
	GFX_COLOR = COLOR_555(31, 0, 0);
	GFX_VERTEX16 = (-4096 << 16) | (-4096 & 0xFFFF);
	GFX_VERTEX16 = 0;
	GFX_COLOR = COLOR_555(0, 31, 0);
	GFX_VERTEX16 = (-4096 << 16) | (4096 & 0xFFFF);
	GFX_VERTEX16 = 0;

	glFlush(0);
}

/*
 * Basic quad
 */
void
test1(void)
{
	setup_test();

	GFX_POLY_FORMAT = 1 << 24 | 31 << 16 | (1 << 6) | (1 << 7);
	GFX_BEGIN = 1;
	GFX_COLOR = COLOR_555(31, 0, 31);
	GFX_VERTEX16 = (4096 << 16) | (-4096 & 0xFFFF);
	GFX_VERTEX16 = 0;
	GFX_COLOR = COLOR_555(31, 0, 0);
	GFX_VERTEX16 = (-4096 << 16) | (-4096 & 0xFFFF);
	GFX_VERTEX16 = 0;
	GFX_COLOR = COLOR_555(0, 31, 0);
	GFX_VERTEX16 = (-4096 << 16) | (4096 & 0xFFFF);
	GFX_VERTEX16 = 0;
	GFX_COLOR = COLOR_555(0, 0, 31);
	GFX_VERTEX16 = (4096 << 16) | (4096 & 0xFFFF);
	GFX_VERTEX16 = 0;

	glFlush(0);
}
