#include "results_bin.h"
#include "tests.h"
#include <fat.h>
#include <filesystem.h>
#include <nds.h>
#include <stdio.h>

#define PROGNAME "template_3d"
#define PROGVER "v1"

enum {
	MODE_TITLE,
	MODE_AUTO_TEST,
	MODE_VIEW_TEST,
	MODE_RECORD,
};

enum {
	DISPLAY_CAPTURED,
	DISPLAY_EXPECTED,
	NUM_DISPLAY_MODES,
};

typedef void (*test_func)(void);
test_func test_funcs[] = {
	test0,
	test1,
};

#define NUM_TESTS (sizeof(test_funcs) / sizeof(test_func))
#define TEST_RESULT_WIDTH 128
#define TEST_RESULT_HEIGHT 128
#define TEST_RESULT_SIZE (2 * TEST_RESULT_WIDTH * TEST_RESULT_HEIGHT)

static struct global_state {
	int mode;
	int pressed;
	int test_num;
	bool quit;
	bool test_results[NUM_TESTS];
	bool rerender;
	int display_mode;
	FILE *file;
} gs;

static void mode_title();
static void mode_autotest();
static void mode_viewtest();
static void mode_record();
static void run_test();
static int check_test();
static void record_test();
static void copy_expected_to_vram();

int
main(void)
{
	fatInitDefault();
	gs.file = fopen(PROGNAME "_data.bin", "wb");

	consoleDemoInit();
	videoSetMode(MODE_0_3D);
	glInit();
	vramSetBankA(VRAM_A_LCD);
	REG_DISPCAPCNT = DCAP_MODE(DCAP_MODE_A) |
	                 DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
	                 DCAP_SIZE(DCAP_SIZE_256x192) | DCAP_OFFSET(0) |
	                 DCAP_BANK(DCAP_BANK_VRAM_A);

	gs.mode = MODE_TITLE;

	vramSetBankB(VRAM_B_LCD);
	for (int y = 0; y < 192; y++) {
		for (int x = 0; x < 256; x++) {
			VRAM_B[y * 256 + x] = 0;
		}
	}

	while (!gs.quit) {
		scanKeys();
		gs.pressed = keysDown();

		switch (gs.mode) {
		case MODE_TITLE:
			mode_title();
			break;
		case MODE_AUTO_TEST:
			mode_autotest();
			break;
		case MODE_VIEW_TEST:
			mode_viewtest();
			break;
		case MODE_RECORD:
			mode_record();
			break;
		}

		swiWaitForVBlank();
	}

	if (gs.file) {
		fclose(gs.file);
	}
}

static void
mode_title()
{
	videoSetMode(MODE_0_3D);
	consoleClear();
	iprintf("%s %s\n\n"
		"A: Run all tests\n"
		"RIGHT: View tests\n"
		"SELECT: Record\n"
		"START: Exit\n",
			PROGNAME, PROGVER);

	if (!gs.file) {
		iprintf("\nCould not open file\n");
	}

	if (gs.pressed & KEY_A) {
		gs.mode = MODE_AUTO_TEST;
		gs.test_num = 0;
	} else if (gs.pressed & KEY_RIGHT) {
		gs.mode = MODE_VIEW_TEST;
		gs.test_num = 0;
		gs.rerender = true;
		gs.display_mode = DISPLAY_CAPTURED;
	} else if (gs.pressed & KEY_SELECT) {
		gs.mode = MODE_RECORD;
		gs.test_num = 0;
	} else if (gs.pressed & KEY_START) {
		gs.quit = true;
	}
}

static void
mode_autotest()
{
	if (gs.test_num < NUM_TESTS) {
		run_test();
		int err = check_test();
		gs.test_results[gs.test_num] = err;
		gs.test_num++;
	}

	if (gs.test_num != NUM_TESTS)
		return;

	consoleClear();
	iprintf("Finished testing\n\n");

	int num_failed = 0;

	for (int i = 0; i < NUM_TESTS; i++) {
		if (gs.test_results[i]) {
			num_failed++;
		}
	}

	if (num_failed == 0) {
		iprintf("All tests passed\n");
	} else {
		iprintf("Failed:\n");
		for (int i = 0; i < NUM_TESTS; i++) {
			if (gs.test_results[i]) {
				iprintf("%i\n", i);
			}
		}
	}

	iprintf("\n"
		"A: Return to title\n"
		"START: Exit\n");

	if (gs.pressed & KEY_A) {
		gs.mode = MODE_TITLE;
	} else if (gs.pressed & KEY_START) {
		gs.quit = true;
	}
}

