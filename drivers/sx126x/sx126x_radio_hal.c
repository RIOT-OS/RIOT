/*
 * Copyright (C) 2023 
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
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "net/gnrc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#include "net/ieee802154/radio.h"

#include "event/thread.h"
#include "ztimer.h"

#include "thread.h"

#include "kernel_defines.h"
#include "event.h"

#include "sx126x.h"
#include "sx126x_params.h"
#include "sx126x_internal.h"

#define LORA_ACK_REPLY_US          1024

#define SX126X_HAL_CHAN_26 (869525000LU)
#define SX126X_HAL_CHAN_BASE (865500000LU)
#define SX126X_HAL_CHAN_SPACING (200000LU)

#define SX126X_CHAN_MIN (11)
#define SX126X_CHAN_MAX (26)

#define SX126X_POWER_MIN (-9)
#define SX126X_POWER_MAX (22)

static const ieee802154_radio_ops_t sx126x_ops;
static ieee802154_dev_t *_sx126x_hal_dev;

static int _set_state(sx126x_t *dev, sx126x_state_t state);
static int _get_state(sx126x_t *dev, void* val);


void sx126x_hal_setup(sx126x_t *dev, ieee802154_dev_t *hal)
{
    hal->driver = &sx126x_ops;
    hal->priv = dev;
}

#if IS_USED(MODULE_SX126X_STM32WL)

void _sx126x_handler(event_t *event)
{
    (void) event;
    ieee802154_dev_t *hal = _sx126x_hal_dev;
    sx126x_hal_task_handler(hal);
}

event_t sx126x_ev = {.handler = _sx126x_handler};

void isr_subghz_radio(void)
{
    /* Disable NVIC to avoid ISR conflict in CPU. */
    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    NVIC_ClearPendingIRQ(SUBGHZ_Radio_IRQn);
    event_post(EVENT_PRIO_HIGHEST, &sx126x_ev);
    cortexm_isr_end();
}
#endif 

static void ack_timer_cb(void *arg)
{
    sx126x_t *dev = arg;
    (void)dev;
    _set_state(dev, STATE_IDLE);
    ztimer_remove(ZTIMER_USEC, &dev->ack_timer);
    uint8_t ack[3] = {
        IEEE802154_FCF_TYPE_ACK,
        0x00, 
        dev->seq_num
    };
    sx126x_set_buffer_base_address(dev, 0x80, 0x00);
    sx126x_write_buffer(dev, 0x80, ack, IEEE802154_ACK_FRAME_LEN-2);
    sx126x_set_lora_payload_length(dev, IEEE802154_ACK_FRAME_LEN-2);
    _set_state(dev, STATE_TX);
    dev->ack_filter = true;
}

void sx126x_setup(ieee802154_dev_t *hal, uint8_t index)
{
    sx126x_t *dev = hal->priv;
    (void)index;

    dev->ack_timer.arg = dev;
    dev->ack_timer.callback = ack_timer_cb;
    dev->ack_filter = false;

     _sx126x_hal_dev = hal;
}

static bool _l2filter(uint8_t *mhr)
{
    ieee802154_dev_t *hal = _sx126x_hal_dev;
    sx126x_t *dev = hal->priv;
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);

    if ((mhr[0] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_BEACON) {
        if ((memcmp(&dev->pan_id, pan_bcast, 2) == 0)) {
            return true;
        }
    }
    /* filter PAN ID */
    /* Will only work on little endian platform (all?) */

    if ((memcmp(pan_bcast, dst_pan.u8, 2) != 0) &&
        (memcmp(&dev->pan_id, dst_pan.u8, 2) != 0)) {
        return false;
    }

    /* check destination address */
    if (((addr_len == IEEE802154_SHORT_ADDRESS_LEN) &&
          (memcmp(dev->short_addr, dst_addr, addr_len) == 0 ||
           memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0)) ||
        ((addr_len == IEEE802154_LONG_ADDRESS_LEN) &&
          (memcmp(dev->long_addr, dst_addr, addr_len) == 0))) {
        return true;
    }

    return false;
}

