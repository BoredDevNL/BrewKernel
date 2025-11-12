#include <stdio.h>

#include "m_argv.h"

#include "doomgeneric.h"
#include "print.h"

pixel_t* DG_ScreenBuffer = NULL;

void M_FindResponseFile(void);
void D_DoomMain (void);


void doomgeneric_Create(int argc, char **argv)
{
	// save arguments
	brew_str("[DOOM] Saving arguments...\n");
    myargc = argc;
    myargv = argv;

	brew_str("[DOOM] Finding response file...\n");
	M_FindResponseFile();

	brew_str("[DOOM] Allocating screen buffer...\n");
	DG_ScreenBuffer = malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * 4);

	if (!DG_ScreenBuffer) {
		brew_str("ERROR: Failed to allocate screen buffer\n");
		return;
	}

	brew_str("[DOOM] Initializing display...\n");
	DG_Init();

	brew_str("[DOOM] Starting main game loop...\n");
	D_DoomMain ();
}

