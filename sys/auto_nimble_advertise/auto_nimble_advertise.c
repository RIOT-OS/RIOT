/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO over UART + NimBLE implementation
 *
 * This file is based on stdio_uart.c and is extended by functionality to access
 * stdio via NimBLE. This module is designed to use UART and NimBLE in parallel.
 * Designed for single device connection.
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "nimble_riot.h"
#include "net/bluetil/ad.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"

#include "auto_nimble_advertise.h"

static struct ble_gap_event_listener _gap_event_listener;

static void _start_advertise(void)
{
    struct ble_gap_adv_params advp;
    int rc;

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER, &advp, NULL, NULL);
    assert(rc == 0);

    (void) rc;
}

static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void) arg;

    switch (event->type) {

        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                _start_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            _start_advertise();
            break;
    }

    return 0;
}

/* is gonna be called by auto_init */
void auto_nimble_advertise_init(void)
{
    int rc = 0;

    /* register gap event listener */
    rc = ble_gap_event_listener_register(&_gap_event_listener, _gap_event_cb, NULL);
    assert(rc == 0);

    /* fix compilation error when using DEVELHELP=0 */
    (void) rc;

    /* set the device name */
    ble_svc_gap_device_name_set(device_name);

    /* configure and set the advertising data */
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    bluetil_ad_t ad;
    bluetil_ad_init_with_flags(&ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
    bluetil_ad_add_name(&ad, device_name);
    ble_gap_adv_set_data(ad.buf, ad.pos);

    /* start to advertise this node */
    _start_advertise();
}
