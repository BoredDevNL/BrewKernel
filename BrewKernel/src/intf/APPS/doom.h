/*
 * Brew Kernel
 * Copyright (C) 2024-2025 boreddevhq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef APPS_DOOM_H
#define APPS_DOOM_H

#include "print.h"
#include "APPS/DOOM/doomgeneric.h"

static void doom() {
	brew_str("\nLaunching DOOM (text-mode)...\n");
	print_disable_cursor();

	// Minimal argv
	char* argv[] = { "doom" };
	int argc = 1;

	brew_str("Calling doomgeneric_Create...\n");
	doomgeneric_Create(argc, argv);
	
	// If we reach here, something went wrong in doomgeneric_Create
	brew_str("ERROR: doomgeneric_Create returned unexpectedly\n");
	return;
}

#endif // APPS_DOOM_H

// someone please make a doom port for brew cause i'm too stupid for that :3