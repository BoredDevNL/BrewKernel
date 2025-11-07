#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include "../print.h"
#include "../io.h"

void brewing(int iterations);

void shutdown_command() {
    brew_str("\nInitiating system shutdown...\n");
    brewing(10000000); 
    outw(0xB004, 0x2000);
    outb(0x64, 0xFE);
        brew_str("WARNING: System shutdown failed.\n");
    brew_str("It is now safe to turn off your computer.\n");
}

#endif