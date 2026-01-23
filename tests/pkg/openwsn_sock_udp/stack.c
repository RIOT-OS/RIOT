/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "net/ipv6/addr.h"

#include "openwsn.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/openrandom.h"
#include "cross-layers/packetfunctions.h"
#include "cross-layers/idmanager.h"
#include "04-TRAN/udp.h"
#include "04-TRAN/sock_internal.h"
#include "02a-MAClow/IEEE802154E.h"
#include "scheduler.h"

#include "constants.h"

extern openqueue_vars_t openqueue_vars;
extern ieee154e_vars_t ieee154e_vars;
extern scheduler_vars_t scheduler_mock_vars;

extern void sock_udp_init(void);

static uint8_t *_get_udp_checksum(OpenQueueEntry_t *pkt)
{
    /* Using uintptr_t as intermediate cast to silence -Wcast-align. Since the
     * end result is of type `uint8_t *` (which has an alignment of 1 byte),
     * no unaligned memory accesses will occur here
     */
    return (uint8_t * )&(((udp_ht *)(uintptr_t)pkt->payload)->checksum);
}

bool _inject_packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                    uint16_t src_port, uint16_t dst_port,
                    void *data, size_t data_len, uint16_t netif)
{
    (void) netif;
    OpenQueueEntry_t *pkt;

    if ((pkt = openqueue_getFreePacketBuffer(COMPONENT_UDP_TO_SOCK)) == NULL) {
        return false;
    }

    pkt->owner = COMPONENT_UDP_TO_SOCK;
    pkt->creator = COMPONENT_UDP_TO_SOCK;

    /* set address*/
    pkt->l3_destinationAdd.type = ADDR_128B;
    pkt->l3_sourceAdd.type = ADDR_128B;
    memcpy(&pkt->l3_destinationAdd.addr_128b, dst, LENGTH_ADDR128b);
    memcpy(&pkt->l3_sourceAdd.addr_128b, src, LENGTH_ADDR128b);

    /* set port */
    pkt->l4_destination_port = dst_port;
    pkt->l4_sourcePortORicmpv6Type = src_port;

    /* set payload */
    if (packetfunctions_reserveHeader(&pkt, data_len)) {
        return false;
    }
    memcpy(pkt->payload, data, data_len);
    pkt->l4_payload = pkt->payload;
    pkt->l4_length = pkt->length;

    /* set remaining fields */
    pkt->l4_protocol_compressed = FALSE;
    pkt->l4_protocol = IANA_UDP;
    packetfunctions_reserveHeader(&pkt, sizeof(udp_ht));
    packetfunctions_htons(pkt->l4_sourcePortORicmpv6Type, &(pkt->payload[0]));
    packetfunctions_htons(pkt->l4_destination_port, &(pkt->payload[2]));
    packetfunctions_htons(pkt->length, &(pkt->payload[4]));
    packetfunctions_calculateChecksum(pkt, _get_udp_checksum(pkt));

    /* set ID to match destination */
    open_addr_t addr;
    addr.type = ADDR_PREFIX;
    memcpy(&addr.prefix, &dst->u8[0], sizeof(addr.prefix));
    idmanager_setMyID(&addr);
    addr.type = ADDR_64B;
    memcpy(&addr.addr_64b, &dst->u8[8], sizeof(addr.addr_64b));
    idmanager_setMyID(&addr);

    /* mock receive push pkt to mailbox */
    sock_receive_internal();

    return true;
}

void _net_init(void)
{
    openrandom_init();
    openqueue_init();
    sock_udp_init();
    scheduler_init();
    ieee154e_vars.isSync = true;
}

bool _check_net(void)
{
    /* queue must me empty */
    for (uint8_t i = 0; i < QUEUELENGTH; i++) {
        if (openqueue_vars.queue[i].owner != COMPONENT_NULL) {
            return false;
        }
    }
    return true;
}

bool _check_packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                   uint16_t src_port, uint16_t dst_port,
                   void *data, size_t data_len, uint16_t netif,
                   bool random_src_port)
{
    /* get packet from queue, there should be one! */
    OpenQueueEntry_t *pkt;

    pkt = openqueue_getPacketByComponent(COMPONENT_SOCK_TO_UDP);
    if (pkt == NULL) {
        return false;
    }

    /* check that UDP task was pushed */
    if (scheduler_mock_vars.last_task != TASKPRIO_UDP) {
        return false;
    }

    /* ignore checking the interface, there is only one possible interface*/
    (void)netif;

    /* check data matches, won't check the formed pkg its assumed that
       OpenWSN writes this correctly to the payload buffer */
    int ret = 0;
    ret =
        ((memcmp(src, &pkt->l3_sourceAdd.addr_128b,
                 sizeof(ipv6_addr_t)) ==  0) &&
         (memcmp(dst, &pkt->l3_destinationAdd.addr_128b,
                 sizeof(ipv6_addr_t)) ==  0) &&
         (dst_port == pkt->l4_destination_port) &&
         (random_src_port || (src_port == pkt->l4_sourcePortORicmpv6Type)) &&
         (memcmp(data, pkt->l4_payload, data_len) == 0));
    openqueue_freePacketBuffer(pkt);
    return ret;
}
