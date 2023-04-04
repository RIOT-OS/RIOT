/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "net/lora.h"
#include "net/gnrc/lorawan.h"
#include "net/gnrc/lorawan/region.h"
#include "errno.h"
#include "net/gnrc/pktbuf.h"

#include "net/lorawan/hdr.h"

#define ENABLE_DEBUG      0
#include "debug.h"

#define _16_UPPER_BITMASK 0xFFFF0000
#define _16_LOWER_BITMASK 0xFFFF

static int gnrc_lorawan_mic_is_valid(uint8_t *buf, size_t len, uint8_t *key,
                                     uint32_t fcnt_up, bool optneg)
{
    (void)fcnt_up;
    le_uint32_t calc_mic;

    lorawan_hdr_t *lw_hdr = (lorawan_hdr_t *)buf;

    uint32_t fcnt = byteorder_ltohs(lw_hdr->fcnt);
    iolist_t iol =
    { .iol_base = buf, .iol_len = len - MIC_SIZE, .iol_next = NULL };

    /* for LoRaWAN 1.0 conf_fcnt is hardcoded to be 0 */
    uint16_t conf_fnct = 0x00;

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && optneg) {
        /**
         * If the device is connected to a LoRaWAN 1.1 Network Server and the
         * ACK bit of the downlink frame is set, meaning this frame is acknowledging
         * an uplink “confirmed” frame,then ConfFCnt is the frame counter value
         * modulo 2^16 of the “confirmed” uplink frame that is being acknowledged.
         * In all other cases ConfFCnt = 0x0000.
         */
        if (lorawan_hdr_get_ack(lw_hdr)) {
            conf_fnct = fcnt_up;
        }
    }

    gnrc_lorawan_calculate_mic_downlink(&lw_hdr->addr, fcnt, conf_fnct, &iol,
                                        key, &calc_mic);

    return calc_mic.u32 == ((le_uint32_t *)(buf + len - MIC_SIZE))->u32;
}

uint32_t gnrc_lorawan_fcnt_stol(uint32_t fcnt_down, uint16_t s_fcnt)
{
    uint32_t u32_fcnt = (fcnt_down & _16_UPPER_BITMASK) | s_fcnt;

    if (fcnt_down + CONFIG_LORAMAC_DEFAULT_MAX_FCNT_GAP >= _16_LOWER_BITMASK
        && s_fcnt < (fcnt_down & _16_LOWER_BITMASK)) {
        u32_fcnt += _16_LOWER_BITMASK;
    }
    return u32_fcnt;
}

/**
 * @brief holder of parsed packet
 */
struct parsed_packet {
    uint32_t fcnt_down;         /**< frame counter */
    lorawan_hdr_t *hdr;         /**< pointer to the LoRaWAN header */
    bool ack_req;               /**< whether an ACK was requested or not */
    iolist_t fopts;             /**< iolist with Fopts information */
    iolist_t enc_payload;       /**< iolist with encrypted payload */
    uint8_t port;               /**< Fport of the packet */
    bool ack;                   /**< whether the ACK bit was set or not */
    bool frame_pending;         /**< whether there's pending data or not */
};

int gnrc_lorawan_parse_dl(gnrc_lorawan_t *mac, uint8_t *buf, size_t len,
                          struct parsed_packet *pkt)
{
    memset(pkt, 0, sizeof(struct parsed_packet));

    lorawan_hdr_t *_hdr = (lorawan_hdr_t *)buf;
    uint8_t *p_mic = buf + len - MIC_SIZE;

    pkt->hdr = _hdr;
    buf += sizeof(lorawan_hdr_t);

    /* Validate header */
    if (_hdr->addr.u32 != mac->dev_addr.u32) {
        DEBUG("gnrc_lorawan: received packet with wrong dev addr. Drop\n");
        return -1;
    }

    int fopts_length = lorawan_hdr_get_frame_opts_len(_hdr);

    if (fopts_length) {
        pkt->fopts.iol_base = buf;
        pkt->fopts.iol_len = fopts_length;
        buf += fopts_length;
    }

    if (buf < p_mic) {
        pkt->port = *(buf++);

        if (!pkt->port && fopts_length) {
            DEBUG("gnrc_lorawan: packet with fopts and port == 0. Drop\n");
            return -1;
        }

        if (pkt->port > LORAMAC_PORT_MAX) {
            DEBUG("gnrc_lorawan: packet with port > 223. Drop\n");
            return -1;
        }

        if (buf < p_mic) {
            pkt->enc_payload.iol_base = buf;
            pkt->enc_payload.iol_len = p_mic - buf;
        }
    }

    uint32_t _fcnt =
        gnrc_lorawan_fcnt_stol(mac->mcps.fcnt_down, _hdr->fcnt.u16);

