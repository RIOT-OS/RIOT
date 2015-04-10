/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SYS_SHELL_SHELL_PRINT_H_
#define SYS_SHELL_SHELL_PRINT_H_

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

#define shell_putchar(shell, c) shell->put_char(c)

extern void shell_print(shell_t *shell, const char *string);
extern void shell_echo_string(shell_t *shell, const char *string);
extern void shell_print_error(shell_t *shell, const char *string);
extern void shell_echo_char(shell_t *shell, char c);
extern void shell_prompt(shell_t *shell);


#if USE_SHELL_COLORS
#define printfc(color, ...) do { \
                                printf(SHELL_COLOR(color)); \
                                printf(__VA_ARGS__); \
                                printf(SHELL_COLOR_MAIN); \
                            } while(0);
#define printfs(color, ...) do { \
                                printf(SHELL_STYLE(color)); \
                                printf(__VA_ARGS__); \
                                printf(SHELL_COLOR_MAIN); \
                            } while(0);
#define printfcs(color, style, ...) do { \
                                printf(SHELL_COLOR_STYLE(color, style)); \
                                printf(__VA_ARGS__); \
                                printf(SHELL_COLOR_MAIN); \
                            } while(0);
#else
/* Printf with shell-color */
#define printfc(color, ...) printf(__VA_ARGS__)
/* Printf with shell-style */
#define printfs(color, ...) printf(__VA_ARGS__)
/* Printf with shell-style and shell_color*/
#define printfcs(color, style, ...) printf(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_SHELL_SHELL_PRINT_H_ */
