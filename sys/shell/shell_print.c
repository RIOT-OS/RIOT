/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "shell_print.h"
#include "shell.h"

#if USE_SHELL_COLORS
#include "shell_colors.h"
#endif // USE_SHELL_COLORS

inline void shell_print(shell_t *shell, const char *string)
{
    while (*string) {
        shell_putchar(shell, *string);
        string++;
    }
}

inline void shell_echo_string(shell_t *shell, const char *string)
{
#if USE_SHELL_COLORS
    shell_print(shell, SHELL_COLOR_ECHO);
    shell_print(shell, string);
    shell_print(shell, SHELL_COLOR_MAIN);
#else
    shell_print(shell, string);
#endif
}

inline void shell_print_error(shell_t *shell, const char *string)
{
#if USE_SHELL_COLORS
    shell_print(shell, SHELL_COLOR_ERROR);
    shell_print(shell, string);
    shell_print(shell, SHELL_COLOR_MAIN);
#else
    shell_print(shell, string);
#endif
}

inline void shell_echo_char(shell_t *shell, char c)
{
#if USE_SHELL_COLORS
    shell_print(shell, SHELL_COLOR_ECHO);
    shell_putchar(shell, c);
    shell_print(shell, SHELL_COLOR_MAIN);
#else
    shell_putchar(shell, c);
#endif
}

inline void shell_prompt(shell_t *shell)
{
#if USE_SHELL_COLORS
    shell_print(shell, SHELL_COLORED(">", SHELL_COLOR_PROMPT));
#else
    shell_putchar(shell, '>');
#endif
    shell_putchar(shell, ' ');
    return;
}

