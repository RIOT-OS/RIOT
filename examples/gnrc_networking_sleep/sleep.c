/*
 * Copyright (C) 2020 ML!PA
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
 * @brief       Demonstrating low power operation with networking
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "periph/rtc.h"
#include "periph/pm.h"

#include "net/gnrc/netif.h"

static void _alarm_cb(void *arg)
{
    (void) arg;
    puts("The alarm rang.");
}

static void hibernate_interfaces(void)
{
    gnrc_netif_t* netif = NULL;
    netopt_state_t state = NETOPT_STATE_SLEEP;
    while ((netif = gnrc_netif_iter(netif))) {
        printf("put interface %d to sleep\n", netif->pid);
        gnrc_netapi_set(netif->pid, NETOPT_STATE, 0, &state, sizeof(netopt_state_t));
    }
}

int sleep_cmd(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <mode> <seconds>\n", argv[0]);
        return 1;
    }

    unsigned mode = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    hibernate_interfaces();

    printf("will enter power mode %d for %d s\n",
           mode, secs);

    struct tm now;
    rtc_get_time(&now);
    now.tm_sec += secs;
    rtc_set_alarm(&now, _alarm_cb, NULL);

    pm_set(mode);

    return 0;
}

extern void gnrc_netif_init(void);
int start_networking_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    gnrc_netif_init();

    return 0;
}
