/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-FileCopyrightText: 2020-2022 Université Grenoble Alpes
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sx1280
 * @{
 * @file
 * @brief       Netdev adaptation for the LoRa SX1280 Driver driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "iolist.h"
#include "net/netopt.h"
#include "net/netdev.h"
#include "net/netdev/lora.h"
#include "net/lora.h"

#include "sx1280.h"
#include "sx1280_params.h"

#include "sx1280_netdev.h"
#include "sx1280_driver/src/sx1280.h"

#include "smtc_ral/src/ral_defs.h"
#include "smtc_ral/src/ral_sx1280.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _send(netdev_t *netdev, const iolist_t *iolist)
{

    sx1280_t *dev = (sx1280_t *)netdev;
    netopt_state_t state;

    netdev->driver->get(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    if (state == NETOPT_STATE_TX) {
        DEBUG("[sx1280] netdev: cannot send packet, radio is already transmitting.\n");
        return -ENOTSUP;
    }
    else {
        DEBUG("[sx1280] netdev: can send packet\n");
    }

    size_t pos = 0;

    /* Write payload buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len > 0) {
            /* write data to payload buffer */
            sx1280_write_buffer(dev, pos, iol->iol_base, iol->iol_len);
            DEBUG("[sx1280] netdev: send: wrote data to payload buffer.\n");
            pos += iol->iol_len;
        }
    }
    /* Ignore send if packet size is 0 */
    if (!pos) {
        return 0;
    }

    DEBUG("[sx1280] netdev: sending packet now (size: %" PRIuSIZE ").\n", pos);
    sx1280_set_lora_payload_length(dev, pos);

    state = NETOPT_STATE_TX;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
    DEBUG("[sx1280] netdev: send: transmission in progress.\n");

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    DEBUG("[sx1280] netdev: read received data.\n");

    sx1280_t *dev = (sx1280_t *)netdev;
    uint8_t size = 0;

    /* Get received packet info and size here */
    netdev_lora_rx_info_t *packet_info = info;

    if (packet_info) {
        sx1280_pkt_status_lora_t pkt_status;
        sx1280_get_lora_pkt_status(dev, &pkt_status);
        packet_info->snr = pkt_status.snr;
        packet_info->rssi = pkt_status.rssi;
    }

    sx1280_rx_buffer_status_t rx_buffer_status;

    sx1280_get_rx_buffer_status(dev, &rx_buffer_status);

    size = rx_buffer_status.pld_len_in_bytes;

    if (buf == NULL) {
        return size;
    }

    if (size > len) {
        return -ENOBUFS;
    }

    /* Read the received packet content here and write it to buf */
    sx1280_read_buffer(dev, rx_buffer_status.buffer_start_pointer, buf, size);

    /* RX SINGLE MODE */
    if (sx1280_hal_get_operating_mode(&dev) == SX1280_HAL_OP_MODE_RX) {
        DEBUG("[sx1280] netdev stop RX single mode" );
        netopt_state_t state = NETOPT_STATE_IDLE;
        dev->netdev.driver->set(&dev->netdev, NETOPT_STATE, &state, sizeof(state));
        return 0;
        /* RX CONTINUOUS MODE */
    }
    else if (sx1280_hal_get_operating_mode(&dev) == SX1280_HAL_OP_MODE_RX_C) {
        DEBUG("[sx1280] netdev keep RX continuous mode" );
        netopt_state_t state = NETOPT_STATE_RX;
        dev->netdev.driver->set(&dev->netdev, NETOPT_STATE, &state, sizeof(state));
        return size;
    }
    else {
        DEBUG("[sx1280] netdev NOT IMPLEMENTED");
        return 0;
    }
}

