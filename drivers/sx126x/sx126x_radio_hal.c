/*
 * Copyright (C) 2023-2025
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx126x
 * @{
 * @file
 * @brief       Radio HAL implementation for the Semtech SX126x
 *
 * @author      Klim Evdokimov <klimevdokimov@mail.ru>
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "macros/utils.h"
#include "modules.h"
#if IS_USED(MODULE_SX126X_IEEE802154)
#include "net/ieee802154/radio.h"
#include "net/ieee802154/submac.h"
#endif
#include "od.h"

#include "sx126x.h"
#include "sx126x_params.h"
#include "sx126x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SX126X_BUFFER_SIZE              (256)
#define SX126X_RX_BUFFER_OFFSET         (0)
#define SX126X_TX_BUFFER_OFFSET         ((SX126X_BUFFER_SIZE) / 2)

#if IS_USED(MODULE_SX126X_IEEE802154)

/**
 * @brief   Internal sx126x device states
 */
typedef enum {
    SX126X_STATE_STANDBY,
    SX126X_STATE_TX,
    SX126X_STATE_RX,
    SX126X_STATE_CAD,
} sx126x_state_t;

static int _set_state(sx126x_t *dev, sx126x_state_t state);
static int _get_state(sx126x_t *dev, void  *val);

static bool _l2filter(ieee802154_dev_t *hal, uint8_t *mhr)
{
    sx126x_t *dev = hal->priv;
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    le_uint16_t pan_bcast = { .u8 = IEEE802154_PANID_BCAST };

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);

    if ((mhr[0] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_BEACON) {
        if (dev->pan_id ==  pan_bcast.u16) {
            SX126X_DEBUG(dev, "hal: beacon\n");
            return true;
        }
    }

    /* filter PAN ID */
    /* Will only work on little endian platform (all?) */

    if (pan_bcast.u16 != byteorder_ltohs(dst_pan) &&
        dev->pan_id != byteorder_ltohs(dst_pan)) {
        SX126X_DEBUG(dev, "hal: PAN ID mismatch\n");
        return false;
    }

    /* check destination address */
    if (addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        if (memcmp(dev->short_addr, dst_addr, addr_len) == 0 ||
            memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0) {
            return true;
        }
        else {
            SX126X_DEBUG(dev, "hal: short address mismatch 0x%02x:0x%02x\n",
                         dst_addr[0], dst_addr[1]);
            return false;
        }
    }
    else if (addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        if (memcmp(dev->long_addr, dst_addr, addr_len) == 0) {
            return true;
        }
        else {
            SX126X_DEBUG(dev, "hal: long address mismatch 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n",
                         dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3], dst_addr[4], dst_addr[5], dst_addr[6], dst_addr[7]);
            return false;
        }
    }

    return false;
}

static int _set_state(sx126x_t *dev, sx126x_state_t state)
{
    switch (state) {
    case SX126X_STATE_STANDBY:
        SX126X_DEBUG(dev, "hal: set STATE_STANDBY\n");
        sx126x_set_state(dev, SX126X_CHIP_MODE_STBY_XOSC);
        break;

    case SX126X_STATE_RX:
        SX126X_DEBUG(dev, "hal: set STATE_RX\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        /* Refer Section 4.2 RF Switch in Application Note (AN5406) */
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, SX126X_RF_MODE_RX);
        }
#endif
        sx126x_set_state(dev, SX126X_CHIP_MODE_RX);
        break;

    case SX126X_STATE_TX:
        SX126X_DEBUG(dev, "hal: set STATE_TX\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, dev->params->tx_pa_mode);
        }
#endif
        sx126x_set_state(dev, SX126X_CHIP_MODE_TX);
        break;

    case SX126X_STATE_CAD:
        SX126X_DEBUG(dev, "hal: set STATE_CCA\n");
        dev->cad_done = false;
        sx126x_set_cad(dev);
        break;

    default:
        return -ENOTSUP;
    }
    return sizeof(sx126x_state_t);
}

static int _get_state(sx126x_t *dev, void *val)
{
    sx126x_state_t state;
    sx126x_chip_modes_t mode = sx126x_get_state(dev);
    SX126X_DEBUG(dev, "hal: state %d\n", mode);
    switch (mode) {
    case SX126X_CHIP_MODE_TX:
        state = SX126X_STATE_TX;
        break;

    case SX126X_CHIP_MODE_RX:
        state = SX126X_STATE_RX;
        break;

    default:
        state = SX126X_STATE_STANDBY;
        break;
    }
    memcpy(val, &state, sizeof(sx126x_state_t));
    return sizeof(sx126x_state_t);
}

