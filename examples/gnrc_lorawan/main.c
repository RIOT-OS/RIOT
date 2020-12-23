/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test application for GNRC LoRaWAN
 *
 * @author      José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"

#include "board.h"

#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netreg.h"
#include "net/loramac.h"
#include "net/gnrc/tx_sync.h"

#define LORAWAN_QUEUE_SIZE (4U)

static void _usage(void)
{
    puts("usage: send <if_pid> <payload> [port]");
}

int tx_cmd(int argc, char **argv)
{
    gnrc_pktsnip_t *pkt;
    uint8_t port = CONFIG_LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
    int interface;
    gnrc_tx_sync_t tx_sync;

    if (argc < 3) {
        _usage();
        return 1;
    }

    interface = atoi(argv[1]);
    /* handle optional parameters */
    if (argc > 3) {
        port = atoi(argv[3]);
        if (port == 0 || port >= 224) {
            printf("error: invalid port given '%d', "
                   "port can only be between 1 and 223\n", port);
            return 1;
        }
    }

    pkt = gnrc_tx_sync_build(&tx_sync);
    pkt = gnrc_pktbuf_add(pkt, argv[2], strlen(argv[2]), GNRC_NETTYPE_UNDEF);

    gnrc_netapi_set(interface, NETOPT_LORAWAN_TX_PORT, 0, &port, sizeof(port));
    gnrc_netif_send(gnrc_netif_get_by_pid(interface), pkt);

    /* wait for packet status and check */
    gnrc_tx_sync(&tx_sync);
    uint32_t err = gnrc_neterr_get(&tx_sync);
    if (err != GNRC_NETERR_SUCCESS) {
        printf("Error sending packet: (status: %" PRIu32 "\n)", err);
    }
    else {
        puts("Successfully sent packet");
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "send",       "Send LoRaWAN data",     tx_cmd },
    { NULL, NULL, NULL }
};


int main(void)
{
    /* start the shell */
    puts("Initialization successful - starting the shell now");
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(CONFIG_LORAMAC_DEFAULT_TX_PORT,
                                                          gnrc_pktdump_pid);

    gnrc_netreg_register(GNRC_NETTYPE_LORAWAN, &dump);
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