static int _set_state(sx126x_t *dev, sx126x_state_t state)
{
    switch (state) {
    case STATE_IDLE:
        DEBUG("[sx126x] netdev: set STATE_STANDBY\n");
        sx126x_set_standby(dev, SX126X_CHIP_MODE_STBY_XOSC);
        break;

    case STATE_RX:
        DEBUG("[sx126x] netdev: set STATE_RX\n");
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

    case STATE_TX:
        DEBUG("[sx126x] netdev: set STATE_TX\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, dev->params->tx_pa_mode);
        }
#endif
        sx126x_set_tx(dev, 0);
        break;

    default:
        return -ENOTSUP;
    }
    dev->state = state;
    return sizeof(sx126x_state_t);
}

static int _get_state(sx126x_t *dev, void *val)
{
    sx126x_chip_status_t radio_status;

    sx126x_get_status(dev, &radio_status);
    sx126x_state_t state = STATE_IDLE;

    switch (radio_status.chip_mode) {
    case SX126X_CHIP_MODE_RFU:
    case SX126X_CHIP_MODE_STBY_RC:
    case SX126X_CHIP_MODE_STBY_XOSC:
        state = STATE_IDLE;
        break;

    case SX126X_CHIP_MODE_TX:
        state = STATE_TX;
        break;

    case SX126X_CHIP_MODE_RX:
        state = STATE_RX;
        break;

    default:
        break;
    }
    dev->state = state;
    memcpy(val, &state, sizeof(sx126x_state_t));
    return sizeof(sx126x_state_t);
}

void sx126x_hal_task_handler(ieee802154_dev_t *hal)
{
    sx126x_t *dev = hal->priv;
    assert(dev);
    uint8_t val;
    (void)_get_state(dev, &val);
    sx126x_irq_mask_t irq_mask;

    sx126x_get_and_clear_irq_status(dev, &irq_mask);

    if (sx126x_is_stm32wl(dev)) {

    if (irq_mask & SX126X_IRQ_TX_DONE) {
        if(dev->ack_filter == false){
        DEBUG("[sx126x] netdev: SX126X_IRQ_TX_DONE\n");
        ztimer_remove(ZTIMER_USEC, &dev->ack_timer);
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
        else {
            dev->ack_filter = false;
            DEBUG("[sx126x] TX ACK done.\n");
            ztimer_remove(ZTIMER_USEC, &dev->ack_timer);
            hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        }
    else if (irq_mask & SX126X_IRQ_RX_DONE) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_RX_DONE\n");
    
    uint8_t rxbuf[127];
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    dev->size = rx_buffer_status.pld_len_in_bytes;
 
    sx126x_read_buffer(dev, rx_buffer_status.buffer_start_pointer, (uint8_t*)rxbuf, dev->size);
        bool l2filter_passed = _l2filter(rxbuf);
        bool is_auto_ack_en = !IS_ACTIVE(CONFIG_IEEE802154_AUTO_ACK_DISABLE);
        bool is_ack = (rxbuf[0] & IEEE802154_FCF_TYPE_ACK)&&(rxbuf[1] == 0x00);
        bool ack_req = rxbuf[0] & IEEE802154_FCF_ACK_REQ;
    /* If radio is in promiscuos mode, indicate packet and
            * don't event think of sending an ACK frame :) */
            if (dev->promisc) {
                DEBUG("[sx126x] Promiscuous mode is enabled.\n");
                hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
            }
        
    /* If the L2 filter passes, device if the frame is indicated
            * directly or if the driver should send an ACK frame before
            * the indication */
            else if (l2filter_passed) {
                    if (ack_req && is_auto_ack_en) {
                        dev->seq_num = rxbuf[2];
                       DEBUG("Received valid frame, need ack\n");
                       ztimer_set(ZTIMER_USEC, &dev->ack_timer, LORA_ACK_REPLY_US);
                    }
                    else {
                        DEBUG("[sx126x] RX frame doesn't require ACK frame.\n");
                        hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
                    }
                }
            else if (is_ack && !(dev->ack_filter)) {
                    DEBUG("[sx126x] Received ACK.\n");
                    hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
                }
                /* If all failed, simply drop the frame and continue listening
                 * to incoming frames */
                else {
                    DEBUG("[sx126x] Addr filter failed or ACK filter on.\n");
                    _set_state(dev, STATE_RX);
                   
                }        
           
    }
    else if (irq_mask & SX126X_IRQ_PREAMBLE_DETECTED) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_PREAMBLE_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_SYNC_WORD_VALID) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_SYNC_WORD_VALID\n");
        
    }
    else if (irq_mask & SX126X_IRQ_HEADER_VALID) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_HEADER_VALID\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_START);
    }
    else if (irq_mask & SX126X_IRQ_HEADER_ERROR) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_HEADER_ERROR\n");
    }
    else if (irq_mask & SX126X_IRQ_CRC_ERROR) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_CRC_ERROR\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_CRC_ERROR);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DONE) {
        if (irq_mask & SX126X_IRQ_CAD_DETECTED){
            DEBUG("[sx126x] netdev: SX126X_IRQ_CAD_DETECTED \n");
            dev->cad_detected = true;
        }
        DEBUG("[sx126x] netdev: SX126X_IRQ_CAD_DONE\n");
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_CCA);
        _set_state(dev, STATE_RX);
        
    }
    else if (irq_mask & SX126X_IRQ_TIMEOUT) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_TIMEOUT\n");
    }
    else {
        DEBUG("[sx126x] netdev: SX126X_IRQ_NONE\n");
    }

    #if IS_USED(MODULE_SX126X_STM32WL)
        NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
    #endif  
}
}

                        /*RADIO HAL FUNCTIONS*/

