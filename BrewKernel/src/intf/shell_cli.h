/*
 * Brew Kernel
 */
#ifndef SHELL_CLI_H
#define SHELL_CLI_H

// Returns 1 if handled, 0 otherwise. May modify *return_to_prompt (1/0).
int shell_handle_command(const char* cmd_upper, char* command_buffer, int* return_to_prompt);

#endif // SHELL_CLI_H


