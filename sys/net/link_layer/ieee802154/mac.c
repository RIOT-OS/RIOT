/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "net/ieee802154/mac.h"
#include "mac_internal.h"
#include "mac_pib.h"
#include "mac_fsm.h"

#define ENABLE_DEBUG 0
#include "debug.h"


void ieee802154_mac_mlme_set_request(ieee802154_mac_t *mac,
                                     ieee802154_pib_attr_t attr,
                                     const ieee802154_pib_value_t *in)
{
    ieee802154_mac_mlme_set(mac, attr, in);
    if (attr == IEEE802154_PIB_PAN_ID) {
        ieee802154_set_panid(&mac->submac, &in->v.u16);
    }
    else if (attr == IEEE802154_PIB_EXTENDED_ADDRESS) {
        ieee802154_set_ext_addr(&mac->submac, &in->v.ext_addr);
    }
    else if (attr == IEEE802154_PIB_SHORT_ADDR) {
        ieee802154_set_short_addr(&mac->submac, &in->v.short_addr);
    }
}


void ieee802154_mac_mlme_get_request(ieee802154_mac_t *mac,
                                     ieee802154_pib_attr_t attr,
                                     ieee802154_pib_value_t *out)
{
    ieee802154_mac_mlme_get(mac, attr, out);
}

void ieee802154_mac_init(ieee802154_mac_t *mac,
                         const ieee802154_mac_cbs_t *cbs)
{
    ieee802154_mac_init_with_devtype(mac, cbs, IEEE802154_DEV_TYPE_KW2XRF);
}

void ieee802154_mac_init_with_devtype(ieee802154_mac_t *mac,
                                      const ieee802154_mac_cbs_t *cbs,
                                      ieee802154_dev_type_t dev_type)
{
    /* Preserve radio HAL descriptor initialized by radio init callback */
    ieee802154_dev_t hal_dev = mac->submac.dev;
    memset(mac, 0, sizeof(*mac));
    mac->submac.dev = hal_dev;
    mac->dev_type = dev_type;
    mac->cbs = *cbs;
    mac->cbs.mac = mac;
    ieee802154_mac_pib_init(mac);
    ieee802154_init_mac_internal(mac);
}

int ieee802154_mac_mlme_scan_request(ieee802154_mac_t *mac, ieee802154_scan_type_t type,
                                     ieee802154_mlme_scan_req_t *req)
{
    (void) type;
    if (mac->scan_active) {
        return -EBUSY;
    }
    if ((req == NULL) || ((req->channels == NULL) && (req->channel_count > 0))) {
        return -EINVAL;
    }
    int res = 0;

    mac->scan_req = req;
    if (req->results_used) {
        *req->results_used = 0;
    }


    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_SCAN_START, NULL) < 0) {
        mac->scan_req = NULL;
        mac->scan_active = false;
        return -EBUSY;
    }

    return res;
}

int ieee802154_mlme_start_request(ieee802154_mac_t *mac,
                                  uint16_t channel)
{
    int res = ieee802154_set_channel_number(&mac->submac, channel);
    if (res < 0) {
        DEBUG("IEEE802154 MAC: failed to set coordinator channel %u\n", channel);
        return res;
    }

    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_COORD_START, NULL) < 0)
    {
        DEBUG("IEEE802154 MAC: failed to start as coordinator\n");
        return -EBUSY;
    }
    return 0;
}

int ieee802154_mcps_data_request(ieee802154_mac_t *mac,
                                 ieee802154_addr_mode_t src_mode,
                                 ieee802154_addr_mode_t dst_mode,
                                 uint16_t dst_panid,
                                 const void *dst_addr,
                                 iolist_t *msdu,
                                 uint8_t msdu_handle,
                                 bool ack_req,
                                 bool indirect)
{
    if (!mac) {
        return -EINVAL;
    }

    int res = -EINVAL;
    ieee802154_mac_fsm_ctx_t ctx;

    memset(&ctx, 0, sizeof(ctx));
    ctx.data_dst_addr = dst_addr;
    ctx.msdu = msdu;
    ctx.msdu_handle = msdu_handle;
    ctx.src_mode = src_mode;
    ctx.dst_mode = dst_mode;
    ctx.dst_panid = dst_panid;
    ctx.ack_req = ack_req;
    ctx.indirect = indirect;
    ctx.result = &res;

    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_MCPS_DATA_REQ, &ctx) < 0) {
        return -EBUSY;
    }

    return res;
}