void sx126x_hal_event_handler(ieee802154_dev_t *hal)
{
    sx126x_t *dev = hal->priv;
    sx126x_irq_mask_t irq_mask;

    sx126x_get_and_clear_irq_status(dev, &irq_mask);
    SX126X_DEBUG(dev, "sx126x_hal_event_handler(): 0x%"PRIx16"\n", irq_mask);
    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
#endif
    }
    if (irq_mask & SX126X_IRQ_TX_DONE) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_TX_DONE\n");
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }
    else if (irq_mask & SX126X_IRQ_RX_DONE) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_RX_DONE\n");
        uint8_t mhdr[IEEE802154_MAX_HDR_LEN];
        sx126x_rx_buffer_status_t rx_buffer_status;
        sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
        size_t rx_size = MIN(sizeof(mhdr), rx_buffer_status.pld_len_in_bytes);
        sx126x_read_buffer(dev, rx_buffer_status.buffer_start_pointer, mhdr, rx_size);
        bool is_ack = (mhdr[0] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_ACK;
        bool is_data = (mhdr[0] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_DATA;
        bool ackf = dev->ack_filter;
        bool match = false;

        /* If the L2 filter passes, device if the frame is indicated
         * directly or if the driver should send an ACK frame before the indication */
        if (is_ack || (!ackf && (match = _l2filter(hal, mhdr)))) {
            if (is_data && (mhdr[0] & IEEE802154_FCF_ACK_REQ)) {
                SX126X_DEBUG(dev, "hal: Received valid frame, need to send ack\n");
            }
            SX126X_DEBUG(dev, "hal: RX (%s) frame\n", is_ack ? "ACK" : (is_data ? "DATA" : "UNKNOWN"));
            hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        /* If radio is in promiscuos mode, indicate packet and
         * don't event think of sending an ACK frame :) */
        else if (dev->promisc) {
            SX126X_DEBUG(dev, "hal: Promiscuous mode is enabled.\n");
            hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        /* If all failed, simply drop the frame and continue listening
         * to incoming frames */
        else {
            SX126X_DEBUG(dev, "hal: ACK filter %s.\n", ackf ? "enabled" : "disabled");
            SX126X_DEBUG(dev, "hal: L2 filter %s.\n", match ? "passed" : "failed");
            _set_state(dev, SX126X_STATE_RX);
        }
    }
    else if (irq_mask & SX126X_IRQ_PREAMBLE_DETECTED) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_PREAMBLE_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_SYNC_WORD_VALID) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_SYNC_WORD_VALID\n");
    }
    else if (irq_mask & SX126X_IRQ_HEADER_VALID) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_HEADER_VALID\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_START);
    }
    else if (irq_mask & SX126X_IRQ_HEADER_ERROR) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_HEADER_ERROR\n");
    }
    else if (irq_mask & SX126X_IRQ_CRC_ERROR) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_CRC_ERROR\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_CRC_ERROR);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DONE) {
        if (irq_mask & SX126X_IRQ_CAD_DETECTED){
            SX126X_DEBUG(dev, "hal: SX126X_IRQ_CAD_DETECTED \n");
            dev->cad_detected = true;
        }
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_CAD_DONE\n");
        dev->cad_done = true;
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_CCA);
        _set_state(dev, SX126X_STATE_RX);
    }
    else if (irq_mask & SX126X_IRQ_TIMEOUT) {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_TIMEOUT\n");
    }
    else {
        SX126X_DEBUG(dev, "hal: SX126X_IRQ_NONE\n");
    }
}

static int _write(ieee802154_dev_t *hal, const iolist_t *iolist)
{
    sx126x_t *dev = hal->priv;
    size_t pos = 0;
    /* Full buffer used for Tx */
    sx126x_set_buffer_base_address(dev, SX126X_TX_BUFFER_OFFSET, SX126X_RX_BUFFER_OFFSET);
    /* Write payload buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (pos + iol->iol_len > SX126X_BUFFER_SIZE - SX126X_TX_BUFFER_OFFSET - IEEE802154_FCS_LEN) {
            SX126X_DEBUG(dev, "hal: no buffer space to write payload\n");
            return -E2BIG;
        }
        if (iol->iol_len > 0) {
            sx126x_write_buffer(dev, SX126X_TX_BUFFER_OFFSET + pos, iol->iol_base, iol->iol_len);
            SX126X_DEBUG(dev, "hal: wrote data to payload buffer\n");
            pos += iol->iol_len;
#if IS_ACTIVE(ENABLE_DEBUG)
            od_hex_dump(iol->iol_base, iol->iol_len, OD_WIDTH_DEFAULT);
#endif
        }
    }
    /* Do not compute IEEE 802.15.4 CRC because LoRa frame also provides CRC */
    uint16_t chksum = 0;
    sx126x_write_buffer(dev, SX126X_TX_BUFFER_OFFSET + pos, (uint8_t *)&chksum, sizeof(chksum));
    pos += sizeof(chksum);
    sx126x_set_lora_payload_length(dev, pos);
    SX126X_DEBUG(dev, "hal: writing (size: %d)\n", (pos));
    return 0;
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    sx126x_t *dev = hal->priv;
    (void)ctx;
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        dev->pending = false;
        ieee802154_radio_cca(hal);
        _set_state(dev, SX126X_STATE_TX);
        break;

    case IEEE802154_HAL_OP_SET_RX:
        _set_state(dev, SX126X_STATE_RX);
        break;

    case IEEE802154_HAL_OP_SET_IDLE:
        _set_state(dev, SX126X_STATE_STANDBY);
        break;

    case IEEE802154_HAL_OP_CCA:
        SX126X_DEBUG(dev, "hal: HAL_OP_CCA (CAD Detection state)\n");
        dev->cad_detected = false;
        _set_state(dev, SX126X_STATE_STANDBY);
        sx126x_set_cad(dev);
        break;
    }
    return 0;
}