static int _write(ieee802154_dev_t *hal, const iolist_t *iolist){

    sx126x_t *dev = hal->priv;
    (void)dev;
    size_t pos = 0;
    /* Full buffer used for Tx */
    sx126x_set_buffer_base_address(dev, 0x80, 0x00);
    /* Write payload buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len > 0) {
            sx126x_write_buffer(dev, pos + 0x80, iol->iol_base, iol->iol_len);
            DEBUG("[sx126x] netdev: send: wrote data to payload buffer.\n");
            pos += iol->iol_len;
        }
    }

    sx126x_set_lora_payload_length(dev, pos);
    DEBUG("[sx126x] writing (size: %d).\n", (pos));
    return 0;
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx){
    sx126x_t *dev = hal->priv;
    (void)dev;
    (void)ctx;
    switch (op) {
        case IEEE802154_HAL_OP_TRANSMIT:
        dev->ack_filter = false;
        _set_state(dev, STATE_TX);

        break;
        case IEEE802154_HAL_OP_SET_RX:

        _set_state(dev, STATE_RX);

        break;

        case IEEE802154_HAL_OP_SET_IDLE:

        _set_state(dev, STATE_IDLE);

        break;

        case IEEE802154_HAL_OP_CCA:
            DEBUG("[sx126x] netdev: HAL_OP_CCA (CAD Detection state)\n");
            dev->cad_detected = 0;
            _set_state(dev, STATE_IDLE);
            sx126x_set_cad(dev);

        break;

        default:
        assert(false);
        break;

    }
    return 0;
}

static int _confirm_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx){
    sx126x_t *dev = hal->priv;
    (void)op;
    (void)ctx;
    ieee802154_tx_info_t *info = ctx;
    bool eagain = false;
    sx126x_state_t state;
    _get_state(dev, &state);

    
switch (op){
    case IEEE802154_HAL_OP_TRANSMIT:
       if (info) {
            info->status = (dev->cad_detected) ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
        }
    while(state == STATE_TX)
        _get_state(dev, &state);
        
    break;

    case IEEE802154_HAL_OP_SET_RX:
        if(state!=STATE_RX) 
            eagain = true;
    break;

    case IEEE802154_HAL_OP_SET_IDLE:
        eagain = (state != STATE_IDLE);
        break;

    case IEEE802154_HAL_OP_CCA:
            *((bool*) ctx) = !dev->cad_detected;
    break;

    default:
        eagain = false;
        assert(false);
        break;
    
}

    if (eagain) {
        DEBUG("EAGAIN");
        return -EAGAIN;
    }

    return 0;
}

static int _len(ieee802154_dev_t *hal){
    (void)hal;
    sx126x_t *dev = hal->priv;
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    return rx_buffer_status.pld_len_in_bytes;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t max_size, ieee802154_rx_info_t *info)
{
    (void)hal;
    (void)buf;
    (void)info;

    sx126x_t* dev = hal->priv;
        /* Getting information about last received packet */
    
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_pkt_status_lora_t pkt_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    sx126x_get_lora_pkt_status(dev, &pkt_status);
    if (info) {
        info->lqi = pkt_status.snr_pkt_in_db;
        info->rssi = pkt_status.rssi_pkt_in_dbm;
    }

      /* Put PSDU to the output buffer */
    
    if (buf == NULL) {
        return rx_buffer_status.pld_len_in_bytes;
    }

    if (rx_buffer_status.pld_len_in_bytes > max_size) {
        return -ENOBUFS;
    }

    if (rx_buffer_status.pld_len_in_bytes < 3) {
        return -EBADMSG;
    }
    sx126x_read_buffer(dev, rx_buffer_status.buffer_start_pointer, (uint8_t*)buf, rx_buffer_status.pld_len_in_bytes);
    DEBUG("[sx126x] first 3 bytes of received packet: %d %d %d\n", *(uint8_t*)buf, *((uint8_t*)buf+1), *((uint8_t*)buf+2));
    return rx_buffer_status.pld_len_in_bytes;
}

