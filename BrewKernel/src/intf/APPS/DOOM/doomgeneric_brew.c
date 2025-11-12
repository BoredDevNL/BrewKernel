/*
 * doomgeneric_brew.c
 * Platform glue for DoomGeneric on BrewKernel (text-mode ASCII rendering).
 *
 * Audio is not implemented.
 */

#include "doomgeneric.h"

#include "print.h"
#include "keyboard.h"
#include "timer.h"

#include <stdint.h>
#include <stddef.h>

// Simple luminance to ASCII ramp
static const char* kAsciiRamp = " .:-=+*#%@";
static const size_t kAsciiRampLen = 10;

// Cache last drawn ASCII frame to avoid redundant screen clears (optional/simple)
static void draw_ascii_frame_from_rgba32(const uint32_t* pixels, uint32_t width, uint32_t height) {
	// We have an 80x25 text grid; assume 8x16 pixel blocks map nicely from 640x400
	const uint32_t cell_w = 8;
	const uint32_t cell_h = 16;
	const uint32_t cols = 80;
	const uint32_t rows = 25;

	// Safety: only handle expected sizes (falls back to scaled mapping otherwise)
	const uint32_t eff_w = width;
	const uint32_t eff_h = height;

	// Render full screen each frame for simplicity
	print_clear();
	for (uint32_t ty = 0; ty < rows; ++ty) {
		for (uint32_t tx = 0; tx < cols; ++tx) {
			// Average luminance over the block
			uint64_t sum_luma = 0;
			uint32_t samples = 0;

			const uint32_t x0 = tx * cell_w;
			const uint32_t y0 = ty * cell_h;
			for (uint32_t py = 0; py < cell_h; ++py) {
				const uint32_t y = y0 + py;
				if (y >= eff_h) break;
				const uint32_t row_off = y * eff_w;
				for (uint32_t px = 0; px < cell_w; ++px) {
					const uint32_t x = x0 + px;
					if (x >= eff_w) break;
					const uint32_t rgba = pixels[row_off + x];
					const uint8_t r = (uint8_t)((rgba >> 16) & 0xFF);
					const uint8_t g = (uint8_t)((rgba >> 8) & 0xFF);
					const uint8_t b = (uint8_t)((rgba >> 0) & 0xFF);
					// Perceived luminance (BT.601)
					const uint32_t l = (uint32_t)(299u * r + 587u * g + 114u * b);
					sum_luma += l;
					++samples;
				}
			}

			char ch = ' ';
			if (samples > 0) {
				// Normalize to 0..255
				const uint32_t avg_l = (uint32_t)(sum_luma / samples) / 10; // 299+587+114=1000 scale
				// Map luminance to ramp index
				const uint32_t idx = (avg_l * (kAsciiRampLen - 1)) / 25; // avg_l in ~[0..25]
				ch = kAsciiRamp[idx < kAsciiRampLen ? idx : (kAsciiRampLen - 1)];
			}
			print_char(ch);
		}
		print_char('\n');
	}
}

void DG_Init() {
	// Nothing special; screen will be cleared per frame.
	// Optionally hide cursor during gameplay for cleaner output.
	print_disable_cursor();
}

void DG_DrawFrame() {
	// DG_ScreenBuffer is RGBA8888 pixels of size DOOMGENERIC_RESX x DOOMGENERIC_RESY by default.
	if (!DG_ScreenBuffer) {
		return;
	}
	draw_ascii_frame_from_rgba32((const uint32_t*)DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

void DG_SleepMs(uint32_t ms) {
	timer_sleep_ms(ms);
}

uint32_t DG_GetTicksMs() {
	// timer_get_ticks returns ticks at configured frequency (default 100 Hz = 10ms/tick)
	const uint64_t ticks = timer_get_ticks();
	// Convert to milliseconds using TIMER_FREQUENCY
	return (uint32_t)((ticks * 1000ULL) / (uint64_t)TIMER_FREQUENCY);
}

// Map PS/2 scancodes to Doom keycodes (doomkeys.h)
static int map_scan_to_doom_key(unsigned char scan, int* pressed_out, unsigned char* key_out) {
	// Key release scancodes have the high bit set in set 1? Some use 0x80+code; keyboard.asm likely provides raw codes.
	// We detect basic press/release by simple heuristic: treat values >= 0x80 as release of (scan&0x7F).
	int pressed = 1;
	unsigned char code = scan;
	if (scan >= 0x80) {
		pressed = 0;
		code = (unsigned char)(scan & 0x7F);
	}

	unsigned char key = 0;

	// Try ASCII mapping first
	char ascii = scan_code_to_ascii(code);
	if (ascii != 0) {
		key = (unsigned char)ascii;
	} else {
		// Special keys and arrows
		// Scan set 1 common arrows: up=0x48, left=0x4B, right=0x4D, down=0x50
		switch (code) {
			case 0x48: key = 0xAD; break; // KEY_UPARROW
			case 0x50: key = 0xAF; break; // KEY_DOWNARROW
			case 0x4B: key = 0xAC; break; // KEY_LEFTARROW
			case 0x4D: key = 0xAE; break; // KEY_RIGHTARROW
			case 0x01: key = 27;    break; // ESC
			case 0x1C: key = 13;    break; // ENTER
			case 0x0F: key = 9;     break; // TAB
			case 0x0E: key = 0x7F;  break; // BACKSPACE
			case 0x2A:
			case 0x36: key = (0x80+0x36); break; // RSHIFT
			default: key = 0; break;
		}
	}

	if (key == 0) {
		return 0;
	}
	*pressed_out = pressed;
	*key_out = key;
	return 1;
}

int DG_GetKey(int* pressed, unsigned char* key) {
	if (!check_keyboard()) {
		return 0;
	}
	unsigned char scan = read_scan_code();
	return map_scan_to_doom_key(scan, pressed, key);
}

void DG_SetWindowTitle(const char * title) {
	(void)title;
	// No real window title in text mode; optionally print once at top
}


