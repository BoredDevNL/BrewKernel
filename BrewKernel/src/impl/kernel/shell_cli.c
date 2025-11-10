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
#include "print.h"
#include "filesys.h"
#include "file.h"
#include "pci.h"
#include "shell_cli.h"

static int strcmp_kernel_cli(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static int strncmp_kernel_cli(const char *s1, const char *s2, int n) {
	for (int i = 0; i < n; i++) {
		if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
			return (unsigned char)s1[i] - (unsigned char)s2[i];
		}
	}
	return 0;
}

static int brew_strlen_cli(const char* s) {
	int n = 0;
	while (s[n] != '\0') n++;
	return n;
}

static int split_command_cli(char *cmd, char *args[], int max_args) {
	int arg_count = 0;
	while (*cmd && arg_count < max_args) {
		while (*cmd == ' ' || *cmd == '\t') {
			*cmd++ = '\0';
		}
		if (*cmd == '\0') break;
		args[arg_count++] = cmd;
		while (*cmd && *cmd != ' ' && *cmd != '\t') {
			cmd++;
		}
	}
	return arg_count;
}

static void handle_pciscan(void) {
	brew_str("\nScanning PCI bus...\n");
	pci_device_t devices[32];
	int count = pci_enumerate_devices(devices, 32);
	if (count == 0) {
		brew_str("No PCI devices found.\n");
		return;
	}
	brew_str("Found ");
	char count_str[16];
	int i = 0;
	int num = count;
	if (num == 0) {
		count_str[i++] = '0';
	} else {
		char temp[16];
		int j = 0;
		while (num > 0) {
			temp[j++] = '0' + (num % 10);
			num /= 10;
		}
		for (int k = j - 1; k >= 0; k--) {
			count_str[i++] = temp[k];
		}
	}
	count_str[i] = '\0';
	brew_str(count_str);
	brew_str(" device(s):\n\n");

	for (int idx = 0; idx < count; idx++) {
		char bus_str[4], dev_str[4], func_str[4];
		int bus = devices[idx].bus;
		int dev = devices[idx].device;
		int func = devices[idx].function;
		bus_str[0] = '0' + (bus / 100);
		bus_str[1] = '0' + ((bus / 10) % 10);
		bus_str[2] = '0' + (bus % 10);
		bus_str[3] = '\0';
		dev_str[0] = '0' + (dev / 10);
		dev_str[1] = '0' + (dev % 10);
		dev_str[2] = '\0';
		func_str[0] = '0' + func;
		func_str[1] = '\0';
		brew_str("  ");
		brew_str(bus_str);
		brew_str(":");
		brew_str(dev_str);
		brew_str(".");
		brew_str(func_str);
		brew_str("  Vendor: 0x");
		char hex[] = "0123456789ABCDEF";
		brew_str("0000");
		int vendor = devices[idx].vendor_id;
		for (int h = 3; h >= 0; h--) {
			char hex_digit[2] = {hex[(vendor >> (h * 4)) & 0xF], '\0'};
			brew_str(hex_digit);
		}
		brew_str("  Device: 0x");
		int device_id = devices[idx].device_id;
		for (int h = 3; h >= 0; h--) {
			char hex_digit2[2] = {hex[(device_id >> (h * 4)) & 0xF], '\0'};
			brew_str(hex_digit2);
		}
		brew_str("  Class: 0x");
		int class_code = devices[idx].class_code;
		for (int h = 1; h >= 0; h--) {
			char hex_digit3[2] = {hex[(class_code >> (h * 4)) & 0xF], '\0'};
			brew_str(hex_digit3);
		}
		brew_str("\n");
	}
}

static void handle_echo(const char* command_buffer) {
	brew_str("\n");
	if (brew_strlen_cli(command_buffer) <= 5) {
		brew_str("\n");
		return;
	}
	const char* args = &command_buffer[5];
	while (*args == ' ') args++;
	if (*args == '\0') {
		brew_str("\n");
		return;
	}
	const char* redirect_pos = args;
	const char* filename = NULL;
	const char* text_start = args;
	const char* text_end = NULL;
	while (*redirect_pos) {
		if (*redirect_pos == '>') {
			text_end = redirect_pos;
			while (text_end > text_start && (*(text_end - 1) == ' ' || *(text_end - 1) == '"')) {
				text_end--;
			}
			filename = redirect_pos + 1;
			while (*filename == '>' || *filename == ' ') filename++;
			break;
		}
		redirect_pos++;
	}
	if (filename && *filename != '\0') {
		if (text_end && text_end > text_start) {
			char text[512];
			size_t text_pos = 0;
			const char* text_ptr = text_start;
			if (*text_ptr == '"') text_ptr++;
			while (text_ptr < text_end && text_pos < sizeof(text) - 1) {
				if (*text_ptr != '"') {
					text[text_pos++] = *text_ptr;
				}
				text_ptr++;
			}
			text[text_pos] = '\0';
			if (fs_write_file_at_path(filename, text, text_pos)) {
			} else {
				brew_str("Error: Could not write to file\n");
			}
		} else {
			brew_str("Usage: ECHO \"text\" > <file>\n");
		}
	} else {
		const char* print_ptr = args;
		if (*print_ptr == '"') print_ptr++;
		while (*print_ptr) {
			if (*print_ptr == '"' && print_ptr > args) {
				break;
			} else if (*print_ptr != '"') {
				print_char(*print_ptr);
			}
			print_ptr++;
		}
		brew_str("\n");
	}
}

