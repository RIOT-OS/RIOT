/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       SAUL-less skald_bthome tests
 *
 * @author      Martine S. Lenders <martine.lenders@tu-dresden.de>
 */

#include <stdio.h>
#include <stdlib.h>

#include "net/netdev/ble.h"
#include "net/skald/bthome.h"
#include "od.h"
#include "ztimer.h"

#define BTHOME_ADV_INTERVAL           (10U)

static skald_bthome_ctx_t _ctx;
static netdev_t _mock_nrfble_dev;
static const netdev_driver_t _mock_netdev_driver;

netdev_t *nrfble_setup(void)
{
    _mock_nrfble_dev.driver = &_mock_netdev_driver;
    _mock_nrfble_dev.event_callback = NULL;
    _mock_nrfble_dev.context = NULL;
    return &_mock_nrfble_dev;
}

static int _mock_nrfble_init(netdev_t *dev)
{
    puts("[mock_nrfble] initialization successful");

    /* signal link UP */
    dev->event_callback(dev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _mock_nrfble_send(netdev_t *dev, const iolist_t *data)
{
    (void)dev;
    assert(data);

    for (unsigned i = 0; data; i++, data = data->iol_next) {
        printf("Data item %u sent:\n", i);
        od_hex_dump(data->iol_base, data->iol_len, OD_WIDTH_DEFAULT);
    }

    return 0;
}

static int _mock_nrfble_get(netdev_t *dev, netopt_t opt, void *val, size_t max_len)
{
    (void)dev;
    (void)max_len;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = NETDEV_TYPE_BLE;
            return sizeof(uint16_t);
        default:
            return -ENOTSUP;
    }
}

static int _mock_nrfble_set(netdev_t *dev, netopt_t opt, const void *val, size_t len)
{
    (void)dev;
    (void)val;
    (void)len;

    switch (opt) {
        case NETOPT_BLE_CTX:
            return sizeof(netdev_ble_ctx_t);
        case NETOPT_TX_POWER:
            assert(len == sizeof(int16_t));
            return sizeof(int16_t);
        default:
            return -ENOTSUP;
    }
}

int main(void)
{
    int res;

    ztimer_sleep(ZTIMER_MSEC, 1000);
    _ctx.skald.pkt.len = 0;
    if ((res = skald_bthome_init(&_ctx, NULL, "Test", 0)) < 0) {
        printf("Unable to initialize BTHome: %s\n", strerror(res));
        return 1;
    }
    if ((res = skald_bthome_add_uint8_measurement(&_ctx, BTHOME_ID_PACKET_ID, 0x42)) < 0) {
        printf("Unable to add packet ID (uint8): %s\n", strerror(res));
        return 1;
    }
    if ((res = skald_bthome_add_int16_measurement(
        &_ctx, BTHOME_ID_TEMPERATURE_FACTOR_0_01, 0x09c4
    )) < 0) {
        printf("Unable to add temperature (factor 0.01, sint16) measurement: %s\n", strerror(res));
        return 1;
    }
    if ((res = skald_bthome_add_uint16_measurement(
        &_ctx, BTHOME_ID_HUMIDITY_FACTOR_0_01, 0x157c
    )) < 0) {
        printf("Unable to add humidity (factor 0.01, uint16) measurement: %s\n", strerror(res));
        return 1;
    }
    if ((res = skald_bthome_add_uint24_measurement(
        &_ctx, BTHOME_ID_PRESSURE_FACTOR_0_01, 0x8f58da
    )) < 0) {
        printf("Unable to add humidity (factor 0.01, uint24) measurement: %s\n", strerror(res));
        return 1;
    }
    // if ((res = skald_bthome_add_int8_measurement(
    //     &_ctx, BTHOME_ID_TEMPERATURE_FACTOR_1, 0x12
    // )) < 0) {
    //     printf("Unable to add temperature factor 1 measurement: %s\n", strerror(res));
    //     return 1;
    // }
    skald_bthome_advertise(&_ctx, BTHOME_ADV_INTERVAL);
    while (1) {}
    return 0;
}

static const netdev_driver_t _mock_netdev_driver = {
    .send = _mock_nrfble_send,
    .recv = NULL,
    .init = _mock_nrfble_init,
    .isr  = NULL,
    .get  = _mock_nrfble_get,
    .set  = _mock_nrfble_set
};

/** @} */
