/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "cpu_conf.h"
#include "xtimer.h"

#include <time.h>
#include <sys/time.h>

#define MAIN_QUEUE_SIZE     (20)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_cmd(int argc, char **argv);

int timer_sleep(__attribute__((unused)) int argc,
        __attribute__((unused)) char **argv)
{
    xtimer_sleep(1);
    return 0;
}

extern char ajdi[CPUID_ID_LEN];

static const shell_command_t shell_commands[] = {{"udp",
        "send data over UDP and listen on UDP ports", udp_cmd}, {"timer",
        "sleep loop", timer_sleep}, {NULL, NULL, NULL}};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("\n\tRIOT network stack example application\n");

    struct timeval t0, t1, t2, t3, t4;
    gettimeofday(&t0, 0);

    printf("Time %d, %d\n", (int)t0.tv_sec, (int)t0.tv_usec);

    xtimer_sleep(2);
    /* ... */
    gettimeofday(&t1, 0);
    printf("Time %d, %d\n", (int)t1.tv_sec, (int)t1.tv_usec);

    gettimeofday(&t1, 0);
    printf("Time %d, %d\n", (int)t1.tv_sec, (int)t1.tv_usec);

    gettimeofday(&t1, 0);
    printf("Time %d, %d\n", (int)t1.tv_sec, (int)t1.tv_usec);

    gettimeofday(&t1, 0);
    printf("Time %d, %d\n", (int)t1.tv_sec, (int)t1.tv_usec);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
