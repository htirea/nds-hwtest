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
	gfx_color(31, 31, 31);
}

void
draw_vtx(int pos, int which)
{
	int x = !which ? 1024 : -1024;
	int y = !which ? 1024 : -1024;

	switch (pos) {
	case 0:
		x -= 2048;
		y += 2048;
		break;
	case 1:
		x += 2048;
		y += 2048;
		break;
	case 2:
		x -= 2048;
		y -= 2048;
		break;
	default:
		x += 2048;
		y -= 2048;
		break;
	}

	gfx_vertex16(x, y, 0);
}

void
draw_test_line(int pos, int type, int which)
{
	int cull_bits = 0;
	switch (pos) {
	case 0:
		cull_bits = 0;
		break;
	case 1:
		cull_bits = 1 << 7;
		break;
	case 2:
		cull_bits = 1 << 6;
		break;
	default:
		cull_bits = 1 << 6 | 1 << 7;
		break;
	}

	GFX_POLY_FORMAT = 1 << 24 | 31 << 16 | cull_bits;
	GFX_BEGIN = 0;

	switch (type) {
	case 0:
		draw_vtx(pos, which);
		draw_vtx(pos, which);
		draw_vtx(pos, !which);
		break;
	case 1:
		draw_vtx(pos, which);
		draw_vtx(pos, !which);
		draw_vtx(pos, !which);
		break;
	default:
		draw_vtx(pos, which);
		draw_vtx(pos, !which);
		draw_vtx(pos, which);
		break;
	}
}

/*
 * Triangle culling (when two vertices are the same)
 *
 * Position		Bit 6	Bit 7
 * 0. Top left		0	0
 * 1. Top right		0	1
 * 2. Bottom left	1	0
 * 3. Bottom right	1	1
 *
 * test0: vtx0 == vtx1
 * test1: vtx1 == vtx2
 * test2: vtx0 == vtx2
 *
 * vtx0 is the top right vertex
 *
 * test3:
 * test4:
 * test5:
 *
 * same as above but vtx0 is the bottom left vertex
 */
void
test0(void)
{
	setup_test();
	gfx_color(31, 31, 31);

	draw_test_line(0, 0, 0);
	draw_test_line(1, 0, 0);
	draw_test_line(2, 0, 0);
	draw_test_line(3, 0, 0);

	glFlush(0);
}

void
test1(void)
{
	setup_test();
	gfx_color(31, 31, 31);

	draw_test_line(0, 1, 0);
	draw_test_line(1, 1, 0);
	draw_test_line(2, 1, 0);
	draw_test_line(3, 1, 0);

	glFlush(0);
}

void
test2(void)
{
	setup_test();
	gfx_color(31, 31, 31);

	draw_test_line(0, 2, 0);
	draw_test_line(1, 2, 0);
	draw_test_line(2, 2, 0);
	draw_test_line(3, 2, 0);

	glFlush(0);
}

void
test3(void)
{
	setup_test();

	draw_test_line(0, 0, 1);
	draw_test_line(1, 0, 1);
	draw_test_line(2, 0, 1);
	draw_test_line(3, 0, 1);

	glFlush(0);
}

void
test4(void)
{
	setup_test();
	gfx_color(31, 31, 31);

	draw_test_line(0, 1, 1);
	draw_test_line(1, 1, 1);
	draw_test_line(2, 1, 1);
	draw_test_line(3, 1, 1);

	glFlush(0);
}

void
test5(void)
{
	setup_test();
	gfx_color(31, 31, 31);

	draw_test_line(0, 2, 1);
	draw_test_line(1, 2, 1);
	draw_test_line(2, 2, 1);
	draw_test_line(3, 2, 1);

	glFlush(0);
}
