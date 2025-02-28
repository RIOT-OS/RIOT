/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx126x
 * @{
 * @file
 * @brief       Netdev adaptation for the SX1261/2/8 and LLCC68 driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "architecture.h"
#include "iolist.h"
#include "net/netopt.h"
#include "net/netdev.h"
#include "net/netdev/lora.h"
#include "net/lora.h"

#include "sx126x.h"
#include "sx126x_netdev.h"
#include "sx126x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

const uint8_t llcc68_max_sf = LORA_SF11;
const uint8_t sx126x_max_sf = LORA_SF12;

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);

    netopt_state_t state;

    netdev->driver->get(netdev, NETOPT_STATE, &state, sizeof(uint8_t));
    if (state == NETOPT_STATE_TX) {
        SX126X_DEBUG(dev, "netdev: cannot send packet, radio is already transmitting.\n");
        return -ENOTSUP;
    }

    size_t pos = 0;

    /* Write payload buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len > 0) {
            sx126x_write_buffer(dev, pos, iol->iol_base, iol->iol_len);
            SX126X_DEBUG(dev, "netdev: send: wrote data to payload buffer.\n");
            pos += iol->iol_len;
        }
    }

    /* Ignore send if packet size is 0 */
    if (!pos) {
        return 0;
    }

    SX126X_DEBUG(dev, "netdev: sending packet now (size: %" PRIuSIZE ").\n", pos);
    sx126x_set_lora_payload_length(dev, pos);

    state = NETOPT_STATE_TX;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(uint8_t));
    SX126X_DEBUG(dev, "netdev: send: transmission in progress.\n");

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);
    SX126X_DEBUG(dev, "netdev: read received data.\n");
    uint8_t size = 0;
    netdev_lora_rx_info_t *packet_info = info;

    if (packet_info) {
        sx126x_pkt_status_lora_t pkt_status;
        sx126x_get_lora_pkt_status(dev, &pkt_status);
        packet_info->snr = pkt_status.snr_pkt_in_db;
        packet_info->rssi = pkt_status.rssi_pkt_in_dbm;
    }

    sx126x_rx_buffer_status_t rx_buffer_status;

    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);

    size = rx_buffer_status.pld_len_in_bytes;

    if (buf == NULL) {
        return size;
    }

    if (size > len) {
        return -ENOBUFS;
    }

    sx126x_read_buffer(dev, rx_buffer_status.buffer_start_pointer, buf, size);

    return size;
}

static int _init(netdev_t *netdev)
{
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);

    /* Launch initialization of driver and device */
    SX126X_DEBUG(dev, "netdev: initializing driver...\n");
    if (sx126x_init(dev) != 0) {
        SX126X_DEBUG(dev, "netdev: initialization failed\n");
        return -1;
    }

    SX126X_DEBUG(dev, "netdev: initialization successful\n");

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static void _isr(netdev_t *netdev)
{
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);

    sx126x_irq_mask_t irq_mask;

    sx126x_get_and_clear_irq_status(dev, &irq_mask);

    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
#endif
    }

    if (irq_mask & SX126X_IRQ_TX_DONE) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_TX_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else if (irq_mask & SX126X_IRQ_RX_DONE) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_RX_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    else if (irq_mask & SX126X_IRQ_PREAMBLE_DETECTED) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_PREAMBLE_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_SYNC_WORD_VALID) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_SYNC_WORD_VALID\n");
    }
    else if (irq_mask & SX126X_IRQ_HEADER_VALID) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_HEADER_VALID\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
    }
    else if (irq_mask & SX126X_IRQ_HEADER_ERROR) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_HEADER_ERROR\n");
    }
    else if (irq_mask & SX126X_IRQ_CRC_ERROR) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_CRC_ERROR\n");
        netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DONE) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_CAD_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_CAD_DONE);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DETECTED) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_CAD_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_TIMEOUT) {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_TIMEOUT\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_TIMEOUT);
    }
    else {
        SX126X_DEBUG(dev, "netdev: SX126X_IRQ_NONE\n");
    }
}