static void
mode_viewtest()
{
	if (gs.rerender) {
		run_test();
		gs.test_results[gs.test_num] = check_test();
		copy_expected_to_vram();
		gs.rerender = false;
	}

	switch (gs.display_mode) {
	case DISPLAY_CAPTURED:
		videoSetMode(MODE_FB0);
		break;
	case DISPLAY_EXPECTED:
		videoSetMode(MODE_FB1);
		break;
	}

	consoleClear();
	iprintf("Test: %i\n\n", gs.test_num);
	if (gs.test_results[gs.test_num]) {
		iprintf("Failed\n");
	} else {
		iprintf("Passed\n");
	}

	iprintf("\n"
		"LEFT: Previous test\n"
		"RIGHT: Next test\n"
		"X: Toggle display mode\n"
		"A: Return to title\n"
		"START: Exit\n");

	iprintf("\nDisplay mode: ");
	switch (gs.display_mode) {
	case DISPLAY_CAPTURED:
		iprintf("captured\n");
		break;
	case DISPLAY_EXPECTED:
		iprintf("expected\n");
		break;
	}

	if (gs.pressed & KEY_LEFT) {
		if (gs.test_num > 0) {
			gs.test_num--;
			gs.rerender = true;
			gs.display_mode = DISPLAY_CAPTURED;
		}
	} else if (gs.pressed & KEY_RIGHT) {
		if (gs.test_num < NUM_TESTS - 1) {
			gs.test_num++;
			gs.rerender = true;
			gs.display_mode = DISPLAY_CAPTURED;
		}
	} else if (gs.pressed & KEY_X) {
		gs.display_mode = (gs.display_mode + 1) % NUM_DISPLAY_MODES;
	} else if (gs.pressed & KEY_A) {
		gs.mode = MODE_TITLE;
	} else if (gs.pressed & KEY_START) {
		gs.quit = true;
	}
}

static void
mode_record()
{
	if (gs.test_num < NUM_TESTS) {
		run_test();
		record_test();
		gs.test_num++;
	}

	if (gs.test_num != NUM_TESTS)
		return;

	consoleClear();
	iprintf("Finished recording\n"
		"\n"
		"A: Return to title\n"
		"START: Exit\n");

	if (gs.pressed & KEY_A) {
		gs.mode = MODE_TITLE;
	} else if (gs.pressed & KEY_START) {
		gs.quit = true;
	}
}

static void
run_test()
{
	test_funcs[gs.test_num]();
	swiWaitForVBlank();

	REG_DISPCAPCNT |= DCAP_ENABLE;
	while (REG_DISPCAPCNT & DCAP_ENABLE)
		;
}

static int
check_test()
{
	int offset = gs.test_num * TEST_RESULT_SIZE;
	if (offset + TEST_RESULT_SIZE > results_bin_size) {
		return 1;
	}

	u16 *src = (u16 *)(results_bin + offset);

	for (int y = 0; y < TEST_RESULT_HEIGHT; y++) {
		for (int x = 0; x < TEST_RESULT_WIDTH; x++) {
			if (VRAM_A[(y + 32) * 256 + 64 + x] != *src++) {
				return 1;
			}
		}
	}

	return 0;
}

static void
record_test()
{
	if (!gs.file)
		return;

	if (gs.test_num == 0) {
		fseek(gs.file, 0, SEEK_SET);
	}

	for (int y = 0; y < TEST_RESULT_HEIGHT; y++) {
		fwrite(&VRAM_A[(y + 32) * 256 + 64], 1, TEST_RESULT_WIDTH * 2,
				gs.file);
	}
}

static void
copy_expected_to_vram()
{
	int offset = gs.test_num * TEST_RESULT_SIZE;
	if (offset + TEST_RESULT_SIZE > results_bin_size) {
		return;
	}

	u16 *src = (u16 *)(results_bin + offset);

	for (int y = 0; y < TEST_RESULT_HEIGHT; y++) {
		for (int x = 0; x < TEST_RESULT_WIDTH; x++) {
			VRAM_B[(y + 32) * 256 + 64 + x] = *src++;
		}
	}
}
