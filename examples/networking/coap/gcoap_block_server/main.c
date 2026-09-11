/*
 * SPDX-FileCopyrightText: 2015-2016 Ken Bannister
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       gcoap example
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "net/gcoap.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern void gcoap_cli_init(void);

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_cli_init();
    puts("gcoap block handler");

    puts("All up, running the shell now");
    /* shell starts implicitly after returning */
    return 0;
}
