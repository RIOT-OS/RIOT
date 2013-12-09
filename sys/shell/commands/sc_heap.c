/*
 * sc_heap.c - the handler of the heap state on the command shell.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @file
 * @internal
 * @brief   Show the heap state for the LPC2387 on the command shell.
 *
 * @author  Freie Universität Berlin, Computer Systems & Telematics
 * @author  Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @note    $Id: sc_heap.c 3855 2013-09-05 12:40:11 kasmi $
 */

#ifdef MODULE_LPC_COMMON

extern void heap_stats(void);

void _heap_handler(char *unused)
{
    (void) unused;
    heap_stats();
}

#endif