static int _init(netdev_t *netdev)
{
    sx1280_t *dev = (sx1280_t *)netdev;

    /* Launch initialization of driver and device */
    DEBUG("[sx1280] netdev: initializing driver...\n");
    if (sx1280_init(dev) != 0) {
        DEBUG("[sx1280] netdev: initialization failed\n");
        return -1;
    }

    DEBUG("[sx1280] netdev: initialization successful\n");

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static void _isr(netdev_t *netdev)
{
    sx1280_t *dev = (sx1280_t *)netdev;
    sx1280_irq_mask_t irq_mask;

    ral_sx1280_get_and_clear_irq_status(&dev->ral, &irq_mask);

    if (irq_mask & RAL_IRQ_RX_DONE) {
        if (irq_mask & RAL_IRQ_RX_CRC_ERROR) {
            DEBUG("[sx1280] netdev: SX1280_IRQ_CRC_ERROR\n");
            netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
        }
        else {
            DEBUG("[sx1280] netdev: SX1280_IRQ_RX_DONE\n");
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
    }
    else if (irq_mask & RAL_IRQ_TX_DONE) {
        DEBUG("[sx1280] netdev: SX1280_IRQ_TX_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else if (irq_mask & RAL_IRQ_RX_HDR_OK) {
        DEBUG("[sx1280] netdev: SX1280_IRQ_HEADER_VALID\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
    }
    else if (irq_mask & RAL_IRQ_RX_HDR_ERROR) {
        DEBUG("[sx1280] netdev: SX1280_IRQ_HEADER_ERROR\n");
    }
    else if (irq_mask & RAL_IRQ_RX_TIMEOUT) {
        DEBUG("[sx1280x] netdev: SX1280_IRQ_RX_TIMEOUT\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_TIMEOUT);
    }
}

static int _get_state(sx1280_t *dev, void *val)
{
    (void)dev;
    netopt_state_t state = NETOPT_STATE_OFF;
    sx1280_chip_status_t radio_status;

    sx1280_get_status(dev, &radio_status);

    switch (radio_status.chip_mode) {

    case SX1280_CHIP_MODE_STBY_RC:
    /* Intentional fall-through */
    case SX1280_CHIP_MODE_STBY_XOSC:
        state = NETOPT_STATE_STANDBY;
        DEBUG("NETOPT_STATE_STANDBY ");
        break;

    case SX1280_CHIP_MODE_RX:
        state = NETOPT_STATE_RX;
        DEBUG("NETOPT_STATE_RX ");
        break;
    case SX1280_CHIP_MODE_TX:
        state = NETOPT_STATE_TX;
        DEBUG("NETOPT_STATE_TX ");
        break;
    case SX1280_CHIP_MODE_FS:
        DEBUG("SX1280_CHIP_MODE_FS");
    /* Intentional fall-through */
    default:
        DEBUG("default");
        break;
    }
    memcpy(val, &state, sizeof(netopt_state_t));
    return sizeof(netopt_state_t);
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    (void)max_len; /* unused when compiled without debug, assert empty */
    sx1280_t *dev = (sx1280_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
    case NETOPT_DEVICE_TYPE:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)val) = NETDEV_TYPE_LORA;
        return sizeof(uint16_t);
    case NETOPT_STATE:
        assert(max_len >= sizeof(netopt_state_t));
        return _get_state(dev, val);

    case NETOPT_CHANNEL_FREQUENCY:
        assert(max_len >= sizeof(uint32_t));
        *((uint32_t *)val) = sx1280_get_channel(dev);
        return sizeof(uint32_t);

    case NETOPT_BANDWIDTH:
        assert(max_len >= sizeof(uint32_t));
        *((uint32_t *)val) = sx1280_get_bandwidth(dev);
        return sizeof(uint32_t);

    case NETOPT_SPREADING_FACTOR:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx1280_get_spreading_factor(dev);
        return sizeof(uint8_t);

    case NETOPT_CODING_RATE:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx1280_get_coding_rate(dev);
        return sizeof(uint8_t);

    case NETOPT_PDU_SIZE:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx1280_get_lora_payload_length(dev);
        return sizeof(uint8_t);

    case NETOPT_INTEGRITY_CHECK:
        assert(max_len >= sizeof(netopt_enable_t));
        *((netopt_enable_t *)val) = sx1280_get_lora_crc(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_IQ_INVERT:
        assert(max_len >= sizeof(uint8_t));
        *((netopt_enable_t *)val) = sx1280_get_lora_iq_invert(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_RSSI:
        assert(max_len >= sizeof(int16_t));
        sx1280_get_rssi_inst(dev, ((int16_t *)val));
        return sizeof(int16_t);

    default:
        break;
    }

    return -ENOTSUP;
}

static int _set_state(sx1280_t *dev, netopt_state_t state)
{
    (void)dev;
    switch (state) {
    case NETOPT_STATE_STANDBY:
        DEBUG("[sx1280] netdev: set NETOPT_STATE_STANDBY state\n");
        sx1280_set_standby(dev, SX1280_STANDBY_CFG_RC);
        break;

    case NETOPT_STATE_IDLE:
    /* intentional fall-throught */
    case NETOPT_STATE_RX:
        DEBUG("[sx1280] netdev: set NETOPT_STATE_RX state\n");
        sx1280_set_rx(dev->ral.context, SX1280_TICK_SIZE_1000_US, SX1280_RX_MODE );
        break;

    case NETOPT_STATE_TX:
        DEBUG("[sx1280] netdev: set NETOPT_STATE_TX state\n");
        sx1280_set_tx(dev->ral.context, SX1280_TICK_SIZE_1000_US, 0 );
        break;

    case NETOPT_STATE_RESET:
        DEBUG("[sx1280] netdev: set NETOPT_STATE_RESET state\n");
        sx1280_reset(dev);
        sx1280_init(dev);
        break;

    default:
        return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    (void)len; /* unused when compiled without debug, assert empty */
    sx1280_t *dev = (sx1280_t *)netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
    case NETOPT_STATE:
        assert(len == sizeof(netopt_state_t));
        return _set_state(dev, *((const netopt_state_t *)val));

    case NETOPT_CHANNEL_FREQUENCY:
        assert(len <= sizeof(uint32_t));
        uint32_t params_lora_bw = sx1280_get_bandwidth(dev);

        uint32_t freq = *((const uint32_t *)val);
        uint32_t min_freq = (uint32_t)LORA_ISM2400_FREQUENCY_LOW + (params_lora_bw / 2);
        uint32_t max_freq = (uint32_t)LORA_ISM2400_FREQUENCY_HIGH - (params_lora_bw / 2);

        if (freq < min_freq || freq > max_freq) {
            res = -EINVAL;
            break;
        }
        sx1280_set_channel(dev, *((const uint32_t *)val));
        return sizeof(uint32_t);

    case NETOPT_BANDWIDTH:
        assert(len <= sizeof(uint16_t));
        uint16_t bw = *((const uint16_t *)val);
        switch (bw) {
        case 200:   /* fall-through */
        case 400:   /* fall-through */
        case 800:   /* fall-through */
        case 1600:
            sx1280_set_bandwidth(dev, bw);
            return sizeof(uint16_t);
        default:
            res = -EINVAL;
            puts("invalid bandwidth, use 200, 400 , 800, 1600");
            break;
        }

        break; /* outer switch*/

    case NETOPT_SPREADING_FACTOR:
        assert(len <= sizeof(uint8_t));
        uint8_t sf = *((const uint8_t *)val);
        if ((sf < RAL_LORA_SF5) || (sf > RAL_LORA_SF12)) {
            res = -EINVAL;
            break;
        }
        sx1280_set_spreading_factor(dev, sf);
        return sizeof(uint8_t);

    case NETOPT_CODING_RATE:
        assert(len <= sizeof(uint8_t));
        uint8_t cr = *((const uint8_t *)val);
        switch (cr) {
        case LORA_CR_4_5:
            cr = RAL_LORA_CR_4_5;
            break;
        case LORA_CR_4_6:
            cr = RAL_LORA_CR_4_6;
            break;
        case LORA_CR_4_7:
            cr = RAL_LORA_CR_4_7;
            break;
        case LORA_CR_4_8:
            cr = RAL_LORA_CR_4_8;
            break;
        case LORA_CR_LI_4_5:
            cr = RAL_LORA_CR_LI_4_5;
            break;
        case LORA_CR_LI_4_6:
            cr = RAL_LORA_CR_LI_4_6;
            break;
        case LORA_CR_LI_4_8:
            cr = RAL_LORA_CR_LI_4_8;
            break;
        default:
            res = -EINVAL;
            puts(
                "invalid cr, use\n \
                LORA_CR_4_5 = 1\n \
                LORA_CR_4_6 = 2\n \
                LORA_CR_4_7 = 3\n \
                LORA_CR_4_8 = 4\n \
                LORA_CR_LI_4_5 = 5\n \
                LORA_CR_LI_4_6 = 6\n \
                LORA_CR_LI_4_8 = 7\n");
            return res;

        }
        sx1280_set_coding_rate(dev, cr);
        return sizeof(uint8_t);

    case NETOPT_PDU_SIZE:
        assert(len <= sizeof(uint8_t));
        sx1280_set_lora_payload_length(dev, *((const uint8_t *)val));
        return sizeof(uint8_t);

    case NETOPT_INTEGRITY_CHECK:
        assert(len <= sizeof(netopt_enable_t));
        sx1280_set_lora_crc(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_PREAMBLE_LENGTH:
        assert(len <= sizeof(uint16_t));
        sx1280_set_lora_preamble_length(dev, *((const uint16_t *)val));
        return sizeof(uint16_t);

    case NETOPT_IQ_INVERT:
        assert(len <= sizeof(netopt_enable_t));
        sx1280_set_lora_iq_invert(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(bool);

    default:
        DEBUG(" OPT unrecognised (missing case) %i", opt);
        break;
    }

    return res;
}

const netdev_driver_t sx1280_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};
