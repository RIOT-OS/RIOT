/*
 * Copyright (C) 2020 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ble_rshell.h"

#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"
#include "nimble_riot.h"
#include "net/bluetil/ad.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "os/os_mbuf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int ble_shell_gatt_cb(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static int start_advertise(ble_rshell_t *ctx);

/**
 * The vendor specific "bleuart" service consists of one write no-rsp characteristic
 * and one notification only read charateristic
 *     o "write no-rsp": a single-byte characteristic that can be written only
 *       over a non-encrypted connection
 *     o "read": a single-byte characteristic that can always be read only via
 *       notifications
 */

/* {6E400001-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_svc_uart_uuid =
BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

/* {6E400002-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_write_uuid =
BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);


/* {6E400003-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_read_uuid =
BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

int ble_rshell_rx_cb(ble_rshell_t *ctx, const char *data, uint8_t len);

static void _disconnect_client(ble_rshell_t *ctx, struct ble_gap_event *event)
{
    (void)event;

    if (!ctx->conn_handle) {
        return;
    }

    rshell_disconnect_client(&ctx->rshell);

    ctx->conn_handle = 0;
}

static void _accept_client(ble_rshell_t *ctx, struct ble_gap_event *event)
{
    ctx->conn_handle = event->connect.conn_handle;

    rshell_accept_client(&ctx->rshell);
}

static int ble_rshell_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    ble_rshell_t *ctx = arg;

    int rc;
    struct ble_gap_conn_desc desc;

    switch (event->type) {
        /* A new connection was established or a connection attempt failed. */
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
                assert(rc == 0);

                if (ctx->conn_handle) {
                    _disconnect_client(ctx, event);
                }

                _accept_client(ctx, event);
            }
            if (event->connect.status) {
                _disconnect_client(ctx, event);
                start_advertise(ctx);
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            _disconnect_client(ctx, event);
            start_advertise(ctx);
            break;
    }

    return 0;
}

static int start_advertise(ble_rshell_t *ctx)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info).
     *     o Advertising tx power.
     *     o 128 bit UUID
     */

    memset(&fields, 0, sizeof fields);

    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assiging the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.uuids128 = BLE_UUID128(&gatt_svr_svc_uart_uuid.u);
    fields.num_uuids128 = 1;
    fields.uuids128_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        return rc;
    }

    memset(&fields, 0, sizeof fields);
    fields.name = (uint8_t *)ble_svc_gap_device_name();
    fields.name_len = strlen((char *)fields.name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&fields);
    if (rc != 0) {
        return rc;
    }

    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_rshell_gap_event_cb, ctx);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static int ble_shell_gatt_cb(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{

    (void) conn_handle;
    (void) attr_handle;
    ble_rshell_t *ctx = arg;

    int rc = 0;

    ble_uuid_t* write_uuid = (ble_uuid_t*) &gatt_svr_chr_uart_write_uuid.u;

    if (ble_uuid_cmp(ctxt->chr->uuid, write_uuid) == 0) {

        switch (ctxt->op) {

            case BLE_GATT_ACCESS_OP_WRITE_CHR:
            {
                uint16_t om_len;
                om_len = OS_MBUF_PKTLEN(ctxt->om);

                /* read sent data */
                char buf[om_len];
                rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &om_len);

                /* fire callback */
                rc = ble_rshell_rx_cb(ctx, buf, om_len);
            }
            break;

            default:
                puts("unhandled operation!");
                rc = 1;
                break;
        }

        return rc;
    }

    puts("unhandled uuid!");
    return 1;
}

/* we call this to send data to the shell that we received over BLE */
int ble_rshell_rx_cb(ble_rshell_t *ctx, const char *data, uint8_t len)
{
    return rshell_rx_cb(&ctx->rshell, data, len);
}

/* this is run from rshell thread context to give us data to send over BLE */
void ble_rshell_tx_cb(const char *data, uint8_t len, void *arg)
{
    ble_rshell_t *ctx = arg;

    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    if (!om) {
        return;
    }

    /* send ble notification */
    ble_gattc_notify_custom(ctx->conn_handle, ctx->read_handle, om);

    /* wait for transmission to complete */
    xtimer_usleep(50 * US_PER_MS);
}

void _configure_gatt(ble_rshell_t *ctx)
{
    /* configure characteristics */
    memset(ctx->characteristics, 0, sizeof(ctx->characteristics));

    /* read characteristic */
    ctx->characteristics[0].uuid = &gatt_svr_chr_uart_read_uuid.u;
    ctx->characteristics[0].access_cb = ble_shell_gatt_cb;
    ctx->characteristics[0].val_handle = &ctx->read_handle;
    ctx->characteristics[0].arg = ctx;
    ctx->characteristics[0].flags = BLE_GATT_CHR_F_NOTIFY;

    /* write characteristic */
    ctx->characteristics[1].uuid = &gatt_svr_chr_uart_write_uuid.u;
    ctx->characteristics[1].access_cb = ble_shell_gatt_cb;
    ctx->characteristics[1].val_handle = &ctx->write_handle;
    ctx->characteristics[1].arg = ctx;
    ctx->characteristics[1].flags =
                            BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP;

    /* configure gatt */
    memset(ctx->gatt_svr_svcs, 0, sizeof(ctx->gatt_svr_svcs));
    ctx->gatt_svr_svcs[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    ctx->gatt_svr_svcs[0].uuid = &gatt_svr_svc_uart_uuid.u;
    ctx->gatt_svr_svcs[0].characteristics = ctx->characteristics;
}

int ble_rshell_init(ble_rshell_t *ctx)
{
    int rc;

    ctx->conn_handle = 0;

    /* initialize the rshell */
    shell_command_t *shell_commands = NULL;
    rc = rshell_init(&ctx->rshell, ble_rshell_tx_cb, ctx, shell_commands);
    assert(!rc);

    /* setup gatt characteristics */
    _configure_gatt(ctx);

    /* verify and add our custom services */
    rc = ble_gatts_count_cfg(ctx->gatt_svr_svcs);
    assert(!rc);
    rc = ble_gatts_add_svcs(ctx->gatt_svr_svcs);
    assert(!rc);

    /* set the device name */
    ble_svc_gap_device_name_set(ctx->device_name);
    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* configure and set the advertising data */
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    bluetil_ad_t ad;
    bluetil_ad_init_with_flags(&ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
    rc = bluetil_ad_add_name(&ad, ctx->device_name);
    assert(!rc);
    rc = ble_gap_adv_set_data(ad.buf, ad.pos);
    assert(!rc);

    /* start to advertise this node */
    rc = start_advertise(ctx);
    assert(!rc);

    return rc;
}
