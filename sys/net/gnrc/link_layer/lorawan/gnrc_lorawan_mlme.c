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
#define BEACON_TOA_US   (153)
#define BEACON_RESERVED (2120)

#define BEACON_COUNTER_DEFAULT (56U)

#include "debug.h"

static void _resolve_slot_offset(gnrc_lorawan_t *mac, bool beacon);

static void _build_join_req_pkt(uint8_t *appeui, uint8_t *deveui,
                                uint8_t *appkey, uint8_t *dev_nonce,
                                uint8_t *psdu)
{
    lorawan_join_request_t *hdr = (lorawan_join_request_t *)psdu;

    hdr->mt_maj = 0;
    lorawan_hdr_set_mtype((lorawan_hdr_t *)hdr, MTYPE_JOIN_REQUEST);
    lorawan_hdr_set_maj((lorawan_hdr_t *)hdr, MAJOR_LRWAN_R1);

    le_uint64_t l_appeui = *((le_uint64_t *)appeui);
    le_uint64_t l_deveui = *((le_uint64_t *)deveui);

    hdr->app_eui = l_appeui;
    hdr->dev_eui = l_deveui;

    le_uint16_t l_dev_nonce = *((le_uint16_t *)dev_nonce);

    hdr->dev_nonce = l_dev_nonce;

    gnrc_lorawan_calculate_join_mic(psdu, JOIN_REQUEST_SIZE - MIC_SIZE, appkey,
                                    &hdr->mic);
}

void gnrc_lorawan_trigger_join(gnrc_lorawan_t *mac)
{
    iolist_t pkt = {.iol_base = mac->mcps.mhdr_mic, .iol_len =
                    sizeof(lorawan_join_request_t), .iol_next = NULL};
    gnrc_lorawan_send_pkt(mac, &pkt, mac->last_dr);
}

static int gnrc_lorawan_send_join_request(gnrc_lorawan_t *mac, uint8_t *deveui,
                                          uint8_t *appeui, uint8_t *appkey,
                                          uint8_t dr)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);

    /* Dev Nonce */
    uint32_t random_number;

    dev->driver->get(dev, NETOPT_RANDOM, &random_number, sizeof(random_number));

    mac->mlme.dev_nonce[0] = random_number & 0xFF;
    mac->mlme.dev_nonce[1] = (random_number >> 8) & 0xFF;

    mac->last_dr = dr;
    mac->state = LORAWAN_STATE_JOIN;

    /* Use the buffer for MHDR */
    _build_join_req_pkt(appeui, deveui, appkey, mac->mlme.dev_nonce, (uint8_t*) mac->mcps.mhdr_mic);

    /* We need a random delay for join request. Otherwise there might be
     * network congestion if a group of nodes start at the same time */
    gnrc_lorawan_set_timer(mac, random_uint32() & GNRC_LORAWAN_JOIN_DELAY_U32_MASK);

    mac->mlme.backoff_budget -= mac->toa;

    return GNRC_LORAWAN_REQ_STATUS_DEFERRED;
}

void gnrc_lorawan_mlme_process_join(gnrc_lorawan_t *mac, uint8_t *data,
                                    size_t size)
{
    int status;
    mlme_confirm_t mlme_confirm;

    if (mac->mlme.activation != MLME_ACTIVATION_NONE) {
        status = -EBADMSG;
        goto out;
    }

    if (size != GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE - CFLIST_SIZE &&
        size != GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE) {
        status = -EBADMSG;
        goto out;
    }

    /* Subtract 1 from join accept max size, since the MHDR was already read */
    uint8_t out[GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE - 1];
    uint8_t has_cflist = (size - 1) > CFLIST_SIZE;

    gnrc_lorawan_decrypt_join_accept(mac->appskey, data + 1,
                                     has_cflist, out);
    memcpy(data + 1, out, size - 1);

    le_uint32_t mic;
    le_uint32_t *expected_mic = (le_uint32_t *)(data + size - MIC_SIZE);

    gnrc_lorawan_calculate_join_mic(data, size - MIC_SIZE, mac->appskey, &mic);
    if (mic.u32 != expected_mic->u32) {
        DEBUG("gnrc_lorawan_mlme: wrong MIC.\n");
        status = -EBADMSG;
        goto out;
    }

    lorawan_join_accept_t *ja_hdr = (lorawan_join_accept_t *)data;

    gnrc_lorawan_generate_session_keys(ja_hdr->app_nonce, mac->mlme.dev_nonce,
                                       mac->appskey, mac->nwkskey,
                                       mac->appskey);

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

out:
    mlme_confirm.type = MLME_JOIN;
    mlme_confirm.status = status;

    gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);
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
            if (mac->mlme.activation != MLME_ACTIVATION_NONE) {
                mlme_confirm->status = -EINVAL;
                break;
            }

            if (mac->mlme.backoff_budget < 0) {
                mlme_confirm->status = -EDQUOT;
                break;
            }
            memcpy(mac->appskey, mlme_request->join.appkey, LORAMAC_APPKEY_LEN);
            mlme_confirm->status = gnrc_lorawan_send_join_request(mac,
                                                                  mlme_request->join.deveui,
                                                                  mlme_request->join.appeui, mlme_request->join.appkey,
                                                                  mlme_request->join.dr);
            break;
        case MLME_SYNC:
            if(mac->mlme.activation == MLME_ACTIVATION_NONE) {
                mlme_confirm->status = -EINVAL;
                break;
            }

            if (mac->state != LORAWAN_STATE_IDLE) {
                mlme_confirm->status = -EBUSY;
                break;
            }

            if (mlme_request->device_time) {
                mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_DEFERRED;
                puts(":)");
                mac->state = LORAWAN_STATE_BEACON_ACQUIRE;
                uint32_t target = mlme_request->device_time->reference + 
                    (0x80 - (mlme_request->device_time->seconds & 0x7F))*1000 - (mlme_request->device_time->msecs);
                uint32_t diff = target - gnrc_lorawan_timer_now(mac); 
                gnrc_lorawan_set_timer(mac, diff);
            }
            else {
                /* TODO: Disable beacons */
            }
            break;

        case MLME_LINK_CHECK:
            mac->mlme.pending_mlme_opts |=
                GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;
            mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_DEFERRED;
            break;
        case MLME_DEVICE_TIME:
            mac->mlme.pending_mlme_opts |= GNRC_LORAWAN_MLME_OPTS_DEVICE_TIME_REQ;
            mlme_confirm->status = GNRC_LORAWAN_REQ_STATUS_DEFERRED;
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

