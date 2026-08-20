/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython built-in help text
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "py/builtin.h"

const char riot_help_text[] =
    "Welcome to the MicroPython RIOT port!\n"
    "\n"
    "Quick overview of commands for the board:\n"
    "  (none so far)\n"
    "\n"
    "Control commands:\n"
    "  CTRL-A        -- on a blank line, enter raw REPL mode\n"
    "  CTRL-B        -- on a blank line, enter normal REPL mode\n"
    "  CTRL-C        -- interrupt a running program\n"
    "  CTRL-D        -- on a blank line, do a soft reset of the board\n"
    " (all probably not working in this initial port)\n"
    "\n"
    "For further help on a specific object, type help(obj)\n";
