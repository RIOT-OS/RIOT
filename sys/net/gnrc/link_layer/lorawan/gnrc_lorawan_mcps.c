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
#include <stdio.h>
#include <string.h>
#include "net/lora.h"
#include "net/gnrc/lorawan.h"
#include "net/gnrc/lorawan/region.h"
#include "errno.h"
#include "net/gnrc/pktbuf.h"

#include "net/lorawan/hdr.h"

#include "random.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _16_UPPER_BITMASK 0xFFFF0000
#define _16_LOWER_BITMASK 0xFFFF

int gnrc_lorawan_mic_is_valid(gnrc_pktsnip_t *mic, uint8_t *nwkskey)
{
    le_uint32_t calc_mic;

    assert(mic->size == MIC_SIZE);
    assert(mic->next->data);
    lorawan_hdr_t *lw_hdr = (lorawan_hdr_t *) mic->next->data;

    uint32_t fcnt = byteorder_ntohs(byteorder_ltobs(lw_hdr->fcnt));
    gnrc_lorawan_calculate_mic(&lw_hdr->addr, fcnt, GNRC_LORAWAN_DIR_DOWNLINK, (iolist_t *) mic->next, nwkskey, &calc_mic);
    return calc_mic.u32 == ((le_uint32_t *) mic->data)->u32;
}

uint32_t gnrc_lorawan_fcnt_stol(uint32_t fcnt_down, uint16_t s_fcnt)
{
    uint32_t u32_fcnt = (fcnt_down & _16_UPPER_BITMASK) | s_fcnt;

    if (fcnt_down + LORAMAC_DEFAULT_MAX_FCNT_GAP >= _16_LOWER_BITMASK
        && s_fcnt < (fcnt_down & _16_LOWER_BITMASK)) {
        u32_fcnt += _16_LOWER_BITMASK;
    }
    return u32_fcnt;
}

void gnrc_lorawan_mcps_process_downlink(gnrc_lorawan_t *mac, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *hdr, *data, *fopts = NULL, *fport = NULL;
    int release = true;
    int error = true;

    /* mark MIC */
    if (!(data = gnrc_pktbuf_mark(pkt, (pkt->size - MIC_SIZE > 0) ? pkt->size - MIC_SIZE : 0, GNRC_NETTYPE_UNDEF))) {
        DEBUG("gnrc_lorawan: failed to mark MIC\n");
        goto out;
    }

    /* NOTE: MIC is in pkt */
    if (!gnrc_lorawan_mic_is_valid(pkt, mac->nwkskey)) {
        DEBUG("gnrc_lorawan: invalid MIC\n");
        goto out;
    }

    /* remove snip */
    pkt = gnrc_pktbuf_remove_snip(pkt, pkt);

    if (!(hdr = gnrc_pktbuf_mark(pkt, sizeof(lorawan_hdr_t), GNRC_NETTYPE_UNDEF))) {
        DEBUG("gnrc_lorawan: failed to allocate hdr\n");
        goto out;
    }

    int _fopts_length = lorawan_hdr_get_frame_opts_len((lorawan_hdr_t *) hdr->data);
    if (_fopts_length && !(fopts = gnrc_pktbuf_mark(pkt, _fopts_length, GNRC_NETTYPE_UNDEF))) {
        DEBUG("gnrc_lorawan: failed to allocate fopts\n");
        goto out;
    }

    assert(pkt != NULL);

    int fopts_in_payload = 0;
    /* only for download frames with payload the FPort must be present */
    if (pkt->size) {
        if ((fport = gnrc_pktbuf_mark(pkt, 1, GNRC_NETTYPE_UNDEF)) == NULL) {
            DEBUG("gnrc_lorawan: failed to allocate fport\n");
            goto out;
        }

        assert(fport->data);

        fopts_in_payload = *((uint8_t *) fport->data) == 0;
        if (fopts && fopts_in_payload) {
            DEBUG("gnrc_lorawan: packet with fopts and port == 0. Drop\n");
            goto out;
        }
    }

    lorawan_hdr_t *lw_hdr = hdr->data;

    if (lw_hdr->addr.u32 != mac->dev_addr.u32) {
        DEBUG("gnrc_lorawan: received packet with wrong dev addr. Drop\n");
        goto out;
    }

    uint32_t fcnt = gnrc_lorawan_fcnt_stol(mac->mcps.fcnt_down, lw_hdr->fcnt.u16);
    if (mac->mcps.fcnt_down > fcnt || mac->mcps.fcnt_down +
        LORAMAC_DEFAULT_MAX_FCNT_GAP < fcnt) {
        goto out;
    }

    mac->mcps.fcnt_down = fcnt;
    error = false;

    int ack_req = lorawan_hdr_get_mtype(lw_hdr) == MTYPE_CNF_DOWNLINK;
    if (ack_req) {
        mac->mcps.ack_requested = true;
    }

    iolist_t payload = { .iol_base = pkt->data, .iol_len = pkt->size };
    if (pkt->data) {
        gnrc_lorawan_encrypt_payload(&payload, &lw_hdr->addr,
                                     byteorder_ntohs(byteorder_ltobs(lw_hdr->fcnt)),
                                     GNRC_LORAWAN_DIR_DOWNLINK,
                                     fopts_in_payload ? mac->nwkskey : mac->appskey);
    }

    /* if there are fopts, it's either an empty packet or application payload */
    if (fopts) {
        gnrc_lorawan_process_fopts(mac, fopts->data, fopts->size);
    }
    else if (fopts_in_payload) {
        gnrc_lorawan_process_fopts(mac, pkt->data, pkt->size);
    }

    gnrc_lorawan_mcps_event(mac, MCPS_EVENT_RX, lorawan_hdr_get_ack(lw_hdr));
    if (pkt->data && fport && *((uint8_t *) fport->data) != 0) {
        pkt->type = GNRC_NETTYPE_LORAWAN;
        release = false;

        mcps_indication_t mcps_indication;
        mcps_indication.type = ack_req;
        mcps_indication.data.pkt = pkt;
        mcps_indication.data.port = *((uint8_t *) fport->data);
        gnrc_lorawan_mcps_indication(mac, &mcps_indication);
    }

    if (lorawan_hdr_get_frame_pending(lw_hdr)) {
        mlme_indication_t mlme_indication;
        mlme_indication.type = MLME_SCHEDULE_UPLINK;
        gnrc_lorawan_mlme_indication(mac, &mlme_indication);
    }

out:
    if (error) {
        gnrc_lorawan_mcps_event(mac, MCPS_EVENT_NO_RX, 0);
    }

    if (release) {
        DEBUG("gnrc_lorawan: release packet\n");
        gnrc_pktbuf_release(pkt);
    }
}