void gnrc_lorawan_beacon_lost(gnrc_lorawan_t *mac)
{
    puts("Beacon lost :(");

    uint32_t exp_beacon_time = byteorder_ntohl(byteorder_ltobl(mac->mlme.beacon_time)) + 0x80;

    mac->mlme.beacon_time = byteorder_btoll(byteorder_htonl(exp_beacon_time));

    _resolve_slot_offset(mac, false);
    /*
    mlme_confirm_t mlme_confirm;
    mlme_confirm.type = MLME_SYNC;
    mlme_confirm.status = -ENOLINK;
    mac->state = LORAWAN_STATE_IDLE;

    netopt_state_t state = NETOPT_STATE_SLEEP;
    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
    gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);
    */
}

uint16_t next_slot;

static uint32_t get_slot_timestamp(gnrc_lorawan_t *mac, int slot)
{
    return mac->mlme.rx_ref + BEACON_RESERVED + slot * 30;
}

void gnrc_lorawan_schedule_next_pingslot(gnrc_lorawan_t *mac)
{
    uint32_t now = gnrc_lorawan_timer_now(mac);
    /* TODO: Add backoff? */
    while (get_slot_timestamp(mac, next_slot) < now) {
        /* TODO: Check */
        next_slot += mac->mlme.ping_period;
    }

    if (next_slot < 4096) {
        gnrc_lorawan_set_timer(mac, get_slot_timestamp(mac, next_slot) - 1 - gnrc_lorawan_timer_now(mac));
        printf("%i\n", next_slot);
    }
    else {
        mac->mlme.rx_ref += 128000;
        gnrc_lorawan_set_timer(mac, mac->mlme.rx_ref - now);
        mac->state = LORAWAN_STATE_BEACON_ACQUIRE;
        puts("BEACON");
    }
}

void _config_pingslot_rx_window(gnrc_lorawan_t *mac);

static void _resolve_slot_offset(gnrc_lorawan_t *mac, bool beacon)
{
    mlme_confirm_t mlme_confirm;
    mlme_confirm.type = MLME_SYNC;
    mlme_confirm.status = 0;
    mac->state = LORAWAN_STATE_IDLE;

    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    int slot_offset = gnrc_lorawan_calculate_slot(&mac->mlme.beacon_time, &mac->dev_addr, mac->mlme.ping_period);

    next_slot = slot_offset;

    _config_pingslot_rx_window(mac);
    gnrc_lorawan_set_timer(mac, get_slot_timestamp(mac, next_slot) - 1 - gnrc_lorawan_timer_now(mac));

    netopt_state_t state = NETOPT_STATE_SLEEP;
    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));

    if (beacon) {
        mac->mlme.sync = true;
        mac->mlme.beacon_counter = BEACON_COUNTER_DEFAULT;
        gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);
    }
    else if ((--mac->mlme.beacon_counter) == 0) {
        /* TODO: Go to class A and generate indication */
        puts("Beacon lost");
    }
}

void gnrc_lorawan_mlme_process_beacon(gnrc_lorawan_t *mac, uint8_t *psdu, size_t size, lora_rx_info_t *info)
{

    memcpy(&mac->mlme.beacon_time, psdu+2, sizeof(le_uint32_t));
    mac->mlme.rx_ref = gnrc_lorawan_timer_now(mac) - BEACON_TOA_US;
    _resolve_slot_offset(mac, true);
    mlme_indication_t mlme_indication;
    mlme_indication.type = MLME_BEACON_NOTIFY;
    mlme_indication.beacon.psdu = psdu;
    mlme_indication.beacon.len = (uint8_t) size;
    mlme_indication.beacon.info = info;
    gnrc_lorawan_mlme_indication(mac, &mlme_indication);
}

