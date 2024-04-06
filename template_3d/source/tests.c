#include "util.h"

#include <nds.h>
#include <stdio.h>

void
gfx_vertex16(int x, int y, int z)
{
	GFX_VERTEX16 = (y << 16 & 0xFFFF0000) | (x & 0xFFFF);
	GFX_VERTEX16 = (z & 0xFFFF);
}

void
gfx_color(int r, int g, int b)
{
	GFX_COLOR = b << 10 | g << 5 | r;
}

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
	glViewport(VIEWPORT_X0, VIEWPORT_Y0, VIEWPORT_X1, VIEWPORT_Y1);
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

	gfx_color(0, 0, 31);
	gfx_vertex16(0, 4096, 0);

	gfx_color(31, 0, 0);
	gfx_vertex16(-4096, -4096, 0);

	gfx_color(0, 31, 0);
	gfx_vertex16(4096, -4096, 0);

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

	gfx_color(31, 0, 31);
	gfx_vertex16(-4096, 4096, 0);

	gfx_color(31, 0, 0);
	gfx_vertex16(-4096, -4096, 0);

	gfx_color(0, 31, 0);
	gfx_vertex16(4096, -4096, 0);

	gfx_color(0, 0, 31);
	gfx_vertex16(4096, 4096, 0);

	glFlush(0);
}