size_t gnrc_lorawan_build_hdr(uint8_t mtype, le_uint32_t *dev_addr, uint32_t fcnt, uint8_t ack, uint8_t fopts_length, lorawan_buffer_t *buf)
{
    assert(fopts_length < 16);
    lorawan_hdr_t *lw_hdr = (lorawan_hdr_t *) buf->data;

    lw_hdr->mt_maj = 0;
    lorawan_hdr_set_mtype(lw_hdr, mtype);
    lorawan_hdr_set_maj(lw_hdr, MAJOR_LRWAN_R1);

    lw_hdr->addr = *dev_addr;
    lw_hdr->fctrl = 0;

    lorawan_hdr_set_ack(lw_hdr, ack);
    lorawan_hdr_set_frame_opts_len(lw_hdr, fopts_length);

    lw_hdr->fcnt = byteorder_btols(byteorder_htons(fcnt));

    buf->index += sizeof(lorawan_hdr_t);

    return sizeof(lorawan_hdr_t);
}

gnrc_pktsnip_t *gnrc_lorawan_build_uplink(gnrc_lorawan_t *mac, gnrc_pktsnip_t *payload, int confirmed_data, uint8_t port)
{
    /* Encrypt payload (it's block encryption so we can use the same buffer!) */
    gnrc_lorawan_encrypt_payload((iolist_t *) payload, &mac->dev_addr, mac->mcps.fcnt, GNRC_LORAWAN_DIR_UPLINK, port ? mac->appskey : mac->nwkskey);

    /* We try to allocate the whole header with fopts at once */
    uint8_t fopts_length = gnrc_lorawan_build_options(mac, NULL);

    gnrc_pktsnip_t *mac_hdr = gnrc_pktbuf_add(payload, NULL, sizeof(lorawan_hdr_t) + fopts_length + 1, GNRC_NETTYPE_UNDEF);

    if (!mac_hdr) {
        gnrc_pktbuf_release_error(payload, -ENOBUFS);
        return NULL;
    }

    gnrc_pktsnip_t *mic = gnrc_pktbuf_add(NULL, NULL, MIC_SIZE, GNRC_NETTYPE_UNDEF);
    if (!mic) {
        gnrc_pktbuf_release_error(mac_hdr, -ENOBUFS);
        return NULL;
    }

    lorawan_buffer_t buf = {
        .data = (uint8_t *) mac_hdr->data,
        .size = mac_hdr->size,
        .index = 0
    };

    gnrc_lorawan_build_hdr(confirmed_data ? MTYPE_CNF_UPLINK : MTYPE_UNCNF_UPLINK,
                           &mac->dev_addr, mac->mcps.fcnt, mac->mcps.ack_requested, fopts_length, &buf);

    gnrc_lorawan_build_options(mac, &buf);

    assert(buf.index == mac_hdr->size - 1);

    buf.data[buf.index++] = port;

    gnrc_lorawan_calculate_mic(&mac->dev_addr, mac->mcps.fcnt, GNRC_LORAWAN_DIR_UPLINK,
                               (iolist_t *) mac_hdr, mac->nwkskey, mic->data);

    LL_APPEND(payload, mic);

    return mac_hdr;
}

