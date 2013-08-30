/*
 * sc_heap.c - the handler of the heap state on the command shell.
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
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author  Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @note    $Id: sc_heap.c  3854 2013-05-24 17:53:01Z zkasmi $
 */

extern void heap_stats(void);

void _heap_handler(char* unused){
    (void) unused;
    heap_stats();
}

