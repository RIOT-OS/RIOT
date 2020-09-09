/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
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
 * @brief       OpenWSN test application
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "net/ieee802154.h"
#include "net/mac/sap.h"
#include "net/netif.h"

#include "openwsn.h"
#include "opendefs.h"

extern mac_t openwsn_mac;

/* Callback from MAC layer */
void mac_mcps_confirm(mac_t *mac, mcps_confirm_t *confirm)
{
    (void) mac;

    if(confirm->status == MAC_REQ_STATUS_SUCCESS) {
        puts("Successfully sent data");
    }
    else {
        puts("Error while sending packet");
    }
}

void mac_mcps_indication(mac_t *mac, mcps_indication_t *ind)
{
    (void) mac;
    char addr_str[3 * IEEE802154_LONG_ADDRESS_LEN];

    puts("Data received:");
    printf("\nSrc. addr.: %s\n",
           netif_addr_to_str(ind->data.addr, sizeof(ind->data.addr),
                             addr_str));
    printf("txt: ");
    for(unsigned i = 0; i < ind->data.len; i++) {
        printf("%c", ind->data.data[i]);
    }
    puts("");
}

int main(void)
{
    puts("OpenWSN MAC test");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
}