static int _get_state(sx126x_t *dev, void *val)
{
    netopt_state_t state;
    sx126x_chip_modes_t mode = sx126x_get_state(dev);
    SX126X_DEBUG(dev, "netdev: state: %d\n", mode);
    switch (mode) {
    case SX126X_CHIP_MODE_FS:
        state = NETOPT_STATE_IDLE;
        break;

    case SX126X_CHIP_MODE_TX:
        state = NETOPT_STATE_TX;
        break;

    case SX126X_CHIP_MODE_RX:
        state = NETOPT_STATE_RX;
        break;

    default:
        state = NETOPT_STATE_STANDBY;
        break;
    }
    memcpy(val, &state, sizeof(netopt_state_t));
    return sizeof(netopt_state_t);
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    (void)max_len; /* unused when compiled without debug, assert empty */
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
    case NETOPT_STATE:
        assert(max_len >= sizeof(netopt_state_t));
        return _get_state(dev, val);

    case NETOPT_DEVICE_TYPE:
        assert(max_len >= sizeof(uint16_t));
        *((uint16_t *)val) = NETDEV_TYPE_LORA;
        return sizeof(uint16_t);

    case NETOPT_CHANNEL_FREQUENCY:
        assert(max_len >= sizeof(uint32_t));
        *((uint32_t *)val) = sx126x_get_channel(dev);
        return sizeof(uint32_t);

    case NETOPT_BANDWIDTH:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx126x_get_bandwidth(dev);
        return sizeof(uint8_t);

    case NETOPT_SPREADING_FACTOR:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx126x_get_spreading_factor(dev);
        return sizeof(uint8_t);

    case NETOPT_CODING_RATE:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx126x_get_coding_rate(dev);
        return sizeof(uint8_t);

    case NETOPT_PDU_SIZE:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx126x_get_lora_payload_length(dev);
        return sizeof(uint8_t);

    case NETOPT_INTEGRITY_CHECK:
        assert(max_len >= sizeof(netopt_enable_t));
        *((netopt_enable_t *)val) = sx126x_get_lora_crc(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_RANDOM:
        assert(max_len >= sizeof(uint32_t));
        sx126x_get_random_numbers(dev, val, 1);
        return sizeof(uint32_t);

    case NETOPT_IQ_INVERT:
        assert(max_len >= sizeof(uint8_t));
        *((netopt_enable_t *)val) = sx126x_get_lora_iq_invert(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_RSSI:
        assert(max_len >= sizeof(int16_t));
        sx126x_get_rssi_inst(dev, ((int16_t *)val));
        return sizeof(int16_t);

    default:
        break;
    }

    return -ENOTSUP;
}

static int _set_state(sx126x_t *dev, netopt_state_t state)
{
    switch (state) {
    case NETOPT_STATE_STANDBY:
        SX126X_DEBUG(dev, "netdev: set NETOPT_STATE_STANDBY state\n");
        sx126x_set_state(dev, SX126X_CHIP_MODE_STBY_XOSC);
        break;

    case NETOPT_STATE_IDLE:
    case NETOPT_STATE_RX:
        SX126X_DEBUG(dev, "netdev: set NETOPT_STATE_RX state\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        /* Refer Section 4.2 RF Switch in Application Note (AN5406) */
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, SX126X_RF_MODE_RX);
        }
#endif
        sx126x_set_state(dev, SX126X_CHIP_MODE_RX);
        break;

    case NETOPT_STATE_TX:
        SX126X_DEBUG(dev, "netdev: set NETOPT_STATE_TX state\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, dev->params->tx_pa_mode);
        }
#endif
        sx126x_set_state(dev, SX126X_CHIP_MODE_TX);
        break;

    case NETOPT_STATE_RESET:
        SX126X_DEBUG(dev, "netdev: set NETOPT_STATE_RESET state\n");
        sx126x_reset(dev);
        break;

    default:
        return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    (void)len; /* unused when compiled without debug, assert empty */
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
    case NETOPT_STATE:
        assert(len == sizeof(netopt_state_t));
        return _set_state(dev, *((const netopt_state_t *)val));

    case NETOPT_DEVICE_TYPE:
        assert(len <= sizeof(uint16_t));
        /* Only LoRa modem is supported for the moment */
        if (*(const uint16_t *)val == NETDEV_TYPE_LORA) {
            sx126x_set_pkt_type(dev, SX126X_PKT_TYPE_LORA);
            return sizeof(uint16_t);
        }
        else {
            return -EINVAL;
        }

    case NETOPT_CHANNEL_FREQUENCY:
        assert(len <= sizeof(uint32_t));
        sx126x_set_channel(dev, *((const uint32_t *)val));
        return sizeof(uint32_t);

    case NETOPT_SINGLE_RECEIVE:
        assert(len <= sizeof(netopt_enable_t));
        netopt_enable_t single_rx = *((const netopt_enable_t *)val);
        if (single_rx) {
            dev->rx_timeout = 0;
        }
        else {
            dev->rx_timeout = -1;
        }
        return sizeof(netopt_enable_t);

    case NETOPT_BANDWIDTH:
        assert(len <= sizeof(uint8_t));
        uint8_t bw = *((const uint8_t *)val);
        if (bw > LORA_BW_500_KHZ) {
            res = -EINVAL;
            break;
        }
        sx126x_set_bandwidth(dev, bw);
        return sizeof(uint8_t);

    case NETOPT_SPREADING_FACTOR:
        assert(len <= sizeof(uint8_t));
        uint8_t sf = *((const uint8_t *)val);
        const uint8_t max_sf = sx126x_is_llcc68(dev)
                               ? llcc68_max_sf
                               : sx126x_max_sf;
        if ((sf < LORA_SF5) || (sf > max_sf)) {
            res = -EINVAL;
            break;
        }
        sx126x_set_spreading_factor(dev, sf);
        return sizeof(uint8_t);

    case NETOPT_CODING_RATE:
        assert(len <= sizeof(uint8_t));
        uint8_t cr = *((const uint8_t *)val);
        if ((cr < LORA_CR_4_5) || (cr > LORA_CR_4_8)) {
            res = -EINVAL;
            break;
        }
        sx126x_set_coding_rate(dev, cr);
        return sizeof(uint8_t);

    case NETOPT_PDU_SIZE:
        assert(len <= sizeof(uint8_t));
        sx126x_set_lora_payload_length(dev, *((const uint8_t *)val));
        return sizeof(uint8_t);

    case NETOPT_INTEGRITY_CHECK:
        assert(len <= sizeof(netopt_enable_t));
        sx126x_set_lora_crc(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_RX_SYMBOL_TIMEOUT:
        assert(len <= sizeof(int32_t));
        int32_t timeout = *((const int32_t *)val);
        if (timeout >= 0) {
            dev->rx_timeout = *(const int32_t *)val;
            return sizeof(int32_t);
        }
        else {
            res = -EINVAL;
            break;
        }

    case NETOPT_TX_POWER:
        assert(len <= sizeof(int16_t));
        int16_t power = *((const int16_t *)val);
        if ((power < SX126X_POWER_MIN) || (power > SX126X_POWER_MAX)) {
            res = -EINVAL;
            break;
        }
        sx126x_set_tx_power(dev, power, CONFIG_SX126X_RAMP_TIME_DEFAULT);
        return sizeof(int16_t);

    case NETOPT_FIXED_HEADER:
        assert(len <= sizeof(netopt_enable_t));
        sx126x_set_lora_implicit_header(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_PREAMBLE_LENGTH:
        assert(len <= sizeof(uint16_t));
        sx126x_set_lora_preamble_length(dev, *((const uint16_t *)val));
        return sizeof(uint16_t);

    case NETOPT_SYNCWORD:
        assert(len <= sizeof(uint8_t));
        sx126x_set_lora_sync_word(dev, *((uint8_t *)val));
        return sizeof(uint8_t);

    case NETOPT_IQ_INVERT:
        assert(len <= sizeof(netopt_enable_t));
        sx126x_set_lora_iq_invert(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(bool);

    default:
        break;
    }

    return res;
}

const netdev_driver_t sx126x_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _event_cb(void *arg)
{
    netdev_trigger_event_isr(arg);
}

void sx126x_setup(sx126x_t *dev, const sx126x_params_t *params, uint8_t index)
{
    memset((uint8_t *)dev + sizeof(dev->netdev), 0, sizeof(*dev) - sizeof(dev->netdev));
    netdev_t *netdev = &dev->netdev;
    netdev->driver = &sx126x_driver;
    dev->params = (sx126x_params_t *)params;
    dev->event_cb = _event_cb;
    dev->event_arg = netdev;
    netdev_register(&dev->netdev, NETDEV_SX126X, index);
#if IS_USED(MODULE_SX126X_STM32WL)
#if SX126X_NUMOF == 1
    extern sx126x_t *sx126x_stm32wl = dev;
#endif
#endif
}
