/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    app_sniffer
 * @brief       Sniffer application based on the new network stack
 * @{
 *
 * @file
 * @brief       Sniffer application for RIOT
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc.h"

/**
 * @brief   Buffer size used by the shell
 */
#define SHELL_BUFSIZE           (64U)

/**
 * @brief   Priority of the RAW dump thread
 */
#define RAWDUMP_PRIO            (THREAD_PRIORITY_MAIN - 1)

/**
 * @brief   Stack for the raw dump thread
 */
static char rawdmp_stack[THREAD_STACKSIZE_MAIN];

/**
 * @brief   Make a raw dump of the given packet contents
 */
void dump_pkt(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *snip = pkt;

    printf("rftest-rx --- len 0x%02x lqi 0x%02x rx_time 0x%08" PRIx64 "\n\n",
           gnrc_pkt_len(pkt), 0, xtimer_now64());

    while (snip) {
        for (size_t i = 0; i < snip->size; i++) {
            printf("0x%02x ", ((uint8_t *)(snip->data))[i]);
        }
        snip = snip->next;
    }
    puts("\n");

    gnrc_pktbuf_release(pkt);
}

/**
 * @brief   Event loop of the RAW dump thread
 *
 * @param[in] arg   unused parameter
 */
void *rawdump(void *arg)
{
    (void)arg;
    msg_t msg;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                dump_pkt((gnrc_pktsnip_t *)msg.content.ptr);
                break;
            default:
                /* do nothing */
                break;
        }
    }

    /* never reached */
    return NULL;
}

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    gnrc_netreg_entry_t dump;

    puts("RIOT sniffer application");

    /* start and register rawdump thread */
    puts("Run the rawdump thread and register it");
    dump.pid = thread_create(rawdmp_stack, sizeof(rawdmp_stack), RAWDUMP_PRIO,
                             CREATE_STACKTEST, rawdump, NULL, "rawdump");
    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* start the shell */
    puts("All ok, starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
