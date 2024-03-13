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
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "net/lora.h"
#include "net/gnrc/lorawan.h"
#include "net/gnrc/lorawan/region.h"
#include "errno.h"
#include "net/gnrc/pktbuf.h"
#include "random.h"

#include "net/lorawan/hdr.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _build_join_req_pkt(uint8_t *joineui, uint8_t *deveui,
                                uint8_t *key, uint8_t *dev_nonce,
                                uint8_t *psdu)
{
    lorawan_join_request_t *hdr = (lorawan_join_request_t *)psdu;

    hdr->mt_maj = 0;
    lorawan_hdr_set_mtype((lorawan_hdr_t *)hdr, MTYPE_JOIN_REQUEST);
    lorawan_hdr_set_maj((lorawan_hdr_t *)hdr, MAJOR_LRWAN_R1);

    le_uint64_t l_joineui = *((le_uint64_t *)joineui);
    le_uint64_t l_deveui = *((le_uint64_t *)deveui);

    hdr->join_eui = l_joineui;
    hdr->dev_eui = l_deveui;

    le_uint16_t l_dev_nonce = *((le_uint16_t *)dev_nonce);

    hdr->dev_nonce = l_dev_nonce;

    gnrc_lorawan_calculate_join_req_mic(psdu, JOIN_REQUEST_SIZE - MIC_SIZE, key,
                                        &hdr->mic);
}

void gnrc_lorawan_trigger_join(gnrc_lorawan_t *mac)
{
    iolist_t pkt = { .iol_base = mac->mcps.mhdr_mic, .iol_len =
                         sizeof(lorawan_join_request_t), .iol_next = NULL };

    gnrc_lorawan_send_pkt(mac, &pkt, mac->last_dr,
                          gnrc_lorawan_pick_channel(mac));
}

static int gnrc_lorawan_send_join_request(gnrc_lorawan_t *mac, uint8_t *deveui,
                                          uint8_t *eui, uint8_t *key, uint8_t dr)
{
    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        /**
         * DevNonce starting at 0 when device is powered up and incremented with
         * every Join-request.
         */
        uint16_t dev_nonce = byteorder_lebuftohs(mac->mlme.dev_nonce);
        byteorder_htolebufs(mac->mlme.dev_nonce, ++dev_nonce);
        gnrc_lorawan_store_dev_nonce(mac->mlme.dev_nonce);

        gnrc_lorawan_generate_lifetime_session_keys(deveui, mac->ctx.nwksenckey,
                                                    gnrc_lorawan_get_jsintkey(mac),
                                                    gnrc_lorawan_get_jsenckey(mac));
    }
    else {
        netdev_t *dev = gnrc_lorawan_get_netdev(mac);

        /* Dev Nonce */
        uint32_t random_number;

        dev->driver->get(dev, NETOPT_RANDOM, &random_number, sizeof(random_number));

        mac->mlme.dev_nonce[0] = random_number & 0xFF;
        mac->mlme.dev_nonce[1] = (random_number >> 8) & 0xFF;
    }

    mac->last_dr = dr;

    /* Use the buffer for MHDR */
    _build_join_req_pkt(eui, deveui, key, mac->mlme.dev_nonce, (uint8_t *)mac->mcps.mhdr_mic);

    gnrc_lorawan_dispatch_event(mac, &mac->mac_fsm, GNRC_LORAWAN_EV_LINK_UP);

    mac->mlme.backoff_budget -= mac->toa;

    return GNRC_LORAWAN_REQ_STATUS_DEFERRED;
}