int _fopts_mlme_device_time_req(lorawan_buffer_t *buf)
{
    if (buf) {
        assert(buf->index + GNRC_LORAWAN_CID_SIZE <= buf->size);
        buf->data[buf->index++] = GNRC_LORAWAN_CID_DEVICE_TIME;
    }

    return GNRC_LORAWAN_CID_SIZE;
}

int _fopts_mlme_link_check_req(lorawan_buffer_t *buf)
{
    if (buf) {
        assert(buf->index + GNRC_LORAWAN_CID_SIZE <= buf->size);
        buf->data[buf->index++] = GNRC_LORAWAN_CID_LINK_CHECK_ANS;
    }

    return GNRC_LORAWAN_CID_SIZE;
}

static int _fopts_mlme_ping_slot_ans(gnrc_lorawan_t *mac, lorawan_buffer_t *buf)
{
    if (buf) {
        assert(buf->index + GNRC_LORAWAN_CID_PING_SLOT_CHANNEL_ANS_SIZE <= buf->size);
        buf->data[buf->index++] = GNRC_LORAWAN_CID_PING_SLOT_CHANNEL_REQ;
        buf->data[buf->index++] = mac->mlme.ps_chan_ans;
    }
    mac->mlme.pending_mlme_opts &= ~GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;
    return GNRC_LORAWAN_CID_PING_SLOT_CHANNEL_ANS_SIZE;
}

static void _mlme_link_check_ans(gnrc_lorawan_t *mac, uint8_t *p)
{
    mlme_confirm_t mlme_confirm;

    mlme_confirm.link_req.margin = p[1];
    mlme_confirm.link_req.num_gateways = p[2];

    mlme_confirm.type = MLME_LINK_CHECK;
    mlme_confirm.status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
    gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);

    mac->mlme.pending_mlme_opts &= ~GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;
}
static void _mlme_ping_slot_channel_req(gnrc_lorawan_t *mac, uint8_t *p)
{
    if (mac->mlme.sync) {
        /* Ignore if class B */
        return;
    }

    uint32_t channel = (p[1] + (p[2] << 8) + ((uint32_t) p[3] << 16)) * 100;
    uint8_t tmp = 0;
    for (unsigned i=0; i<GNRC_LORAWAN_MAX_CHANNELS; i++) {
        if (mac->channel[i] == channel) {
            tmp = 1;
        }
    }
    if (gnrc_lorawan_validate_dr(p[4])) {
        tmp |= 1<<1;
    }

    if ((tmp & 0x3) == 0x3) {
        mac->mlme.ps_channel = channel ? channel : CONFIG_GNRC_LORAWAN_DEFAULT_PING_SLOT_CHANNEL;
        mac->mlme.ps_dr = p[4];
    }

    mac->mlme.ps_chan_ans = tmp;
    mac->mlme.pending_mlme_opts |= GNRC_LORAWAN_MLME_OPTS_PING_SLOT_CHANNEL_ANS;
}

static void _mlme_device_time_ans(gnrc_lorawan_t *mac, uint8_t *p)
{
    mlme_confirm_t mlme_confirm;
    mlme_dt_opt_t *device_time = (void*) (p+1);

    mac->mlme.pending_mlme_opts &= ~GNRC_LORAWAN_MLME_OPTS_DEVICE_TIME_REQ;

    mlme_confirm.device_time.seconds = byteorder_ntohl(byteorder_ltobl(device_time->seconds));
    mlme_confirm.device_time.msecs = (device_time->frac * 1000) >> 8;
    mlme_confirm.device_time.reference = mac->mlme.timestamp;

    mlme_confirm.type = MLME_DEVICE_TIME;
    mlme_confirm.status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
    gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);
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
            case GNRC_LORAWAN_CID_PING_SLOT_CHANNEL_REQ:
                ret += GNRC_LORAWAN_CID_PING_SLOT_CHANNEL_REQ_SIZE;
                cb = _mlme_ping_slot_channel_req;
                break;
            case GNRC_LORAWAN_CID_DEVICE_TIME:
                ret += GNRC_LORAWAN_FOPT_DEVICE_TIME_ANS_SIZE;
                cb = _mlme_device_time_ans;
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

    if (mac->mlme.pending_mlme_opts & GNRC_LORAWAN_MLME_OPTS_PING_SLOT_CHANNEL_ANS) {
        size += _fopts_mlme_ping_slot_ans(mac, buf);
    }

    if (mac->mlme.pending_mlme_opts & GNRC_LORAWAN_MLME_OPTS_DEVICE_TIME_REQ) {
        size += _fopts_mlme_device_time_req(buf);
    }

    return size;
}
