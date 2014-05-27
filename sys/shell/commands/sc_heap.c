/*
 * sc_heap.c - the handler of the heap state on the command shell.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @internal
 * @brief   Show the heap state for the LPC2387 on the command shell.
 *
 * @author  Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @note    $Id: sc_heap.c 3855 2013-09-05 12:40:11 kasmi $
 */

extern void heap_stats(void);

int _heap_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    heap_stats();

    return 0;
}
