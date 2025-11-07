#ifndef REBOOT_H
#define REBOOT_H

#include "../print.h"
#include "../io.h"

void brewing(int iterations);

void reboot_command() {
    brew_str("\nInitiating system reboot...\n");
    brewing(10000000);  
    while ((inb(0x64) & 2) != 0) {
        brewing(1000);
    }  
    outb(0x64, 0xFE); 
    brewing(50000000);   
    asm volatile ("lidt %0" : : "m"(*(char*)0)); 
    asm volatile ("int $0x3");  
    brew_str("WARNING: System reboot failed.\n");
    brew_str("Please reset your computer manually.\n");
}

#endif