    uint32_t fcnt_down = mac->mcps.fcnt_down;

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && pkt->port) {
        fcnt_down = gnrc_lorawan_get_afcnt_down(mac);
    }

    if (fcnt_down > _fcnt || fcnt_down + CONFIG_LORAMAC_DEFAULT_MAX_FCNT_GAP < _fcnt) {
        DEBUG("gnrc_lorawan: wrong frame counter\n");
        return -1;
    }

    pkt->fcnt_down = _fcnt;

    pkt->ack_req = lorawan_hdr_get_mtype(_hdr) == MTYPE_CNF_DOWNLINK;
    pkt->ack = lorawan_hdr_get_ack(_hdr);
    pkt->frame_pending = lorawan_hdr_get_frame_pending(_hdr);

    return 0;
}

void gnrc_lorawan_mcps_process_downlink(gnrc_lorawan_t *mac, uint8_t *psdu,
                                        size_t size)
{
    struct parsed_packet _pkt;

    /* NOTE: MIC is in pkt */
    if (!gnrc_lorawan_mic_is_valid(psdu, size, mac->ctx.snwksintkey,
                                   mac->mcps.fcnt, gnrc_lorawan_optneg_is_set(mac))) {
        DEBUG("gnrc_lorawan: invalid MIC\n");
        return;
    }

    if (gnrc_lorawan_parse_dl(mac, psdu, size, &_pkt) < 0) {
        DEBUG("gnrc_lorawan: couldn't parse packet\n");
        return;
    }

    iolist_t *fopts = NULL;

    if (_pkt.fopts.iol_base) {
        fopts = &_pkt.fopts;
    }

    if (_pkt.enc_payload.iol_base) {
        uint8_t *key;
        if (_pkt.port) {
            key = mac->ctx.appskey;
        }
        else {
            key = mac->ctx.nwksenckey;
            fopts = &_pkt.enc_payload;
        }
        gnrc_lorawan_encrypt_payload(&_pkt.enc_payload, &_pkt.hdr->addr,
                                     byteorder_ltohs(_pkt.hdr->fcnt),
                                     GNRC_LORAWAN_DIR_DOWNLINK,
                                     key);
    }

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && _pkt.port) {
        gnrc_lorawan_set_afcnt_down(mac, _pkt.fcnt_down);
    }
    else {
        mac->mcps.fcnt_down = _pkt.fcnt_down;
    }

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        gnrc_lorawan_set_last_fcnt_down(mac, _pkt.fcnt_down);
    }

    if (_pkt.ack_req) {
        mac->mcps.ack_requested = true;
    }

    /* if there are fopts, it's either an empty packet or application payload */
    if (fopts) {
        if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
            if (_pkt.port) {
                gnrc_lorawan_encrypt_fopts(fopts->iol_base, fopts->iol_len,
                                           &mac->dev_addr,
                                           gnrc_lorawan_get_afcnt_down(mac), true,
                                           GNRC_LORAWAN_DIR_DOWNLINK,
                                           mac->ctx.nwksenckey);
            }
            else {
                gnrc_lorawan_encrypt_fopts(fopts->iol_base, fopts->iol_len,
                                           &mac->dev_addr,
                                           mac->mcps.fcnt_down, false,
                                           GNRC_LORAWAN_DIR_DOWNLINK,
                                           mac->ctx.nwksenckey);
            }
        }
        DEBUG("gnrc_lorawan: processing fopts\n");
        gnrc_lorawan_process_fopts(mac, fopts->iol_base, fopts->iol_len);
    }

    /* In class C we do not need to request an uplink for frame pending,
     * as the server can send a downlink at any time */
    if ((_pkt.frame_pending && !IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C))
        || _pkt.ack_req) {
        mlme_indication_t mlme_indication;
        mlme_indication.type = MLME_SCHEDULE_UPLINK;
        gnrc_lorawan_mlme_indication(mac, &mlme_indication);
    }

    if (_pkt.port) {
        mcps_indication_t mcps_indication;
        mcps_indication.type = _pkt.ack_req;
        mcps_indication.data.pkt = &_pkt.enc_payload;
        mcps_indication.data.port = _pkt.port;
        gnrc_lorawan_mcps_indication(mac, &mcps_indication);
    }

    if (_pkt.ack) {
        /* The state machine will catch this in the WAIT_FOR_ACK event */
        mac->mcps.waiting_for_ack = false;
    }
}

