#pragma once

/**
 * @brief ANSI escape codes for terminal text colors.
 *
 * These macros define ANSI escape sequences for setting text colors in terminal
 * output. The `L_` prefix represents the "light" or "bold" version of the
 * color.
 *
 * @note Use `NONE` to reset the text color to the default terminal color.
 * @note These escape codes work in terminals that support ANSI color sequences.
 *
 * @example
 * @code
 * printf(RED "This is red text" NONE "\n");         // Prints red text
 * printf(L_GREEN "This is bright green text" NONE "\n");
 * printf(BLUE "This is blue text" NONE "\n");
 * @endcode
 */
#define NONE "\e[0m"         ///< Reset text color to default
#define BLACK "\e[0;30m"     ///< Black text
#define L_BLACK "\e[1;30m"   ///< Light black (gray) text
#define RED "\e[0;31m"       ///< Red text
#define L_RED "\e[1;31m"     ///< Light red (bright red) text
#define GREEN "\e[0;32m"     ///< Green text
#define L_GREEN "\e[1;32m"   ///< Light green (bright green) text
#define YELLOW "\e[0;33m"    ///< Yellow text
#define L_YELLOW "\e[1;33m"  ///< Light yellow (bright yellow) text
#define BLUE "\e[0;34m"      ///< Blue text
#define L_BLUE "\e[1;34m"    ///< Light blue (bright blue) text
#define PURPLE "\e[0;35m"    ///< Purple (magenta) text
#define L_PURPLE "\e[1;35m"  ///< Light purple (bright magenta) text
#define CYAN "\e[0;36m"      ///< Cyan text
#define L_CYAN "\e[1;36m"    ///< Light cyan (bright cyan) text
#define GRAY "\e[0;37m"      ///< Gray text
#define WHITE "\e[1;37m"     ///< White (bright white) text