int ieee802154_mac_mlme_associate_request(ieee802154_mac_t *mac,
                                          const ieee802154_addr_t *coord_addr,
                                          uint16_t channel_num,
                                          uint16_t coord_panid,
                                          ieee802154_assoc_capability_t capability)
{
    if (!mac || !coord_addr) {
        return -EINVAL;
    }

    int res = ieee802154_set_channel_number(&mac->submac, channel_num);
    if (res < 0) {
        return res;
    }
    res = -EINVAL;
    ieee802154_mac_fsm_ctx_t ctx;
    const void *addr = NULL;
    ieee802154_pib_value_t panid;

    if (coord_addr->type == IEEE802154_ADDR_MODE_SHORT) {
        addr = &coord_addr->v.short_addr;
    }
    else if (coord_addr->type == IEEE802154_ADDR_MODE_EXTENDED) {
        addr = &coord_addr->v.ext_addr;
    }
    else {
        return -EINVAL;
    }

    panid.type = IEEE802154_PIB_TYPE_U16;
    panid.v.u16 = coord_panid;
    ieee802154_mac_mlme_set_request(mac, IEEE802154_PIB_PAN_ID, &panid);

    memset(&ctx, 0, sizeof(ctx));
    ctx.dst_addr = addr;
    ctx.data_dst_addr = addr;
    ctx.dst_mode = coord_addr->type;
    ctx.dst_panid = coord_panid;
    ctx.capability = capability;
    ctx.result = &res;

    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_MLME_ASSOC_REQ, &ctx) < 0) {
        return -EBUSY;
    }

    return res;
}

int ieee802154_mac_mlme_associate_response(ieee802154_mac_t *mac,
                                           const ieee802154_addr_t *dst_addr,
                                           ieee802154_assoc_status_t status,
                                           uint16_t short_addr)
{
    if (!mac || !dst_addr) {
        return -EINVAL;
    }

    int res = -EINVAL;
    ieee802154_mac_fsm_ctx_t ctx;
    ieee802154_pib_value_t panid;
    const void *addr = NULL;

    if (dst_addr->type == IEEE802154_ADDR_MODE_SHORT)
    {
        addr = &dst_addr->v.short_addr;
    }
    else if (dst_addr->type == IEEE802154_ADDR_MODE_EXTENDED)
    {
        addr = &dst_addr->v.ext_addr;
    }
    else
    {
        return -EINVAL;
    }

    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_PAN_ID, &panid);

    memset(&ctx, 0, sizeof(ctx));
    ctx.dst_addr = addr;
    ctx.data_dst_addr = addr;
    ctx.dst_mode = dst_addr->type;
    ctx.dst_panid = panid.v.u16;
    ctx.assoc_status = (uint8_t)status;
    ctx.assoc_short_addr = short_addr;
    ctx.result = &res;

    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_MLME_ASSOC_RES, &ctx) < 0)
    {
        return -EBUSY;
    }

    return res;
}

int ieee802154_mac_mlme_poll(ieee802154_mac_t *mac, ieee802154_addr_mode_t coord_mode,
                             uint16_t coord_panid, const void *coord_addr)
{
    if (!mac) {
        return -EINVAL;
    }

    int res = -EINVAL;
    ieee802154_mac_fsm_ctx_t ctx;
    ieee802154_pib_value_t short_addr;

    memset(&ctx, 0, sizeof(ctx));
    ctx.dst_addr = coord_addr;
    ctx.data_dst_addr = coord_addr;
    ctx.dst_mode = coord_mode;
    ctx.dst_panid = coord_panid;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_SHORT_ADDR, &short_addr);
    if (short_addr.v.short_addr.u16 != 0xFFFFU) {
        ctx.src_mode = IEEE802154_ADDR_MODE_SHORT;
    }
    else {
        ctx.src_mode = IEEE802154_ADDR_MODE_EXTENDED;
    }
    ctx.result = &res;

    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_MLME_POLL, &ctx) < 0) {
        return -EBUSY;
    }

    return res;
}

/** @} */