static int _confirm_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    sx126x_t *dev = hal->priv;
    ieee802154_tx_info_t *info = ctx;
    bool eagain = false;
    sx126x_state_t state;
    _get_state(dev, &state);

    switch (op){
    case IEEE802154_HAL_OP_TRANSMIT:
        if (state == SX126X_STATE_TX) {
            eagain = true;
        }
        else if (info) {
            info->status = (dev->cad_detected) ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
        }
        break;

    case IEEE802154_HAL_OP_SET_RX:
        eagain = (state != SX126X_STATE_RX);
        break;

    case IEEE802154_HAL_OP_SET_IDLE:
        eagain = (state != SX126X_STATE_STANDBY);
        break;

    case IEEE802154_HAL_OP_CCA:
        *((bool *)ctx) = !dev->cad_detected;
        break;
    }
    return eagain ? -EAGAIN : 0;
}

static int _len(ieee802154_dev_t *hal)
{
    sx126x_t *dev = hal->priv;
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    return rx_buffer_status.pld_len_in_bytes - IEEE802154_FCS_LEN;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t max_size, ieee802154_rx_info_t *info)
{
    sx126x_t* dev = hal->priv;
    /* Getting information about last received packet */
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_pkt_status_lora_t pkt_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    sx126x_get_lora_pkt_status(dev, &pkt_status);

    if (info) {
        /* scale int8_t LoRa SNR into uint8_t IEEE 802.15.4 LQI */
        SX126X_DEBUG(dev, "hal: SNR %"PRId8"db\n", pkt_status.snr_pkt_in_db);
        info->lqi = (-INT8_MIN) + pkt_status.snr_pkt_in_db;
        /* scale into IEEE 802.15.4 RSSI [-174,80] dbm */
        SX126X_DEBUG(dev, "hal: RSSI %"PRId8"dbm\n", pkt_status.rssi_pkt_in_dbm);
        info->rssi = ieee802154_dbm_to_rssi(pkt_status.rssi_pkt_in_dbm);
    }
    /* Put PSDU to the output buffer */
    if (buf == NULL) {
        return rx_buffer_status.pld_len_in_bytes - IEEE802154_FCS_LEN;
    }
    if (rx_buffer_status.pld_len_in_bytes > max_size + IEEE802154_FCS_LEN) {
        return -ENOBUFS;
    }
    if (rx_buffer_status.pld_len_in_bytes < IEEE802154_ACK_FRAME_LEN) {
        return -EBADMSG;
    }
    sx126x_read_buffer(dev, rx_buffer_status.buffer_start_pointer,
                       buf, rx_buffer_status.pld_len_in_bytes - IEEE802154_FCS_LEN);
    /* Do not compute IEEE 802.15.4 CRC because LoRa frame also provides CRC */
    return rx_buffer_status.pld_len_in_bytes - IEEE802154_FCS_LEN;
}

static int _set_cca_threshold(ieee802154_dev_t *hal, int8_t threshold)
{
    (void)hal;
    (void)threshold;
    return 0;
}

static int _config_phy(ieee802154_dev_t *hal, ieee802154_phy_conf_t *conf)
{
    sx126x_t *dev = hal->priv;
    /* Ignore channel number and page because it does not apply for LoRa */
    sx126x_set_tx_power(dev, conf->pow, CONFIG_SX126X_RAMP_TIME_DEFAULT);
    conf->res.ack_timeout_us =
        sx126x_symbol_time_on_air_us(dev) * (IEEE802154_ATURNAROUNDTIME_IN_SYMBOLS +
                                             IEEE802154_AUNITBACKOFF_PERIOD_IN_SYMBOLS) +
        sx126x_time_on_air_us(dev, IEEE802154_ACK_FRAME_LEN);
    conf->res.sifs_period_us = 12 * sx126x_symbol_time_on_air_us(dev);
    assert(conf->res.ack_timeout_us);
    assert(conf->res.sifs_period_us);
    return 0;
}

