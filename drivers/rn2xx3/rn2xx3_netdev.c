/*
 * Copyright (C) 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rn2xx3
 * @{
 * @file
 * @brief       Netdev adaption for the rn2xx3 driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "net/netopt.h"
#include "net/netdev.h"
#include "net/loramac.h"

#include "rn2xx3_internal.h"
#include "rn2xx3.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Internal helper functions */
static int _set_state(rn2xx3_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            rn2xx3_sys_sleep(dev);
            break;

        case NETOPT_STATE_RESET:
            rn2xx3_sys_reset(dev);
            break;

        default:
            return -ENOTSUP;
    }

    return sizeof(netopt_state_t);
}

static int _get_state(rn2xx3_t *dev, void *val)
{
    netopt_state_t state;
    switch(dev->int_state) {
        case RN2XX3_INT_STATE_SLEEP:
            state = NETOPT_STATE_SLEEP;
            break;

        case RN2XX3_INT_STATE_CMD:
        case RN2XX3_INT_STATE_MAC_JOIN:
        case RN2XX3_INT_STATE_IDLE:
            state = NETOPT_STATE_STANDBY;
            break;

        case RN2XX3_INT_STATE_MAC_TX:
            state = NETOPT_STATE_TX;
            break;

        case RN2XX3_INT_STATE_MAC_RX_PORT:
        case RN2XX3_INT_STATE_MAC_RX_MESSAGE:
            state = NETOPT_STATE_RX;
            break;

        default:
            break;
    }
    memcpy(val, &state, sizeof(netopt_state_t));
    return sizeof(netopt_state_t);
}

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count)
{
    rn2xx3_t *dev = (rn2xx3_t *) netdev;

    if (dev->int_state != RN2XX3_INT_STATE_IDLE) {
        DEBUG("[WARNING] Cannot send packet: module is not ready .\n");
        return -ENOTSUP;
    }

    /* Start send command */
    rn2xx3_mac_tx_start(dev);

    /* Write payload buffer */
    for (size_t i = 0; i < count; i++) {
        rn2xx3_cmd_append(dev, vector[i].iov_base, vector[i].iov_len);
    }

    /* Finalize the transmission */
    rn2xx3_mac_tx_finalize(dev);

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void) info;
    rn2xx3_t *dev = (rn2xx3_t *) netdev;

    uint8_t size = strlen((char *)dev->rx_buf);

    if (buf == NULL) {
        return size;
    }

    if (size > len) {
        return -ENOBUFS;
    }

    memcpy(buf, dev->rx_buf, size);

    return size;
}

static int _init(netdev_t *netdev)
{
    rn2xx3_t *dev = (rn2xx3_t *) netdev;

    DEBUG("[rn2xx3] netdev: initializing device\n");
    if (rn2xx3_init(dev) != RN2XX3_OK) {
        DEBUG("[rn2xx3] netdev: device initialization failed\n");
        return -1;
    }

    DEBUG("[rn2xx3] netdev: initializing mac layer\n");
    if (rn2xx3_mac_init(dev) != RN2XX3_OK) {
        DEBUG("[rn2xx3] netdev: mac initialization failed\n");
        return -1;
    }

    DEBUG("[rn2xx3] netdev: initialization succeeded\n");

    return 0;
}

static void _isr(netdev_t *netdev)
{
    rn2xx3_t *dev = (rn2xx3_t *)netdev;

    switch (dev->int_state) {
        case RN2XX3_INT_STATE_MAC_RX_MESSAGE:
            DEBUG("[rn2xx3] isr: data available, waiting for read\n");
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            break;

        case RN2XX3_INT_STATE_MAC_TX:
        case RN2XX3_INT_STATE_MAC_RX_PORT:
            DEBUG("[rn2xx3] isr: data sent\n");
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
            break;
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    rn2xx3_t *dev = (rn2xx3_t *) netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch(opt) {
        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            return _get_state(dev, val);

        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *) val) = rn2xx3_mac_get_tx_power(dev);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_DR:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *) val) = rn2xx3_mac_get_dr(dev);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_ADR:
            assert(max_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t *) val) = rn2xx3_mac_get_adr(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
            break;

        case NETOPT_RETRANS:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *) val) = rn2xx3_mac_get_retx(dev);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_RX2_DR:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *) val) = rn2xx3_mac_get_rx2_dr(dev);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_RX2_FREQ:
            assert(max_len >= sizeof(uint32_t));
            *((uint32_t *) val) = rn2xx3_mac_get_rx2_freq(dev);
            return sizeof(uint32_t);

        case NETOPT_LORAWAN_TX_PORT:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *) val) = rn2xx3_mac_get_tx_port(dev);
            return sizeof(uint8_t);

        case NETOPT_ACK_REQ:
            assert(max_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t*) val) = (rn2xx3_mac_get_tx_mode(dev) == LORAMAC_TX_CNF) ? NETOPT_ENABLE : NETOPT_DISABLE;
            break;

        default:
            break;
    }

    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    rn2xx3_t *dev = (rn2xx3_t *) netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch(opt) {
        case NETOPT_STATE:
            assert(len <= sizeof(netopt_state_t));
            return _set_state(dev, *((const netopt_state_t *) val));

        case NETOPT_TX_POWER:
            assert(len <= sizeof(uint8_t));
            uint8_t poweridx = *((const uint8_t *)val);
            if (poweridx > LORAMAC_TX_PWR_15) {
                res = -EINVAL;
                break;
            }
            rn2xx3_mac_set_tx_power(dev, poweridx);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_DR:
            assert(len <= sizeof(uint8_t));
            uint8_t dr = *((const uint8_t *)val);
            if (dr > LORAMAC_DR_15) {
                res = -EINVAL;
                break;
            }
            rn2xx3_mac_set_dr(dev, dr);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_ADR:
            assert(len <= sizeof(netopt_enable_t));
            rn2xx3_mac_set_adr(dev, *((const netopt_enable_t *) val) ? true : false);
            return sizeof(netopt_enable_t);

        case NETOPT_LORAWAN_RX2_DR:
            assert(len <= sizeof(uint8_t));
            uint8_t rx2_dr = *((const uint8_t *)val);
            if (rx2_dr > LORAMAC_DR_15) {
                res = -EINVAL;
                break;
            }
            rn2xx3_mac_set_rx2_dr(dev, rx2_dr);
            return sizeof(uint8_t);

        case NETOPT_LORAWAN_RX2_FREQ:
            assert(len <= sizeof(uint32_t));
            rn2xx3_mac_set_rx2_freq(dev, *((const uint32_t *) val));
            return sizeof(uint32_t);

        case NETOPT_LORAWAN_TX_PORT:
            assert(len <= sizeof(uint8_t));
            uint8_t port = *((const uint8_t *)val);
            if (port < LORAMAC_PORT_MIN ||
                port > LORAMAC_PORT_MAX) {
                res = -EINVAL;
                break;
            }
            rn2xx3_mac_set_tx_port(dev, port);
            return sizeof(uint8_t);

        case NETOPT_ACK_REQ:
            assert(len <= sizeof(netopt_enable_t));
            rn2xx3_mac_set_tx_mode(dev, *((const netopt_enable_t *) val) ? LORAMAC_TX_CNF : LORAMAC_TX_UNCNF);
            return sizeof(netopt_enable_t);

        default:
            break;

    }

    return res;
}

const netdev_driver_t rn2xx3_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};