void gnrc_lorawan_mlme_process_join(gnrc_lorawan_t *mac, uint8_t *data,
                                    size_t size)
{
    int status;

    /* Ignore if the state machine is not waiting for Join Accept messages */
    if (gnrc_lorawan_is_joined(mac)) {
        return;
    }

    if (size != GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE - CFLIST_SIZE &&
        size != GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE) {
        status = -EBADMSG;
        goto dispatch;
    }

    /* Subtract 1 from join accept max size, since the MHDR was already read */
    uint8_t out[GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE - 1];
    uint8_t has_cflist = (size - 1) > CFLIST_SIZE;

    gnrc_lorawan_decrypt_join_accept(mac->ctx.nwksenckey, data + 1, has_cflist, out);

    memcpy(data + 1, out, size - 1);

    le_uint32_t mic;
    le_uint32_t *expected_mic = (le_uint32_t *)(data + size - MIC_SIZE);

    lorawan_join_accept_t *ja_hdr = (lorawan_join_accept_t *)data;

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        gnrc_lorawan_set_optneg(mac, lorawan_ja_hdr_get_optneg(ja_hdr));
    }

    gnrc_lorawan_calculate_join_acpt_mic(data, size - MIC_SIZE, mac, &mic);

    if (mic.u32 != expected_mic->u32) {
        DEBUG("gnrc_lorawan_mlme: wrong MIC.\n");
        status = -EBADMSG;
        goto dispatch;
    }

    void *joineui = IS_USED(MODULE_GNRC_LORAWAN_1_1) ? mac->joineui : NULL;

    gnrc_lorawan_generate_session_keys(ja_hdr->join_nonce,
                                       mac->mlme.dev_nonce, joineui, mac);

    le_uint32_t le_nid;

    le_nid.u32 = 0;
    memcpy(&le_nid, ja_hdr->net_id, 3);
    mac->mlme.nid = byteorder_ltohl(le_nid);
    /* Copy devaddr */
    memcpy(&mac->dev_addr, ja_hdr->dev_addr, sizeof(mac->dev_addr));

    mac->dl_settings = ja_hdr->dl_settings;

    /* delay 0 maps to 1 second */
    mac->rx_delay = ja_hdr->rx_delay ? ja_hdr->rx_delay : 1;

    if (has_cflist) {
        gnrc_lorawan_process_cflist(mac, out + sizeof(lorawan_join_accept_t) - 1);
    }

    DEBUG("gnrc_lorawan: Channel mask is %04x\n", mac->channel_mask);
    mac->mlme.activation = MLME_ACTIVATION_OTAA;
    status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;

    /* schedule rekey indication command */
    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        mac->mlme.pending_mlme_opts |= GNRC_LORAWAN_MLME_OPTS_REKEY_IND_REQ;
    }

dispatch:

    /* Indicate that the reception was successful */
    gnrc_lorawan_dispatch_event(mac, &mac->mac_fsm, status == 0
                                                    ? GNRC_LORAWAN_EV_RX_DONE
                                                    : GNRC_LORAWAN_EV_RX_ERROR);
}

void gnrc_lorawan_mlme_backoff_expire_cb(gnrc_lorawan_t *mac)
{
    uint8_t counter = mac->mlme.backoff_state & 0x1F;
    uint8_t state = mac->mlme.backoff_state >> 5;

    if (counter == 0) {
        switch (state) {
        case GNRC_LORAWAN_BACKOFF_STATE_1:
            counter = GNRC_LORAWAN_BACKOFF_TIME_1;
            state = GNRC_LORAWAN_BACKOFF_STATE_2;
            mac->mlme.backoff_budget = GNRC_LORAWAN_BACKOFF_BUDGET_1;
            break;
        case GNRC_LORAWAN_BACKOFF_STATE_2:
            counter = GNRC_LORAWAN_BACKOFF_TIME_2;
            state = GNRC_LORAWAN_BACKOFF_STATE_3;
            mac->mlme.backoff_budget = GNRC_LORAWAN_BACKOFF_BUDGET_2;
            break;
        case GNRC_LORAWAN_BACKOFF_STATE_3:
        default:
            counter = GNRC_LORAWAN_BACKOFF_TIME_3;
            mac->mlme.backoff_budget = GNRC_LORAWAN_BACKOFF_BUDGET_3;
            break;
        }
    }

    counter--;
    mac->mlme.backoff_state = state << 5 | (counter & 0x1F);
}

static void _mlme_set(gnrc_lorawan_t *mac, const mlme_request_t *mlme_request,
                      mlme_confirm_t *mlme_confirm)
{
    mlme_confirm->status = -EINVAL;
    switch (mlme_request->mib.type) {
    case MIB_ACTIVATION_METHOD:
        if (mlme_request->mib.activation != MLME_ACTIVATION_OTAA) {
            mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
            mac->mlme.activation = mlme_request->mib.activation;
            gnrc_lorawan_dispatch_event(mac, &mac->mac_fsm, GNRC_LORAWAN_EV_LINK_UP);
        }
        break;
    case MIB_DEV_ADDR:
        mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        memcpy(&mac->dev_addr, mlme_request->mib.dev_addr,
               sizeof(uint32_t));
        break;
    case MIB_RX2_DR:
        mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        gnrc_lorawan_set_rx2_dr(mac, mlme_request->mib.rx2_dr);
        break;
    default:
        break;
    }
}

static void _mlme_get(gnrc_lorawan_t *mac, const mlme_request_t *mlme_request,
                      mlme_confirm_t *mlme_confirm)
{
    switch (mlme_request->mib.type) {
    case MIB_ACTIVATION_METHOD:
        mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        mlme_confirm->mib.activation = mac->mlme.activation;
        break;
    case MIB_DEV_ADDR:
        mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        mlme_confirm->mib.dev_addr = &mac->dev_addr;
        break;
    default:
        mlme_confirm->status = -EINVAL;
        break;
    }
}

