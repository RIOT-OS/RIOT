/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José I. Álamos <jose.alamos@haw-hamburg.de>
 */
#include "assert.h"
#include "opendsme/DSMEMessage.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#include "opendsme/opendsme.h"

namespace dsme {

void DSMEMessage::prependFrom(DSMEMessageElement *msg)
{
    gnrc_pktsnip_t *head = gnrc_pktbuf_add(pkt, NULL,
                                           msg->getSerializationLength(), GNRC_NETTYPE_UNDEF);

    /* Hardcorded to O-QPSK */
    DSME_ASSERT(msg->getSerializationLength() <= IEEE802154_FRAME_LEN_MAX);
    DSME_ASSERT(head);
    int res = gnrc_pktbuf_merge(head);

    DSME_ASSERT(res == 0);
    DSME_ASSERT(gnrc_pkt_len(head) <= IEEE802154_FRAME_LEN_MAX);
    pkt = head;
    assert(head);
    Serializer s((uint8_t *)head->data, SERIALIZATION);

    msg->serialize(s);
}

void DSMEMessage::decapsulateTo(DSMEMessageElement *me)
{
    this->copyTo(me);
    this->dropHdr(me->getSerializationLength());
}

void DSMEMessage::copyTo(DSMEMessageElement *msg)
{
    Serializer s(this->getPayload(), DESERIALIZATION);

    msg->serialize(s);
    DSME_ASSERT(this->getPayload() + msg->getSerializationLength() == s.getData());
}

uint8_t DSMEMessage::getMPDUSymbols()
{
    /* Not used by OpenDSME */
    assert(false);
    return 0;
}

int DSMEMessage::loadBuffer(size_t len)
{
    int res = -ENOBUFS;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, len, GNRC_NETTYPE_UNDEF);

    DSME_ASSERT(len <= IEEE802154_FRAME_LEN_MAX);
    if (pkt == NULL) {
        DSME_ASSERT(false);
        goto end;
    }
    this->pkt = pkt;
    res = 0;

end:
    return res;
}

int DSMEMessage::loadBuffer(iolist_t *pkt)
{
    int res = -ENOBUFS;

    if (pkt == NULL) {
        DSME_ASSERT(false);
        goto end;
    }
    this->pkt = (gnrc_pktsnip_t *)pkt;
    res = 0;

end:
    return res;
}

int DSMEMessage::dropHdr(size_t len)
{
    gnrc_pktsnip_t *hdr = gnrc_pktbuf_mark(this->pkt, len, GNRC_NETTYPE_UNDEF);

    if (!hdr) {
        DSME_ASSERT(false);
        return -EINVAL;
    }
    gnrc_pktbuf_remove_snip(pkt, hdr);
    return 0;
}

void DSMEMessage::releaseMessage()
{
    DSME_ASSERT(!free);
    if (pkt) {
        gnrc_pktbuf_release(pkt);
    }
    free = true;
    delete this;
}

iolist_t *DSMEMessage::getIolPayload()
{
    return (iolist_t *)pkt;
}

void DSMEMessage::clearMessage()
{
    pkt = NULL;
    free = false;
    prepare();
}

void DSMEMessage::dispatchMessage()
{
    DSME_ASSERT(!free);
    uint16_t addr = getHeader().getSrcAddr().getShortAddress();
    uint8_t _addr[IEEE802154_SHORT_ADDRESS_LEN];
    _addr[0] = addr >> 8;
    _addr[1] = addr & 0xFF;

    uint16_t dst_addr = getHeader().getDestAddr().getShortAddress();
    uint8_t _dst_addr[IEEE802154_SHORT_ADDRESS_LEN];
    _dst_addr[0] = dst_addr >> 8;
    _dst_addr[1] = dst_addr & 0xFF;

    gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build((uint8_t *)_addr, IEEE802154_SHORT_ADDRESS_LEN,
                                                     (uint8_t *)_dst_addr,
                                                     IEEE802154_SHORT_ADDRESS_LEN);
    size_t mhr_len = ieee802154_get_frame_hdr_len(static_cast<uint8_t *>(pkt->data));

    pkt->type = CONFIG_OPENDSME_GNRC_PKTSNIP_TYPE;
    gnrc_netif_hdr_t *hdr = static_cast<gnrc_netif_hdr_t *>(netif_hdr->data);

    gnrc_netif_hdr_set_netif(hdr, this->netif);
    pkt = gnrc_pkt_append(pkt, netif_hdr);
    if (gnrc_netapi_dispatch_receive(CONFIG_OPENDSME_GNRC_PKTSNIP_TYPE, GNRC_NETREG_DEMUX_CTX_ALL,
                                     pkt)) {
        /* Pass packet to GNRC */
        pkt = NULL;
    }
    releaseMessage();
}
}

/** @} */
