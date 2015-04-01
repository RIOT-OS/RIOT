/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __SHELL_COLORS_H
#define __SHELL_COLORS_H

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SHELL_STYLE_BOLD                    1
#define SHELL_STYLE_DIM                     2
#define SHELL_STYLE_UNDERLINED              4
#define SHELL_STYLE_BLINK                   5
#define SHELL_STYLE_REVERSE                 7
#define SHELL_STYLE_HIDDEN                  8

#define SHELL_RESET_ALL                     0
#define SHELL_RESET_BOLD                    21
#define SHELL_RESET_DIM                     22
#define SHELL_RESET_UNDERLINED              24
#define SHELL_RESET_BLINK                   25
#define SHELL_RESET_REVERSE                 27
#define SHELL_RESET_HIDDEN                  28

#define SHELL_COLOR_DEFAULT                 39
#define SHELL_COLOR_BLACK                   30
#define SHELL_COLOR_RED                     31
#define SHELL_COLOR_GREEN                   32
#define SHELL_COLOR_YELLOW                  33
#define SHELL_COLOR_BLUE                    34
#define SHELL_COLOR_MAGENTA                 35
#define SHELL_COLOR_CYAN                    36
#define SHELL_COLOR_WHITE                   37

#define SHELL_BACKGROUND_DEFAULT            49
#define SHELL_BACKGROUND_BLACK              40
#define SHELL_BACKGROUND_RED                41
#define SHELL_BACKGROUND_GREEN              42
#define SHELL_BACKGROUND_YELLOW             43
#define SHELL_BACKGROUND_BLUE               44
#define SHELL_BACKGROUND_MAGENTA            45
#define SHELL_BACKGROUND_CYAN               46
#define SHELL_BACKGROUND_WHITE              47

#define xstr(s) str(s)
#define str(s) #s

#define SHELL_COLOR_ESC                     \x1b[

#ifndef SHELL_COLOR_MAIN
#define SHELL_COLOR_MAIN                    xstr(SHELL_COLOR_ESC) xstr(SHELL_RESET_ALL) "m"
#endif
#ifndef SHELL_COLOR_PROMPT
#define SHELL_COLOR_PROMPT                  SHELL_COLOR_STYLE(SHELL_COLOR_GREEN, SHELL_STYLE_BOLD)
#endif
#ifndef SHELL_COLOR_ECHO
#define SHELL_COLOR_ECHO                    SHELL_COLOR(SHELL_COLOR_BLUE)
#endif
#ifndef SHELL_COLOR_ERROR
#define SHELL_COLOR_ERROR                   SHELL_COLOR(SHELL_COLOR_RED)
#endif

#define SHELL_COLOR(color)                  xstr(SHELL_COLOR_ESC) str(color) "m"
#define SHELL_COLOR_STYLE(color, style)     xstr(SHELL_COLOR_ESC) str(color) ";" str(style) "m"
/* Construct a constant string that wrapps colors control around it */
#define SHELL_COLORED(str, color_string)    color_string str SHELL_COLOR_MAIN

#ifdef __cplusplus
}
#endif

#endif // __SHELL_COLORS_H
