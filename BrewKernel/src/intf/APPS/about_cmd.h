/*
 * Brew Kernel
 * Copyright (C) 2025-2026 boreddevnl
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
#ifndef APPS_ABOUT_CMD_H
#define APPS_ABOUT_CMD_H

#include "../print.h"

static int strcmp_about(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static void display_about_command(const char* cmd_name) {
    brew_str("\n");
    if (strcmp_about(cmd_name, "HELP") == 0) {
        brew_str("HELP - Display available commands\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows a list of all available commands in the Brew Kernel CLI.\n");
    }
    else if (strcmp_about(cmd_name, "DATE") == 0) {
        brew_str("DATE - Display current date and time\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows the current system date and time with timezone support.\n");
    }
    else if (strcmp_about(cmd_name, "CALC") == 0) {
        brew_str("CALC - short for calculator.\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("A visual calculator with numpad navigation using arrow keys.\n");
        brew_str("Navigate with arrow keys, press Enter to select, C to clear, ESC to exit.\n");
    }
    else if (strcmp_about(cmd_name, "SYSINFO") == 0) {
        brew_str("SYSINFO - Display system information\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows detailed system information.\n");
    }
    else if (strcmp_about(cmd_name, "MAN") == 0) {
        brew_str("MAN - Show detailed user manual\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Displays detailed manual.\n");
    }
    else if (strcmp_about(cmd_name, "LICENSE") == 0) {
        brew_str("LICENSE - Display GNU GPLv3 license\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows the full GNU General Public License v3.\n");
    }
    else if (strcmp_about(cmd_name, "UPTIME") == 0) {
        brew_str("UPTIME - Display system uptime\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows how long the system has been running since boot.\n");
    }
    else if (strcmp_about(cmd_name, "MEMORY") == 0) {
        brew_str("MEMORY - Display memory usage statistics\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows current memory allocation and usage information.\n");
    }
    else if (strcmp_about(cmd_name, "BEEP") == 0) {
        brew_str("BEEP - Makes a beep sound using the PC speaker\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Produces an audible beep tone from the system speaker.\n");
    }
    else if (strcmp_about(cmd_name, "TXTEDIT") == 0) {
        brew_str("TXTEDIT - Open the text editor\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("A simple text editor.\n");
    }
    else if (strcmp_about(cmd_name, "COWSAY") == 0) {
        brew_str("COWSAY - Display text with an ASCII cow\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Makes an ASCII cow say whatever you want.\n");
    }
    else if (strcmp_about(cmd_name, "CLEAR") == 0) {
        brew_str("CLEAR - Clear the screen\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Clears the console display.\n");
    }
    else if (strcmp_about(cmd_name, "LS") == 0) {
        brew_str("LS - List files in directory\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Lists files and directories in the current or specified directory.\n");
    }
    else if (strcmp_about(cmd_name, "CD") == 0) {
        brew_str("CD - Change current directory\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Changes the working directory.\n");
    }
    else if (strcmp_about(cmd_name, "PWD") == 0) {
        brew_str("PWD - Print working directory\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Displays the full path of the current working directory.\n");
    }
    else if (strcmp_about(cmd_name, "MKDIR") == 0) {
        brew_str("MKDIR - Create directories\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Creates one or more new directories.\n");
    }
    else if (strcmp_about(cmd_name, "RM") == 0) {
        brew_str("RM - Remove files or directories\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Deletes a file or empty directory.\n");
    }
    else if (strcmp_about(cmd_name, "CAT") == 0) {
        brew_str("CAT - Display file contents\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Shows the contents of a text file.\n");
    }
    else if (strcmp_about(cmd_name, "ECHO") == 0) {
        brew_str("ECHO - Print text or write to file\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Prints text to screen or redirects to file.\n");
    }
    else if (strcmp_about(cmd_name, "TOUCH") == 0) {
        brew_str("TOUCH - Create empty file\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Creates a new empty file.\n");
    }
    else if (strcmp_about(cmd_name, "NETINIT") == 0) {
        brew_str("NETINIT - Initialize network card\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Initializes and enables the network interface card.\n");
    }
    else if (strcmp_about(cmd_name, "NETINFO") == 0) {
        brew_str("NETINFO - Show network status\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Displays network configuration.\n");
    }
    else if (strcmp_about(cmd_name, "UDPTEST") == 0) {
        brew_str("UDPTEST - UDP echo server\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Starts a UDP echo server on port 12345 (experimental).\n");
    }
    else if (strcmp_about(cmd_name, "UDPSEND") == 0) {
        brew_str("UDPSEND - Send UDP packet\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Sends a UDP packet to a host.\n");
    }
    else if (strcmp_about(cmd_name, "SHUTDOWN") == 0) {
        brew_str("SHUTDOWN - Shutdown the system\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Powers down the Brew Kernel system.\n");
    }
    else if (strcmp_about(cmd_name, "REBOOT") == 0) {
        brew_str("REBOOT - Reboot the system\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Restarts the Brew Kernel system.\n");
    }
    else if (strcmp_about(cmd_name, "EXIT") == 0) {
        brew_str("EXIT - Exit CLI mode\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("Exits the command line interface/shuts down the system.\n");
    }
    else if (strcmp_about(cmd_name, "BREWER") == 0) {
        brew_str("BREWER - Compiler\n");
        brew_str("Copyright (C) 2025-2026 boreddevnl\n");
        brew_str("The Brew Kernel basic compiler.\n");
    }
    else {
        brew_str("about: unknown command '");
        brew_str(cmd_name);
        brew_str("'\n");
        brew_str("Use 'HELP' to see available commands.\n");
    }
}

static void about_command(const char* command_buffer) {
    // Extract the command name after "ABOUT "
    const char* cmd_name = command_buffer + 6; // Skip "ABOUT "
    
    // Skip leading spaces
    while (*cmd_name == ' ') cmd_name++;
    
    if (*cmd_name == '\0') {
        brew_str("\nusage: ABOUT [command]\n");
        brew_str("Display information about a specific command.\n");
        brew_str("Example: ABOUT TXTEDIT\n");
    } else {
        // Convert to uppercase for comparison
        char cmd_upper[256];
        int i;
        for(i = 0; cmd_name[i] && i < 255; i++) {
            cmd_upper[i] = cmd_name[i] >= 'a' && cmd_name[i] <= 'z' 
                         ? cmd_name[i] - 32 
                         : cmd_name[i];
        }
        cmd_upper[i] = '\0';
        
        display_about_command(cmd_upper);
    }
}

#endif // APPS_ABOUT_CMD_H