static int _set_cca_threshold(ieee802154_dev_t *hal, int8_t threshold)
{
    (void)hal;
    (void)threshold;

    return 0;
}

static int _config_phy(ieee802154_dev_t *hal, const ieee802154_phy_conf_t *conf){
    (void)hal;
    (void)conf;
    sx126x_t *dev = hal->priv;
    uint8_t channel = conf->channel;
    int8_t pow = conf->pow;
    if((channel < SX126X_CHAN_MIN) && (channel > SX126X_CHAN_MAX))
        return -EINVAL;

    if (channel == 26) {
        sx126x_set_channel(dev, SX126X_HAL_CHAN_26);
    }
    else {
        sx126x_set_channel(dev, (channel-11)*SX126X_HAL_CHAN_SPACING + SX126X_HAL_CHAN_BASE);
    }

    if((pow < SX126X_POWER_MIN) && (pow > SX126X_POWER_MAX))
        return -EINVAL;

    sx126x_set_tx_params(dev, pow, SX126X_RAMP_10_US);
    return 0;
}

static int _off(ieee802154_dev_t *hal)
{
    (void)hal;
    sx126x_t *dev = hal->priv;
    sx126x_set_sleep(dev, SX126X_SLEEP_CFG_COLD_START);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *hal, ieee802154_af_cmd_t cmd, const void *value)
{
    (void)cmd;
    (void)value;
    uint16_t pan_id = *(uint16_t*)value;
    sx126x_t *dev = hal->priv;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            memcpy(dev->short_addr, value, IEEE802154_SHORT_ADDRESS_LEN);
            break;
        case IEEE802154_AF_EXT_ADDR:
            memcpy(dev->long_addr, value, IEEE802154_LONG_ADDRESS_LEN);
            break;
        case IEEE802154_AF_PANID:
            dev->pan_id = pan_id;
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
    _set_state(dev, STATE_IDLE);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *hal)
{
    (void)hal;

    return -ENOTSUP;
}

static int _set_cca_mode(ieee802154_dev_t *hal, ieee802154_cca_mode_t mode)
{
    (void)hal;
    sx126x_t* dev = hal->priv;
    DEBUG("[sx126x] netdev: set_cca_mode \n");
        dev->cad_params.cad_exit_mode = SX126X_CAD_ONLY,
        dev->cad_params.cad_detect_min = 10,
        dev->cad_params.cad_detect_peak = 22,
        dev->cad_params.cad_symb_nb = SX126X_CAD_02_SYMB,
        dev->cad_params.cad_timeout = 0x000F00;
    (void)mode;

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

    bool ackf = true;
    bool _promisc = false;

    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            DEBUG("Filter_accept_all\n");
            break;
        case IEEE802154_FILTER_PROMISC:
            _promisc = true;
            break;
        case IEEE802154_FILTER_ACK_ONLY:
            ackf = false;
            DEBUG("Filter_ack_only\n");
            break;
        default:
            return -ENOTSUP;
    }

    dev->ack_filter = ackf;
    dev->promisc = _promisc;

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *hal, const ieee802154_csma_be_t *bd, int8_t retries)
{
    (void)hal;
    (void)bd;
    (void)retries;

    return -ENOTSUP;
}


static const ieee802154_radio_ops_t sx126x_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_PHY_BPSK,
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
};