static void _end_of_tx(gnrc_lorawan_t *mac, int type, int status)
{
    mac->mcps.waiting_for_ack = false;

    mcps_confirm_t mcps_confirm;

    mcps_confirm.type = type;
    mcps_confirm.status = status;
    gnrc_lorawan_mcps_confirm(mac, &mcps_confirm);

    mac->mcps.fcnt += 1;
}

void gnrc_lorawan_mcps_event(gnrc_lorawan_t *mac, int event, int data)
{
    if (mac->mlme.activation == MLME_ACTIVATION_NONE) {
        return;
    }

    if (event == MCPS_EVENT_ACK_TIMEOUT) {
        gnrc_lorawan_send_pkt(mac, mac->mcps.outgoing_pkt, mac->last_dr);
    }
    else {
        int state = mac->mcps.waiting_for_ack ? MCPS_CONFIRMED : MCPS_UNCONFIRMED;
        if (state == MCPS_CONFIRMED && ((event == MCPS_EVENT_RX && !data) ||
                                        event == MCPS_EVENT_NO_RX)) {
            if (mac->mcps.nb_trials-- == 0) {
                _end_of_tx(mac, MCPS_CONFIRMED, -ETIMEDOUT);
            }
        }
        else {
            _end_of_tx(mac, state, GNRC_LORAWAN_REQ_STATUS_SUCCESS);
        }

        mac->msg.type = MSG_TYPE_MCPS_ACK_TIMEOUT;
        if (mac->mcps.outgoing_pkt) {
            xtimer_set_msg(&mac->rx, 1000000 + random_uint32_range(0, 2000000), &mac->msg, thread_getpid());
        }
    }
}

void gnrc_lorawan_mcps_request(gnrc_lorawan_t *mac, const mcps_request_t *mcps_request, mcps_confirm_t *mcps_confirm)
{
    int release = true;
    gnrc_pktsnip_t *pkt = mcps_request->data.pkt;

    if (mac->mlme.activation == MLME_ACTIVATION_NONE) {
        DEBUG("gnrc_lorawan_mcps: LoRaWAN not activated\n");
        mcps_confirm->status = -ENOTCONN;
        goto out;
    }

    if (!gnrc_lorawan_mac_acquire(mac)) {
        mcps_confirm->status = -EBUSY;
        goto out;
    }

    if (mcps_request->data.port < LORAMAC_PORT_MIN ||
        mcps_request->data.port > LORAMAC_PORT_MAX) {
        mcps_confirm->status = -EBADMSG;
        goto out;
    }

    if (!gnrc_lorawan_validate_dr(mcps_request->data.dr)) {
        mcps_confirm->status = -EINVAL;
        goto out;
    }

    int waiting_for_ack = mcps_request->type == MCPS_CONFIRMED;
    if (!(pkt = gnrc_lorawan_build_uplink(mac, pkt, waiting_for_ack, mcps_request->data.port))) {
        /* This function releases the pkt if fails */
        release = false;
        mcps_confirm->status = -ENOBUFS;
        goto out;
    }

    if ((gnrc_pkt_len(pkt) - MIC_SIZE - 1) > gnrc_lorawan_region_mac_payload_max(mcps_request->data.dr)) {
        mcps_confirm->status = -EMSGSIZE;
        goto out;
    }

    release = false;
    mac->mcps.waiting_for_ack = waiting_for_ack;
    mac->mcps.ack_requested = false;

    mac->mcps.nb_trials = LORAMAC_DEFAULT_RETX;

    assert(mac->mcps.outgoing_pkt == NULL);
    mac->mcps.outgoing_pkt = pkt;

    gnrc_lorawan_send_pkt(mac, pkt, mcps_request->data.dr);
    mcps_confirm->status = GNRC_LORAWAN_REQ_STATUS_DEFERRED;
out:

    if (mcps_confirm->status != GNRC_LORAWAN_REQ_STATUS_DEFERRED) {
        gnrc_lorawan_mac_release(mac);
    }

    if (release) {
        gnrc_pktbuf_release_error(pkt, mcps_confirm->status);
    }
}

/** @} */
