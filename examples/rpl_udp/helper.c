/*
 * Copyright (C) 2013, 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief UDP RPL example application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg.h"
#include "sixlowpan/ip.h"
#include "transceiver.h"
#include "ieee802154_frame.h"
#include "rpl/rpl_structs.h"

#include "rpl_udp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define LL_HDR_LEN  (0x4)
#define IPV6_HDR_LEN    (0x28)

extern uint8_t ipv6_ext_hdr_len;

msg_t msg_q[RCV_BUFFER_SIZE];

void rpl_udp_set_id(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s address\n", argv[0]);
        printf("\taddress must be an 8 bit integer\n");
        printf("\n\t(Current address is %u)\n", id);
        return;
    }

    id = atoi(argv[1]);

    printf("Set node ID to %u\n", id);
}

void *rpl_udp_monitor(void *arg)
{
    (void) arg;

    msg_t m;
    radio_packet_t *p;
    ipv6_hdr_t *ipv6_buf;
    uint8_t icmp_type, icmp_code;
    icmpv6_hdr_t *icmpv6_buf = NULL;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;

            DEBUGF("Received packet from ID %u\n", p->src);
            DEBUG("\tLength:\t%u\n", p->length);
            DEBUG("\tSrc:\t%u\n", p->src);
            DEBUG("\tDst:\t%u\n", p->dst);
            DEBUG("\tLQI:\t%u\n", p->lqi);
            DEBUG("\tRSSI:\t%i\n", (int8_t) p->rssi);

            for (uint8_t i = 0; i < p->length; i++) {
                DEBUG("%02X ", p->data[i]);
            }

            p->processing--;
            DEBUG("\n");
        }
        else if (m.type == IPV6_PACKET_RECEIVED) {
            ipv6_buf = (ipv6_hdr_t *) m.content.ptr;
            printf("IPv6 datagram received (next header: %02X)", ipv6_buf->nextheader);
            printf(" from %s ", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                                 &ipv6_buf->srcaddr));

            if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
                icmpv6_buf = (icmpv6_hdr_t *) &ipv6_buf[(LL_HDR_LEN + IPV6_HDR_LEN) + ipv6_ext_hdr_len];
                icmp_type = icmpv6_buf->type;
                icmp_code = icmpv6_buf->code;
            }

            if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
                DEBUG("\t ICMP type: %02X ", icmp_type);
                DEBUG("\t ICMP code: %02X ", icmp_code);
                (void) icmp_type;
                (void) icmp_code;
            }

            printf("\n");
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            printf("Unknown packet received, type %04X\n", m.type);
        }
    }

    return NULL;
}

transceiver_command_t tcmd;

void rpl_udp_ignore(int argc, char **argv)
{
    uint16_t a;

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not runnning.");
        return;
    }

    msg_t mesg;
    mesg.type = DBG_IGN;
    mesg.content.ptr = (char *) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &a;

    if (argc == 2) {
        /* cppcheck: a is actually read via tcmd.data */
        /* cppcheck-suppress unreadVariable */
        a = atoi(argv[1]);
        printf("sending to transceiver (%" PRIkernel_pid "): %u\n", transceiver_pid, (*(uint8_t *)tcmd.data));
        msg_send(&mesg, transceiver_pid);
    }
    else {
        printf("Usage: %s <addr>\n", argv[0]);
    }
}
