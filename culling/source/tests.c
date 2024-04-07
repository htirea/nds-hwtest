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
draw_line_vtx(int pos, int which, int xy, int z, int xydiff)
{
	int x = !which ? xy : -xy;
	int y = !which ? xy : -xy;

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

	x -= xydiff;
	y -= xydiff;

	gfx_vertex16(x, y, z);
}

void
draw_test_line_z_diff(int pos, int type, int which, int xy, int z0, int z1,
		int z2, int xydiff)
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
		draw_line_vtx(pos, which, xy, z0, 0);
		draw_line_vtx(pos, which, xy, z1, xydiff);
		draw_line_vtx(pos, !which, xy, z2, 0);
		break;
	case 1:
		draw_line_vtx(pos, which, xy, z0, 0);
		draw_line_vtx(pos, !which, xy, z1, 0);
		draw_line_vtx(pos, !which, xy, z2, xydiff);
		break;
	default:
		draw_line_vtx(pos, which, xy, z0, 0);
		draw_line_vtx(pos, !which, xy, z1, 0);
		draw_line_vtx(pos, which, xy, z2, xydiff);
		break;
	}
}

void
do_test(int type, int which, int xy, int z0, int z1, int z2, int xydiff)
{
	setup_test();
	gfx_color(31, 31, 31);

	draw_test_line_z_diff(0, type, which, xy, z0, z1, z2, xydiff);
	draw_test_line_z_diff(1, type, which, xy, z0, z1, z2, xydiff);
	draw_test_line_z_diff(2, type, which, xy, z0, z1, z2, xydiff);
	draw_test_line_z_diff(3, type, which, xy, z0, z1, z2, xydiff);

	glFlush(0);
}

void
do_test_6_11(int type, int which, int z0, int z1, int z2)
{
	do_test(type, which, 1024, z0, z1, z2, 0);
}

void
do_test_1_5(int type, int which)
{
	do_test_6_11(type, which, 0, 0, 0);
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
 * tests 3-5:
 * same as tests 0-2 but vtx0 is the bottom left vertex
 *
 * tests 6-11
 * same as tests 0-6 but all z values are different
 *
 * tests 12-17:
 * same as tests 0-6 but one of the similar vertices has its x/y coords
 * offset by 1. The x/y coords are adjusted so that the screen x/y coords
 * end up the same.
 *
 * tests-18-23:
 * same as tests 12-17 but the x/y coords are not adjusted. So the screen x/y
 * coords of the 'same' vertices are different.
 */
void
test0(void)
{
	do_test_1_5(0, 0);
}

void
test1(void)
{
	do_test_1_5(1, 0);
}

void
test2(void)
{
	do_test_1_5(2, 0);
}

void
test3(void)
{
	do_test_1_5(0, 1);
}

void
test4(void)
{
	do_test_1_5(1, 1);
}

void
test5(void)
{
	do_test_1_5(2, 1);
}

void
test6(void)
{
	do_test_6_11(0, 0, 1024, 2048, 3072);
}

void
test7(void)
{
	do_test_6_11(1, 0, 1024, 2048, 3072);
}

void
test8(void)
{
	do_test_6_11(2, 0, 1024, 2048, 3072);
}

void
test9(void)
{
	do_test_6_11(0, 1, 1024, 2048, 3072);
}

void
test10(void)
{
	do_test_6_11(1, 1, 1024, 2048, 3072);
}

void
test11(void)
{
	do_test_6_11(2, 1, 1024, 2048, 3072);
}

void
test12(void)
{
	do_test(0, 0, 1010, 0, 0, 0, 1);
}

void
test13(void)
{
	do_test(1, 0, 1010, 0, 0, 0, 1);
}

void
test14(void)
{
	do_test(2, 0, 1010, 0, 0, 0, 1);
}

void
test15(void)
{
	do_test(0, 1, 1010, 0, 0, 0, 1);
}

void
test16(void)
{
	do_test(1, 1, 1010, 0, 0, 0, 1);
}

void
test17(void)
{
	do_test(2, 1, 1010, 0, 0, 0, 1);
}

void
test18(void)
{
	do_test(0, 0, 1024, 0, 0, 0, 1);
}

void
test19(void)
{
	do_test(1, 0, 1024, 0, 0, 0, 1);
}

void
test20(void)
{
	do_test(2, 0, 1024, 0, 0, 0, 1);
}

void
test21(void)
{
	do_test(0, 1, 1024, 0, 0, 0, 1);
}

void
test22(void)
{
	do_test(1, 1, 1024, 0, 0, 0, 1);
}

void
test23(void)
{
	do_test(2, 1, 1024, 0, 0, 0, 1);
}