static int _off(ieee802154_dev_t *hal)
{
    (void)hal;
    sx126x_t *dev = hal->priv;
    sx126x_set_sleep(dev, SX126X_SLEEP_CFG_WARM_START);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *hal, ieee802154_af_cmd_t cmd, const void *value)
{
    (void)cmd;
    (void)value;
    sx126x_t *dev = hal->priv;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            memcpy(dev->short_addr, value, IEEE802154_SHORT_ADDRESS_LEN);
            break;
        case IEEE802154_AF_EXT_ADDR:
            memcpy(dev->long_addr, value, IEEE802154_LONG_ADDRESS_LEN);
            break;
        case IEEE802154_AF_PANID:
            dev->pan_id = *(uint16_t *)value;
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }
    return 0;
}

static int _request_on(ieee802154_dev_t *hal)
{
    (void)hal;
    sx126x_t *dev = hal->priv;
    _set_state(dev, SX126X_STATE_STANDBY);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *hal)
{
    (void)hal;
    return -ENOTSUP;
}

static int _set_cca_mode(ieee802154_dev_t *hal, ieee802154_cca_mode_t mode)
{
    (void)mode;
    sx126x_t* dev = hal->priv;
    SX126X_DEBUG(dev, "hal: set_cca_mode \n");
    sx126x_cad_params_t cad_params = {
        .cad_exit_mode = SX126X_CAD_ONLY,
        .cad_detect_min = 10,
        .cad_detect_peak = 22,
        .cad_symb_nb = SX126X_CAD_02_SYMB,
        /* Rx timeout = cad_timeout * 15.625us */
        /* Rx timeout = 60ms */
        .cad_timeout = 0x000F00,
    };
    sx126x_set_cad_params(dev, &cad_params);
    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *hal, ieee802154_src_match_t cmd, const void *value)
{
    (void)hal;
    (void)cmd;
    (void)value;
    return -ENOTSUP;
}

static int _set_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t mode)
{
    sx126x_t* dev = hal->priv;

    bool ackf = false;
    bool promisc = false;

    switch (mode) {
    case IEEE802154_FILTER_ACCEPT:
        SX126X_DEBUG(dev, "hal: Filter accept all\n");
        break;
    case IEEE802154_FILTER_PROMISC:
        promisc = true;
        break;
    case IEEE802154_FILTER_ACK_ONLY:
        ackf = true;
        SX126X_DEBUG(dev, "hal: Filter ACK only\n");
        break;
    default:
        return -ENOTSUP;
    }

    dev->ack_filter = ackf;
    dev->promisc = promisc;

    return 0;
}

static int _get_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t *mode)
{
    sx126x_t* dev = hal->priv;

    if (dev->ack_filter) {
        *mode = IEEE802154_FILTER_ACK_ONLY;
    }
    else if (dev->promisc) {
        *mode = IEEE802154_FILTER_PROMISC;
    }
    else {
        *mode = IEEE802154_FILTER_ACCEPT;
    }
    SX126X_DEBUG(dev, "hal: get_frame_filter_mode %d\n", *mode);
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *hal, const ieee802154_csma_be_t *bd, int8_t retries)
{
    (void)hal;
    (void)bd;
    (void)retries;

    return -ENOTSUP;
}

static const ieee802154_radio_ops_t _sx126x_ops = {
    .caps =  IEEE802154_CAP_SUB_GHZ
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_PHY_LORA,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
    .get_frame_filter_mode = _get_frame_filter_mode,
};

#endif /* IS_USED(MODULE_SX126X_IEEE802154) */

extern const netdev_driver_t sx126x_driver;

void sx126x_hal_setup(sx126x_t *dev, const sx126x_params_t *params, uint8_t index,
                      void *hal, void (*event_cb)(void *arg), void *arg)
{
    (void)dev;
    (void)params;
    (void)hal;
    (void)index;
    (void)event_cb;
    (void)arg;
    /* legacy */
    memset((uint8_t *)dev + sizeof(dev->netdev), 0, sizeof(*dev) - sizeof(dev->netdev));
    netdev_t *netdev = &dev->netdev;
    netdev->driver = &sx126x_driver;
#if IS_USED(MODULE_SX126X_IEEE802154)
    dev->params = (sx126x_params_t *)params;
    dev->ack_filter = false;
    dev->pending = false;
    dev->event_cb = event_cb;
    dev->event_arg = arg;
    ((ieee802154_dev_t *)hal)->driver = &_sx126x_ops;
    ((ieee802154_dev_t *)hal)->priv = dev;
#if IS_USED(MODULE_SX126X_STM32WL)
#if SX126X_NUMOF == 1
    extern sx126x_t *sx126x_stm32wl = dev;
#endif
#endif
#endif
}
