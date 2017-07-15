/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2018 Hamburg University of Applied Sciences
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
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "xtimer.h"
#include "net/ipv6.h"

#include "opendefs.h"
#include "scheduler.h"
#include "02a-MAClow/IEEE802154E.h"
#include "03b-IPv6/icmpv6rpl.h"
#include "04-TRAN/openudp.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

extern udp_resource_desc_t uinject_vars;
extern idmanager_vars_t idmanager_vars;

static uint16_t counter = 0;

OpenQueueEntry_t *pkt;

void push_pkt_cb(void){
    owerror_t ret = openudp_send(pkt);
    if (ret == E_FAIL) {
        puts("could not send");
    }
}

static int udp_send(char *addr_str, char *port_str, char *data, unsigned int num,
                    unsigned int delay)
{
    size_t data_len;
    ipv6_addr_t addr;

    /* don't run if not in synch */
    if (ieee154e_isSynch() == FALSE) {
        puts("Error: Node not in synch, exit");
        return 1;
    }

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }

    data_len = strlen(data);
    uint8_t asnArray[data_len];

    /* get a free packet buffer */
    pkt = openqueue_getFreePacketBuffer(COMPONENT_UINJECT);
    if (pkt == NULL) {
        puts("Erro: could not create packet buffer");
        return 1;
    }

    /* change resource port so "send done" callback can be triggered;
     * this is dangerous because it's the destination port */
    uinject_vars.port = atoi(port_str);

    pkt->owner = COMPONENT_UINJECT;
    pkt->creator = COMPONENT_UINJECT;
    pkt->l4_protocol = IANA_UDP;
    pkt->l4_destination_port = atoi(port_str);
    pkt->l4_sourcePortORicmpv6Type = atoi(port_str);     // TODO
    pkt->l3_destinationAdd.type = ADDR_128B;
    memcpy(&pkt->l3_destinationAdd.addr_128b[0], (void *)&addr, 16);
    /* add payload */
    packetfunctions_reserveHeaderSize(pkt, data_len);
    memcpy(&pkt->payload[0], data, data_len);

    packetfunctions_reserveHeaderSize(pkt, sizeof(uint16_t));
    pkt->payload[1] = (uint8_t)((counter & 0xff00) >> 8);
    pkt->payload[0] = (uint8_t)(counter & 0x00ff);
    counter++;

    packetfunctions_reserveHeaderSize(pkt, sizeof(asn_t));
    ieee154e_getAsn(asnArray);
    pkt->payload[0] = asnArray[0];
    pkt->payload[1] = asnArray[1];
    pkt->payload[2] = asnArray[2];
    pkt->payload[3] = asnArray[3];
    pkt->payload[4] = asnArray[4];

    for (unsigned int i = 0; i < num; i++) {
        printf("Send %u byte over UDP to [%s]:%s\n",
                (unsigned)data_len, addr_str, port_str);
        scheduler_push_task(push_pkt_cb, TASKPRIO_COAP);
        xtimer_usleep(delay);
    }
    return 0;
}

int udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000;
        /* don't send as root */
        if (idmanager_vars.isDAGroot) {
            puts("Error: Node is root, exit");
            return 1;
        }
        if (argc < 5) {
            printf("usage: %s send <addr> <port> <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = atoi(argv[5]);
        }
        if (argc > 6) {
            delay = atoi(argv[6]);
        }
        return udp_send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return 1;
            }
            uint16_t port = atoi(argv[3]);
            uinject_vars.port = port;
            printf("Set UDP server port to %" PRIu16 "\n", port);
            return 0;
        }
        else {
            puts("error: invalid command");
            return 1;
        }
    }
    else {
        puts("error: invalid command");
        return 1;
    }
}
/** @} */