static void handle_ls(const char* command_buffer) {
	brew_str("\n");
	if (brew_strlen_cli(command_buffer) > 3) {
		const char* path = &command_buffer[3];
		if (!fs_list_directory_at_path(path)) {
			brew_str("Directory not found: ");
			brew_str(path);
			brew_str("\n");
		}
	} else {
		fs_list_directory();
	}
}

static void handle_cat(const char* command_buffer) {
	if (brew_strlen_cli(command_buffer) <= 4) {
		brew_str("\nUsage: CAT <file>\n");
		return;
	}
	const char* path = &command_buffer[4];
	while (*path == ' ') path++;
	if (*path == '\0') {
		brew_str("Usage: CAT <file>\n");
		return;
	}
	size_t size = 0;
	const char* content = fs_read_file_at_path(path, &size);
	if (!content || size == 0) {
		brew_str("Error: Could not read file or file is empty\n");
	} else {
		brew_str("\n");
		for (size_t i = 0; i < size; i++) {
			print_char(content[i]);
		}
		brew_str("\n");
	}
}

static void handle_touch(const char* command_buffer) {
	if (brew_strlen_cli(command_buffer) <= 6) {
		brew_str("\nUsage: TOUCH <file>\n");
		return;
	}
	const char* path = &command_buffer[6];
	while (*path == ' ') path++;
	brew_str("\n");
	if (*path == '\0') {
		brew_str("Usage: TOUCH <file>\n");
		return;
	}
	if (fs_create_file_at_path(path)) {
	} else {
		brew_str("Error: Could not create file\n");
	}
}

static void handle_rm(const char* command_buffer) {
	if (brew_strlen_cli(command_buffer) <= 3) {
		brew_str("\nUsage: RM <file_or_directory>\n");
		return;
	}
	const char* path = &command_buffer[3];
	while (*path == ' ') path++;
	brew_str("\n");
	if (*path == '\0') {
		brew_str("Usage: RM <file_or_directory>\n");
	} else if (!fs_remove_file(path)) {
	} else {
		brew_str("Successfully removed ");
		brew_str(path);
		brew_str("\n");
	}
}

static void handle_cd_usage(void) {
	brew_str("\nUsage: CD <directory>\n");
}

static void handle_cd(const char* command_buffer) {
	const char* path = &command_buffer[3];
	brew_str("\n");
	if (!fs_change_directory(path)) {
		brew_str("Directory not found: ");
		brew_str(path);
		brew_str("\n");
	}
}

static void handle_pwd(void) {
	brew_str("\n");
	fs_print_working_directory();
}

static void handle_mkdir(const char* command_buffer) {
	if (brew_strlen_cli(command_buffer) <= 6) {
		brew_str("\nUsage: MKDIR <directory> [directory ...]\n");
		return;
	}
	char* args[16];
	char cmd_copy[256];
	int i;
	for (i = 6; command_buffer[i]; i++) {
		cmd_copy[i-6] = command_buffer[i];
	}
	cmd_copy[i-6] = '\0';
	int arg_count = split_command_cli(cmd_copy, args, 16);
	if (arg_count == 0) {
		brew_str("\nUsage: MKDIR <directory> [directory ...]\n");
		return;
	}
	brew_str("\n");
	int all_success = 1;
	for (int k = 0; k < arg_count; k++) {
		if (!fs_create_directory_at_path(args[k])) {
			brew_str("Failed to create directory: ");
			brew_str(args[k]);
			brew_str("\n");
			all_success = 0;
		}
	}
	if (all_success) {
		brew_str("All directories created successfully\n");
	}
}

int shell_handle_command(const char* cmd_upper, char* command_buffer, int* return_to_prompt) {
	(void)return_to_prompt;
	if (strcmp_kernel_cli(cmd_upper, "PCISCAN") == 0) {
		handle_pciscan();
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "ECHO") == 0 || (brew_strlen_cli(cmd_upper) > 4 && strncmp_kernel_cli(cmd_upper, "ECHO ", 5) == 0)) {
		handle_echo(command_buffer);
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "LS") == 0 || (brew_strlen_cli(cmd_upper) > 2 && strncmp_kernel_cli(cmd_upper, "LS ", 3) == 0)) {
		handle_ls(command_buffer);
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "CAT") == 0 || (brew_strlen_cli(cmd_upper) > 3 && strncmp_kernel_cli(cmd_upper, "CAT ", 4) == 0)) {
		handle_cat(command_buffer);
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "TOUCH") == 0 || (brew_strlen_cli(cmd_upper) > 5 && strncmp_kernel_cli(cmd_upper, "TOUCH ", 6) == 0)) {
		handle_touch(command_buffer);
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "RM") == 0 || (brew_strlen_cli(cmd_upper) > 2 && strncmp_kernel_cli(cmd_upper, "RM ", 3) == 0)) {
		handle_rm(command_buffer);
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "CD") == 0) {
		handle_cd_usage();
		return 1;
	}
	if (brew_strlen_cli(cmd_upper) > 3 && strncmp_kernel_cli(cmd_upper, "CD ", 3) == 0) {
		handle_cd(command_buffer);
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "PWD") == 0) {
		handle_pwd();
		return 1;
	}
	if (strcmp_kernel_cli(cmd_upper, "MKDIR") == 0 || (brew_strlen_cli(cmd_upper) > 5 && strncmp_kernel_cli(cmd_upper, "MKDIR ", 6) == 0)) {
		handle_mkdir(command_buffer);
		return 1;
	}
	return 0;
}