void gnrc_lorawan_mlme_request(gnrc_lorawan_t *mac,
                               const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm)
{
    switch (mlme_request->type) {
    case MLME_JOIN:
        if (gnrc_lorawan_is_joined(mac)) {
            mlme_confirm->status = -EINVAL;
            break;
        }

        if (gnrc_lorawan_is_busy(mac)) {
            mlme_confirm->status = -EBUSY;
            break;
        }

        if (mac->mlme.backoff_budget < 0) {
            mlme_confirm->status = -EDQUOT;
            break;
        }

        if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
            memcpy(mac->ctx.appskey, gnrc_lorawan_mlme_join_get_appkey(
                       &mlme_request->join), LORAMAC_APPKEY_LEN);
        }

        memcpy(mac->ctx.nwksenckey, mlme_request->join.nwkkey, LORAMAC_NWKKEY_LEN);
        mlme_confirm->status = gnrc_lorawan_send_join_request(mac,
                                                              mlme_request->join.deveui,
                                                              mlme_request->join.joineui,
                                                              mlme_request->join.nwkkey,
                                                              mlme_request->join.dr);
        break;
    case MLME_LINK_CHECK:
        if (mlme_request->mib.link_check) {
            mac->mlme.pending_mlme_opts |=
                GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;
        }
        else {
            mac->mlme.pending_mlme_opts &=
                ~GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;
        }
        mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        break;
    case MLME_SET:
        _mlme_set(mac, mlme_request, mlme_confirm);
        break;
    case MLME_GET:
        _mlme_get(mac, mlme_request, mlme_confirm);
        break;
    case MLME_RESET:
        gnrc_lorawan_reset(mac);
        mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        break;
    default:
        break;
    }
}

static int _fopts_mlme_link_check_req(lorawan_buffer_t *buf)
{
    if (buf) {
        assert(buf->index + GNRC_LORAWAN_CID_SIZE <= buf->size);
        buf->data[buf->index++] = GNRC_LORAWAN_CID_LINK_CHECK_ANS;
    }

    return GNRC_LORAWAN_CID_SIZE;
}

static void _mlme_link_check_ans(gnrc_lorawan_t *mac, uint8_t *p)
{
    mlme_indication_t mlme_indication;

    mlme_indication.link_req.margin = p[1];
    mlme_indication.link_req.num_gateways = p[2];

    mlme_indication.type = MLME_LINK_CHECK;
    gnrc_lorawan_mlme_indication(mac, &mlme_indication);
}

static int _fopts_mlme_link_rekey_ind(lorawan_buffer_t *buf)
{
    if (buf) {
        assert(buf->index + GNRC_LORAWAN_CID_SIZE +
               GNCR_LORAWAN_REKEY_IND_SIZE <= buf->size);

        buf->data[buf->index++] = GNCR_LORAWAN_CID_REKEY_CONF;
        buf->data[buf->index++] = MINOR_LRWAN;
    }

    return GNRC_LORAWAN_CID_SIZE + GNCR_LORAWAN_REKEY_IND_SIZE;
}

static void _mlme_rekey_check_conf(gnrc_lorawan_t *mac, uint8_t *p)
{
    /* server version must by smaller or equal to device's LoRaWAN version */
    uint8_t server_minor = p[1];

    if (server_minor <= MINOR_LRWAN) {
        mac->mlme.pending_mlme_opts &= ~GNRC_LORAWAN_MLME_OPTS_REKEY_IND_REQ;
    }
}

void gnrc_lorawan_process_fopts(gnrc_lorawan_t *mac, uint8_t *fopts,
                                size_t size)
{
    if (!fopts || !size) {
        return;
    }

    uint8_t ret = 0;

    void (*cb)(gnrc_lorawan_t *, uint8_t *p) = NULL;

    for (uint8_t pos = 0; pos < size; pos += ret) {
        switch (fopts[pos]) {
        case GNRC_LORAWAN_CID_LINK_CHECK_ANS:
            ret += GNRC_LORAWAN_FOPT_LINK_CHECK_ANS_SIZE;
            cb = _mlme_link_check_ans;
            break;
        case GNCR_LORAWAN_CID_REKEY_CONF:
            ret += GNRC_LORAWAN_FOPT_REKEY_CONF_SIZE;
            cb = _mlme_rekey_check_conf;
            break;
        default:
            return;
        }

        if (pos + ret > size) {
            return;
        }

        cb(mac, &fopts[pos]);
    }
}

uint8_t gnrc_lorawan_build_options(gnrc_lorawan_t *mac, lorawan_buffer_t *buf)
{
    size_t size = 0;

    if (mac->mlme.pending_mlme_opts & GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ) {
        size += _fopts_mlme_link_check_req(buf);
    }

    if (mac->mlme.pending_mlme_opts & GNRC_LORAWAN_MLME_OPTS_REKEY_IND_REQ) {
        size += _fopts_mlme_link_rekey_ind(buf);
    }

    return size;
}
