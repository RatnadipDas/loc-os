#pragma once

/**
 * @brief Entry point for the user-space shell.
 *
 * This function implements a simple command-line shell that accepts and executes
 * predefined commands in an infinite loop. Supported commands include:
 *
 * - `hello`      : Prints a hello message from the shell.
 * - `readfile`   : Reads and prints the contents of "hello.txt".
 * - `writefile`  : Writes a predefined message to "hello.txt".
 * - `shutdown`   : Shuts down the system.
 * - `exit`       : Exits the shell process.
 *
 * The shell reads input character by character until a carriage return (`\r`)
 * is entered. It then parses the command and performs the corresponding action.
 * If an unknown command is entered, an error message is displayed.
 *
 * @note The command line is limited to 127 characters (plus null terminator).
 * @note Input and output are handled using `getchar()` and `putchar()`.
 */
void main(void);
