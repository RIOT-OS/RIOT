/*
 * Copyright (C) 2020 HAW
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Compile test for gnrc_rpl
 *
 * @author      Cenk Gündoğan <mail+dev@gundogan.net>
 *
 * @}
 */

#include <stdio.h>
#include "shell.h"
#include "msg.h"
#include "thread.h"
#include "net/gnrc/netif.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static char _uplink_mon_stack[THREAD_STACKSIZE_DEFAULT];

static void * _uplink_mon(void *arg)
{
    (void)arg;

    if (gnrc_netif_ipv6_wait_for_global_address(NULL, 10 * MS_PER_SEC)) {
        puts("uplink ready");
    } else {
        puts("uplink timeout");
    }

    return NULL;
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    thread_create(_uplink_mon_stack, sizeof(_uplink_mon_stack), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, _uplink_mon, NULL, "uplink monitor");

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