size_t gnrc_lorawan_build_hdr(uint8_t mtype, le_uint32_t *dev_addr,
                              uint32_t fcnt, uint8_t ack, uint8_t fopts_length,
                              lorawan_buffer_t *buf)
{
    assert(fopts_length < 16);
    lorawan_hdr_t *lw_hdr = (lorawan_hdr_t *)buf->data;

    lw_hdr->mt_maj = 0;
    lorawan_hdr_set_mtype(lw_hdr, mtype);
    lorawan_hdr_set_maj(lw_hdr, MAJOR_LRWAN_R1);

    lw_hdr->addr = *dev_addr;
    lw_hdr->fctrl = 0;

    lorawan_hdr_set_ack(lw_hdr, ack);
    lorawan_hdr_set_frame_opts_len(lw_hdr, fopts_length);

    lw_hdr->fcnt = byteorder_htols(fcnt);

    buf->index += sizeof(lorawan_hdr_t);

    return sizeof(lorawan_hdr_t);
}

size_t gnrc_lorawan_build_uplink(gnrc_lorawan_t *mac, iolist_t *payload,
                                 int confirmed_data, uint8_t port)
{
    lorawan_buffer_t buf = {
        .data = (uint8_t *)mac->mcps.mhdr_mic,
        .size = sizeof(mac->mcps.mhdr_mic),
        .index = 0
    };
    lorawan_hdr_t *lw_hdr = (lorawan_hdr_t *)buf.data;

    lw_hdr->mt_maj = 0;
    lorawan_hdr_set_mtype(lw_hdr,
                          confirmed_data ? MTYPE_CNF_UPLINK : MTYPE_UNCNF_UPLINK);
    lorawan_hdr_set_maj(lw_hdr, MAJOR_LRWAN_R1);

    lw_hdr->addr = mac->dev_addr;
    lw_hdr->fctrl = 0;

    lorawan_hdr_set_ack(lw_hdr, mac->mcps.ack_requested);

    lw_hdr->fcnt = byteorder_htols(mac->mcps.fcnt);

    buf.index += sizeof(lorawan_hdr_t);

    size_t fopts_length = gnrc_lorawan_build_options(mac, &buf);

    assert(fopts_length < 16);
    lorawan_hdr_set_frame_opts_len(lw_hdr, fopts_length);

    buf.data[buf.index++] = port;

    uint8_t *key;

    if (port) {
        key = mac->ctx.appskey;
    }
    else {
        key = mac->ctx.nwksenckey;
    }

    gnrc_lorawan_encrypt_payload(payload, &mac->dev_addr, mac->mcps.fcnt,
                                 GNRC_LORAWAN_DIR_UPLINK, key);

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        key = mac->ctx.nwksenckey;

        gnrc_lorawan_encrypt_fopts(&buf.data[sizeof(lorawan_hdr_t)], fopts_length,
                                   &mac->dev_addr, mac->mcps.fcnt, false,
                                   GNRC_LORAWAN_DIR_UPLINK, key);
    }

    return buf.index;
}

void gnrc_lorawan_mcps_request(gnrc_lorawan_t *mac,
                               const mcps_request_t *mcps_request,
                               mcps_confirm_t *mcps_confirm)
{
    iolist_t *pkt = mcps_request->data.pkt;

    if (mac->mlme.activation == MLME_ACTIVATION_NONE) {
        DEBUG("gnrc_lorawan_mcps: LoRaWAN not activated\n");
        mcps_confirm->status = -ENOTCONN;
        return;
    }

    if (gnrc_lorawan_is_busy(mac)) {
        mcps_confirm->status = -EBUSY;
        return;
    }

    if (mcps_request->data.port < LORAMAC_PORT_MIN ||
        mcps_request->data.port > LORAMAC_PORT_MAX) {
        mcps_confirm->status = -EBADMSG;
        return;
    }

    if (!gnrc_lorawan_validate_dr(mcps_request->data.dr)) {
        mcps_confirm->status = -EINVAL;
        return;
    }

    uint8_t fopts_length = gnrc_lorawan_build_options(mac, NULL);
    /* We don't include the port because `MACPayload` doesn't consider
     * the MHDR...*/
    size_t mac_payload_size = sizeof(lorawan_hdr_t) + fopts_length +
                              iolist_size(pkt);

    if (mac_payload_size >
        gnrc_lorawan_region_mac_payload_max(mcps_request->data.dr)) {
        mcps_confirm->status = -EMSGSIZE;
        return;
    }

    int waiting_for_ack = mcps_request->type == MCPS_CONFIRMED;

    gnrc_lorawan_build_uplink(mac, pkt, waiting_for_ack,
                              mcps_request->data.port);

    mac->mcps.waiting_for_ack = waiting_for_ack;
    mac->mcps.ack_requested = false;

    mac->mcps.nb_trials = waiting_for_ack ? CONFIG_LORAMAC_DEFAULT_RETX : mac->mcps.redundancy;

    mac->mcps.msdu = pkt;
    mac->last_dr = mcps_request->data.dr;
    mcps_confirm->status = GNRC_LORAWAN_REQ_STATUS_DEFERRED;
    gnrc_lorawan_dispatch_event(mac, &mac->mac_fsm, GNRC_LORAWAN_EV_REQUEST_TX);
}

/** @} */
