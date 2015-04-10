/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "shell_print.h"
#include "shell_colors.h"

#define printentry(title, text) \
    printf("      "); \
    printfc(SHELL_STYLE_BOLD, title); \
    printf(text); \
    printf("\n");

void readline_print_keyboard(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    printfcs(SHELL_COLOR_GREEN, SHELL_STYLE_BOLD, "  READLINE KEYBOARD ");
    printfc(SHELL_COLOR_GREEN, "(bash/zsh compatible)\n");

    printfcs(SHELL_COLOR_RED, SHELL_STYLE_BOLD, "    CURSOR\n");
    printentry("CTRL+LEFT", "   Skip previous word");
    printentry("ALT+B", "       Skip previous word");
    printentry("CTRL+RIGHT", "  Skip next word");
    printentry("ALT+F", "       Skip next word");
    printentry("CTRL+A", "      HOME");
    printentry("CTRL+E", "      END");
    printentry("CTRL+B", "      Back");
    printentry("CTRL+F", "      Forward");

    printfcs(SHELL_COLOR_RED, SHELL_STYLE_BOLD, "    EDITING\n");
    printentry("CTRL+DEL", "    Delete next word");
    printentry("ALT+D", "       Cut next word");
    printentry("CTRL+D", "      DEL");
    printentry("CTRL+K", "      Cut behind cursor");
    printentry("CTRL+U", "      Cut line");
    printentry("CTRL+W", "      Cut previous word");
    printentry("ESC+BS", "      Cut previous word");
    printentry("CTRL+Y", "      Paste");

    printfcs(SHELL_COLOR_RED, SHELL_STYLE_BOLD, "   HISTORY\n");
    printentry("UP/DOWN", "     Cycle history");
    printentry("CTRL+N", "      Next");
    printentry("CTRL+P", "      Previous");
}
