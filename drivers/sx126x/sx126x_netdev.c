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
#include <string.h>
#include <errno.h>

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

#if IS_USED(MODULE_SX126X_STM32WL)
static netdev_t *_dev;

void isr_subghz_radio(void)
{
    /* Disable NVIC to avoid ISR conflict in CPU. */
    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    NVIC_ClearPendingIRQ(SUBGHZ_Radio_IRQn);
    netdev_trigger_event_isr(_dev);
    cortexm_isr_end();
}
#endif

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);

    netopt_state_t state;

    netdev->driver->get(netdev, NETOPT_STATE, &state, sizeof(uint8_t));
    if (state == NETOPT_STATE_TX) {
        DEBUG("[sx126x] netdev: cannot send packet, radio is already transmitting.\n");
        return -ENOTSUP;
    }

    size_t pos = 0;

    /* Write payload buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len > 0) {
            sx126x_write_buffer(dev, pos, iol->iol_base, iol->iol_len);
            DEBUG("[sx126x] netdev: send: wrote data to payload buffer.\n");
            pos += iol->iol_len;
        }
    }

    /* Ignore send if packet size is 0 */
    if (!pos) {
        return 0;
    }

    DEBUG("[sx126x] netdev: sending packet now (size: %d).\n", pos);
    sx126x_set_lora_payload_length(dev, pos);

    state = NETOPT_STATE_TX;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(uint8_t));
    DEBUG("[sx126x] netdev: send: transmission in progress.\n");

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    DEBUG("[sx126x] netdev: read received data.\n");

    sx126x_t *dev = container_of(netdev, sx126x_t, netdev);
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

    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        _dev = netdev;
#endif
    }

    /* Launch initialization of driver and device */
    DEBUG("[sx126x] netdev: initializing driver...\n");
    if (sx126x_init(dev) != 0) {
        DEBUG("[sx126x] netdev: initialization failed\n");
        return -1;
    }

    DEBUG("[sx126x] netdev: initialization successful\n");
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
        DEBUG("[sx126x] netdev: SX126X_IRQ_TX_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else if (irq_mask & SX126X_IRQ_RX_DONE) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_RX_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    else if (irq_mask & SX126X_IRQ_PREAMBLE_DETECTED) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_PREAMBLE_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_SYNC_WORD_VALID) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_SYNC_WORD_VALID\n");
    }
    else if (irq_mask & SX126X_IRQ_HEADER_VALID) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_HEADER_VALID\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
    }
    else if (irq_mask & SX126X_IRQ_HEADER_ERROR) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_HEADER_ERROR\n");
    }
    else if (irq_mask & SX126X_IRQ_CRC_ERROR) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_CRC_ERROR\n");
        netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DONE) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_CAD_DONE\n");
        netdev->event_callback(netdev, NETDEV_EVENT_CAD_DONE);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DETECTED) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_CAD_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_TIMEOUT) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_TIMEOUT\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_TIMEOUT);
    }
    else {
        DEBUG("[sx126x] netdev: SX126X_IRQ_NONE\n");
    }
}

static int _get_state(sx126x_t *dev, void *val)
{
    sx126x_chip_status_t radio_status;

    sx126x_get_status(dev, &radio_status);
    netopt_state_t state = NETOPT_STATE_OFF;

    switch (radio_status.chip_mode) {
    case SX126X_CHIP_MODE_RFU:
    case SX126X_CHIP_MODE_STBY_RC:
    case SX126X_CHIP_MODE_STBY_XOSC:
        state = NETOPT_STATE_STANDBY;
        break;

    case SX126X_CHIP_MODE_TX:
        state = NETOPT_STATE_TX;
        break;

    case SX126X_CHIP_MODE_RX:
        state = NETOPT_STATE_RX;
        break;

    default:
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
        sx126x_pkt_type_t pkt_type;
        sx126x_get_pkt_type(dev, &pkt_type);
        *((uint16_t *)val) =
            (pkt_type == SX126X_PKT_TYPE_LORA) ? NETDEV_TYPE_LORA : NETDEV_TYPE_UNKNOWN;
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
        DEBUG("[sx126x] netdev: set NETOPT_STATE_STANDBY state\n");
        sx126x_set_standby(dev, SX126X_CHIP_MODE_STBY_XOSC);
        break;

    case NETOPT_STATE_IDLE:
    case NETOPT_STATE_RX:
        DEBUG("[sx126x] netdev: set NETOPT_STATE_RX state\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        /* Refer Section 4.2 RF Switch in Application Note (AN5406) */
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, SX126X_RF_MODE_RX);
        }
#endif
        sx126x_cfg_rx_boosted(dev, true);
        int _timeout = (sx126x_symbol_to_msec(dev, dev->rx_timeout));
        if (_timeout != 0) {
            sx126x_set_rx(dev, _timeout);
        }
        else {
            sx126x_set_rx(dev, SX126X_RX_SINGLE_MODE);
        }
        break;

    case NETOPT_STATE_TX:
        DEBUG("[sx126x] netdev: set NETOPT_STATE_TX state\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, dev->params->tx_pa_mode);
        }
#endif
        sx126x_set_tx(dev, 0);
        break;

    case NETOPT_STATE_RESET:
        DEBUG("[sx126x] netdev: set NETOPT_STATE_RESET state\n");
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
        if ((sf < LORA_SF6) || (sf > max_sf)) {
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
        assert(len <= sizeof(uint16_t));
        dev->rx_timeout = *(const uint16_t *)val;
        return sizeof(uint16_t);

    case NETOPT_TX_POWER:
        assert(len <= sizeof(int16_t));
        int16_t power = *((const int16_t *)val);
        if ((power < INT8_MIN) || (power > INT8_MAX)) {
            res = -EINVAL;
            break;
        }
        sx126x_set_tx_params(dev, power, SX126X_RAMP_10_US);
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
