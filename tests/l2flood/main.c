/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdbool.h>
#include <stdio.h>

#include "net/ng_netbase.h"
#include "shell.h"
#include "board_uart0.h"
#include "posix_io.h"
#include "timex.h"
#include "vtimer.h"

static inline bool _is_iface(kernel_pid_t dev)
{
    kernel_pid_t ifs[NG_NETIF_NUMOF];
    size_t numof = ng_netif_get(ifs);

    for (size_t i = 0; i < numof; i++) {
        if (ifs[i] == dev) {
            return true;
        }
    }

    return false;
}

static int l2flood(int argc, char **argv)
{
    kernel_pid_t iface;
    uint8_t addr[NG_NETIF_HDR_L2ADDR_MAX_LEN];
    size_t addr_len;
    timex_t delay = { 0, 0 };
    uint32_t num = 0, max_num;
    uint8_t flags = 0x00;

    if (argc < 4) {
        printf("usage: %s <if> {<addr>|bcast} <num> [<delay>] [<extra_data>]\n", argv[0]);
        return 1;
    }

    /* parse interface */
    iface = (kernel_pid_t)atoi(argv[1]);

    if (!_is_iface(iface)) {
        puts("error: invalid interface given");
        return 1;
    }

    /* parse address */
    addr_len = ng_netif_addr_from_str(addr, sizeof(addr), argv[2]);

    if (addr_len == 0) {
        if (strcmp(argv[2], "bcast") == 0) {
            flags |= NG_NETIF_HDR_FLAGS_BROADCAST;
        }
        else {
            puts("error: invalid address given");
            return 1;
        }
    }

    max_num = atoi(argv[3]);

    if (max_num == 0) {
        printf("usage: %s <if> {<addr>|bcast} <num> [<delay>] [<extra_data>]\n", argv[0]);
        return 1;
    }

    if (argc > 4) {
        delay.microseconds = (uint32_t)atoi(argv[4]);
        timex_normalize(&delay);
    }

    for (num = 0; num < max_num; num++) {
        ng_netif_hdr_t *nethdr;
        ng_pktsnip_t *pkt = NULL;
        /* put packet together */
        if (argc > 5) {
            pkt = ng_pktbuf_add(NULL, argv[5], strlen(argv[5]), NG_NETTYPE_UNDEF);
        }

        pkt = ng_pktbuf_add(pkt, &num, sizeof(uint32_t), NG_NETTYPE_UNDEF);
        pkt = ng_pktbuf_add(pkt, NULL, sizeof(ng_netif_hdr_t) + addr_len,
                            NG_NETTYPE_NETIF);
        nethdr = (ng_netif_hdr_t *)pkt->data;
        ng_netif_hdr_init(nethdr, 0, addr_len);
        ng_netif_hdr_set_dst_addr(nethdr, addr, addr_len);
        nethdr->flags = flags;
        /* and send it */
        ng_netapi_send(iface, pkt);
        vtimer_sleep(delay);
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "l2flood", "send multiple L2 packets", l2flood },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;

    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    /* should be never reached */
    return 0;
}
