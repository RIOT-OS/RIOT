/*
 * Copyright (C) 2015 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Basic ccn-lite relay example (produce and consumer via shell)
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

/* TODO: currently TLSF has to be disabled for native because of its stricter
 *       CFLAGS (-pedantic) */
#ifndef BOARD_NATIVE
#   include "tlsf-malloc.h"
#endif
#include "msg.h"
#include "shell.h"
#include "ccn-lite-riot.h"

/* main thread's message queue */
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#ifndef BOARD_NATIVE
    /* some buffer for the heap */
#   define TLSF_BUFFER     (10240 / sizeof(uint32_t))
    static uint32_t _tlsf_heap[TLSF_BUFFER];
#endif

int main(void)
{
#ifndef BOARD_NATIVE
    tlsf_create_with_pool(_tlsf_heap, sizeof(_tlsf_heap));
#endif
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Basic CCN-Lite example");

    ccnl_core_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
