/*
 * Copyright (C) 2025 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Text application for gnrc_legacy_tx_sync
 *
 * @author      Lukas Luger     <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/af.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/tx_sync.h"
#include "net/gnrc.h"
#include "ztimer.h"
#include "thread.h"

static const char test_msg[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU"
                               "VWXYZ0123456789.,:;!?@#$%^&*()[]{}-_=+/<>`~\'\""
                               "\\";

void print_failed(void *arg)
{
    (void)arg;
    puts("TEST FAILED");
}

int main(void)
{
    puts(
        "Test application for gnrc_legacy_tx_sync\n"
        "========================================\n"
        "\n"
        "This application sends a single message over the nrfmin netdev.\n"
        "Other legacy netdevs can be used by changing the board and using\n"
        "a different driver. Note: RIOT will sometimes choose the\n"
        "netdev_ieee802154_submac as default driver which is not legacy.\n"
        "tx_sync will only fail if gnrc_netif_pktq is used with a legacy driver.\n"
        "If tx_sync does not finish in one second, the test will fail.\n"
    );

    /* Preparing the gnrc message */
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, test_msg, sizeof(test_msg),
                                                        GNRC_NETTYPE_UNDEF);
    gnrc_tx_sync_t tx_sync;
    gnrc_tx_sync_append(pkt, &tx_sync);

    /* Set timeout message in case tx sync fails */
    ztimer_t timer = {
        .callback = print_failed,
        .arg = NULL,
    };
    ztimer_set(ZTIMER_SEC, &timer, 1);

    /* Sending and waiting */
    gnrc_netapi_send(netif->pid, pkt);
    gnrc_tx_sync(&tx_sync);

    /* Cancel error message and print success message */
    ztimer_remove(ZTIMER_SEC, &timer);
    puts("TEST PASSED");

    return 0;
}
