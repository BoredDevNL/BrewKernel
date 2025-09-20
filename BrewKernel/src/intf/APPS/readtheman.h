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
#ifndef APPS_NERD_H
#define APPS_NERD_H

#include "print.h"

static void nerd() {
    brew_str("\n");
    brew_str("You read the manual? NERD. you know what?\n");
    brew_str("Fuck you.\n");
    brewing(5000000000);
    
    for(int i = 0; i < 1000000000000000000000000000; i++) {  
        print_set_color(PRINT_INDEX_0, PRINT_INDEX_15); // black on white
        print_clear();
        brewing(500000); 
        
        print_set_color(PRINT_INDEX_15, PRINT_INDEX_0); // white on black
        print_clear();
        brewing(500000);
    }

}

#endif // APPS_NERD_H
