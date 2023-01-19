/*
 * Copyright (C) 2020 Locha Inc
 *               2022 Francisco Acosta
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 Radio HAL implementation for the cc26x2_cc13x2 RF driver
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @author      Francisco Acosta <fco.ja.ac@gmail.com>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "net/ieee802154/radio.h"
#include "periph/timer.h"

#include "cc26x2_cc13x2_rf.h"
#include "cc26x2_cc13x2_rf_internal.h"
#include "cc26x2_cc13x2_rfc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/** The FCS is calculated by the radio */
#define ACK_PSDU_LEN        (IEEE802154_ACK_FRAME_LEN - IEEE802154_FCS_LEN)

#define MAC_TIMER_CHAN_ACK  (0U) /**< MAC timer channel for transmitting an ACK
                                      frame */
#define MAC_TIMER_CHAN_IFS  (1U) /**< MAC timer channel for handling IFS
                                      logic */

/* Set timer period to 20 us (IEEE 802.15.4g-2012 MR-FSK symbol time) */
#define TIMER_FREQ          (50000UL)

static const ieee802154_radio_ops_t cc26x2_cc13x2_rf_ops;
static ieee802154_dev_t *cc26x2_cc13x2_rf_hal;

typedef enum {
    STATE_IDLE,
    STATE_TX,
    STATE_ACK,
    STATE_RX,
    STATE_CCA_CLEAR,
    STATE_CCA_BUSY,
} cc26x2_cc13x2_state_t;

static volatile uint8_t _state;

static struct {
    bool promisc    : 1; /**< whether the device is in promiscuous mode or no */
    bool cca_send   : 1; /**< whether the next transmission uses CCA or not */
    bool ack_filter : 1; /**< whether the ACK filter is activated or not */
    bool ifs        : 1; /**< whether the radio is in IFS mode */
} _cfg = {
    .promisc    = false,
    .cca_send   = true,
    .ack_filter = true,
    .ifs        = false,
};

static uint8_t _ack[ACK_PSDU_LEN];

enum {
    CCA_RESULT_NONE,    /**< No result produced yet */
    CCA_RESULT_IDLE,    /**< Channel is Idle */
    CCA_RESULT_BUSY,    /**< Channel is Busy */
} _cca_result;

enum {
    TX_STATE_IDLE,         /**< We are not transmitting anything */
    TX_STATE_TX,           /**< Normal TX */
    TX_STATE_TX_ACK,       /**< Transmitting ACK */
} _tx_state;

static int _tx_done = 0;    /**< Transmission result, counts the number of
                                 transmissions done */

static void _set_ifs_timer(bool lifs)
{
    uint8_t timeout;
    _cfg.ifs = true;
    if (lifs) {
        timeout = IEEE802154_LIFS_SYMS;
    }
    else {
        timeout = IEEE802154_SIFS_SYMS;
    }

    timer_set(CC26X2_CC13X2_RF_TIMER, MAC_TIMER_CHAN_IFS, timeout);
    timer_start(CC26X2_CC13X2_RF_TIMER);
}

void cc26x2_cc13x2_rfc_isr(void)
{
    uint8_t *psdu;
    rfc_data_entry_general_t *entry;

    ieee802154_dev_t *dev = cc26x2_cc13x2_rf_hal;

    /* Check if CPE_IRQ_RX_ENTRY_DONE is enabled and the flag is present */
    if ((RFC_DBELL->RFCPEIFG & CPE_IRQ_RX_ENTRY_DONE) &&
        (RFC_DBELL->RFCPEIEN & CPE_IRQ_RX_ENTRY_DONE)) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_RX_ENTRY_DONE;

        entry = cc26x2_cc13x2_rf_recv();
        if (entry == NULL) {
            return;
        }

        /* we don't want an empty payload or a big one we can't handle :-) */
        uint16_t pktlen = *(uint16_t *)&entry->data;
        if (pktlen < CC26X2_CC13X2_RF_RX_STATUS_LEN ||
            pktlen > IEEE802154G_FRAME_LEN_MAX + CC26X2_CC13X2_RF_RX_STATUS_LEN) {
            entry->status = RFC_DATA_ENTRY_PENDING;
            return;
        }
        psdu = (&entry->data) + 2;

        /* pass the PSDU (the MHR) to filter our packets */
        bool l2filter_passed = cc26x2_cc13x2_rf_l2_filter(psdu);
        bool is_auto_ack_en = _ack[0];
        bool is_ack = psdu[0] & IEEE802154_FCF_TYPE_ACK;
        bool is_ack_req = psdu[0] & IEEE802154_FCF_ACK_REQ;

        if (_cfg.promisc) {
            dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        /* If the L2 filter passes, check if the frame should be indicated
         * directly or if the driver should send an ACK frame before the
         * indication */
        else if (l2filter_passed) {
            if (is_ack_req && is_auto_ack_en) {
                /* copy the sequence number to the ACK frame we'll send */
                _ack[2] = psdu[2];

                /* to send the ACK we need to stop receiving */
                if (cc26x2_cc13x2_rf_rx_is_on()) {
                    cc26x2_cc13x2_rf_rx_stop();
                }

                /* wait a short IFS in order to send the ACK */
                timer_set(CC26X2_CC13X2_RF_TIMER, MAC_TIMER_CHAN_ACK, IEEE802154_SIFS_SYMS);
                timer_start(CC26X2_CC13X2_RF_TIMER);
            } else {
                /* frame doesn't want an ACK or we don't want to auto ACK this
                 * frame, either way just pass it on */
                dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
            }
        }
        /* In case the packet is an ACK and the ACK filter is disabled,
         * indicate the frame reception */
        else if (is_ack && !_cfg.ack_filter) {
            dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        /* If all failed, simply drop the frame and continue listening to
         * incoming frames */
        else {
            entry->status = RFC_DATA_ENTRY_PENDING;
        }

    }

    if ((RFC_DBELL->RFCPEIFG & CPE_IRQ_LAST_COMMAND_DONE) &&
        (RFC_DBELL->RFCPEIEN & CPE_IRQ_LAST_COMMAND_DONE)) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
        RFC_DBELL->RFCPEIEN &= ~CPE_IRQ_LAST_COMMAND_DONE;

        if (rf_cmd_prop_tx_adv.status == RFC_PROP_DONE_OK) {
            rf_cmd_prop_tx_adv.status = RFC_IDLE;

            switch (_tx_state) {
            case STATE_TX:
                _tx_state = TX_STATE_IDLE;
                _set_ifs_timer(cc26x2_cc13x2_rf_tx_psdu_len() > IEEE802154_SIFS_MAX_FRAME_SIZE);
                _tx_done++;
                dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
                break;
            case STATE_ACK:
                _tx_state = TX_STATE_IDLE;
                /* restore RX */
                cc26x2_cc13x2_rf_rx_start();

                _set_ifs_timer(false);
                dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                break;
            default:
                DEBUG("cc26x2_cc13x2_rf]: undefined TX state\n");
                break;
            }
        }

        if (rf_cmd_prop_cs.status == RFC_PROP_DONE_IDLE ||
            rf_cmd_prop_cs.status == RFC_PROP_DONE_BUSY) {
            _cca_result =
                rf_cmd_prop_cs.status == RFC_PROP_DONE_IDLE ? CCA_RESULT_IDLE :
                                                              CCA_RESULT_BUSY;
            rf_cmd_prop_cs.status = RFC_IDLE;

            dev->cb(dev, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    (void)dev;

    return cc26x2_cc13x2_rf_write(iolist);
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t max_size,
                 ieee802154_rx_info_t *info)
{
    (void)dev;
    rfc_data_entry_general_t *entry;
    uint8_t *psdu;
    uint16_t pktlen;

    /* shouldn't be null as we informed about a packet */
    entry = cc26x2_cc13x2_rf_recv();
    if (entry == NULL) {
        return -ENOENT;
    }

    /* first two bytes are PSDU length */
    pktlen = *(uint16_t *)&entry->data;
    if (pktlen < CC26X2_CC13X2_RF_RX_STATUS_LEN ||
        pktlen > IEEE802154G_FRAME_LEN_MAX + CC26X2_CC13X2_RF_RX_STATUS_LEN) {
        entry->status = RFC_DATA_ENTRY_PENDING;
        return -ENOENT;
    }
    pktlen -= CC26X2_CC13X2_RF_RX_STATUS_LEN;
    DEBUG("[cc26x2_cc13x2_rf]: reading packet of length %i\n", pktlen);

    psdu = (&entry->data) + sizeof(uint16_t);

    if (max_size < pktlen) {
        DEBUG("[cc26x2_cc13x2_rf]: buffer is too small\n");
        entry->status = RFC_DATA_ENTRY_PENDING;
        return -ENOBUFS;
    }
    else {
        if (info != NULL) {
            /* RSSI resides at the end of the PSDU */
            DEBUG("RSSI: %i\n", (int)(int8_t)psdu[pktlen]);
            info->rssi = ieee802154_dbm_to_rssi((int8_t)psdu[pktlen]);
            info->lqi = 0;
        }
        memcpy(buf, psdu, pktlen);
        entry->status = RFC_DATA_ENTRY_PENDING;
    }

    return pktlen;
}

static int _len(ieee802154_dev_t *dev)
{
    (void)dev;

    /* shouldn't be null as we informed about a packet */
    rfc_data_entry_general_t *entry = cc26x2_cc13x2_rf_recv();
    assert(entry != NULL);

    uint16_t pktlen = *(uint16_t *)&entry->data;
    if (pktlen < CC26X2_CC13X2_RF_RX_STATUS_LEN) {
        entry->status = RFC_DATA_ENTRY_PENDING;
        return -ENOENT;
    }

    return pktlen - CC26X2_CC13X2_RF_RX_STATUS_LEN;
}

static int _off(ieee802154_dev_t *dev)
{
    (void)dev;
    return -ENOTSUP;
}

static int _request_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    (void)dev;

    int res = -EBUSY;
    int state = STATE_IDLE;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        if (_cfg.ifs) {
            DEBUG("[cc26x2_cc13x2_rf]: waiting IFS period\n");
            goto end;
        }
        state = STATE_TX;
        _tx_state = TX_STATE_TX;

        if (cc26x2_cc13x2_rf_rx_is_on()) {
            cc26x2_cc13x2_rf_rx_stop();
        }

        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
        RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_LAST_COMMAND_DONE;

        if ((res = cc26x2_cc13x2_rf_request_transmit()) != 0) {
            DEBUG("[cc26x2_cc13x2_rf]: failed to transmit\n");
            RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
        }
        break;
    case IEEE802154_HAL_OP_SET_RX:
        if (_state != STATE_IDLE && _state != STATE_RX) {
            goto end;
        }
        state = STATE_RX;
        _tx_state = TX_STATE_IDLE;
        res = cc26x2_cc13x2_rf_rx_start();
        break;
    case IEEE802154_HAL_OP_SET_IDLE: {
        assert(ctx);
        bool force = *((bool*) ctx);
        if (!force && _state != STATE_IDLE && _state != STATE_RX) {
            goto end;
        }
        if (cc26x2_cc13x2_rf_rx_is_on()) {
            cc26x2_cc13x2_rf_rx_stop();
        }
        state = STATE_IDLE;
        break;
    }
    case IEEE802154_HAL_OP_CCA:
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
        RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_LAST_COMMAND_DONE;

        if ((res = cc26x2_cc13x2_rf_request_cca()) != 0) {
            RFC_DBELL_NONBUF->RFCPEIEN &= ~CPE_IRQ_LAST_COMMAND_DONE;
        }
        state = STATE_IDLE;
        break;
    default:
        assert(false);
        state = 0;
        break;
    }

    _state = state;
    res = 0;

end:
    return res;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    (void) dev;
    bool eagain;
    ieee802154_tx_info_t *info = ctx;
    int state = _state;
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        info = ctx;
        /* verify if we have any pending confirmation */
        if (_tx_done == 0) {
            return -EAGAIN;
        } else {
            eagain = (state != STATE_IDLE && state != STATE_CCA_BUSY) ? false : true;
        }
        _tx_done--;
        state = STATE_IDLE;
        if (info) {
            info->status = (_state == STATE_CCA_BUSY) ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
            info->retrans = 0;
        }
        break;
    case IEEE802154_HAL_OP_SET_RX:
        eagain = (state == STATE_RX) ? false : true;
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        eagain = (state == STATE_IDLE && _tx_state == TX_STATE_IDLE) ? false : true;
        break;
    case IEEE802154_HAL_OP_CCA:
        eagain = (state != STATE_CCA_BUSY && state != STATE_CCA_CLEAR);
        assert(ctx);
        *((bool*) ctx) = (state == STATE_CCA_CLEAR) ? true : false;
        switch (_cca_result) {
        case CCA_RESULT_IDLE:
            eagain = true;
            break;
        case CCA_RESULT_BUSY:
            eagain = false;
            break;
        case CCA_RESULT_NONE:
        default:
            eagain = -EAGAIN;
    }
        state = STATE_IDLE;
        break;
    default:
        eagain = false;
        assert(false);
        break;
    }

    if (eagain) {
        return -EAGAIN;
    }

    _state = state;

    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    (void)dev;
    _tx_state = TX_STATE_IDLE;
    cc26x2_cc13x2_rf_request_on();
    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void)dev;
    if (cc26x2_cc13x2_rf_confirm_on() != 0) {
        return -EAGAIN;
    }

    /* turn on the RX IRQs */
    RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_RX_ENTRY_DONE;

    return 0;
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void)dev;
    cc26x2_cc13x2_rf_set_cca_threshold(threshold);
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void)dev;

    /* Carrier-sensing only for now,
     *
     * TODO: energy-detecion mode maybe could be done by obtaining the current
     * readed RSSI by the RF Core and compare it with the threshold. Other modes
     * could be implemented with these two */
    if (mode == IEEE802154_CCA_MODE_CARRIER_SENSING) {
        return 0;
    }

    return -ENOTSUP;
}

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    (void)dev;
    int ret;

    if ((ret = cc26x2_cc13x2_rf_change_chan(conf->channel)) != 0) {
        DEBUG("[cc26x2_cc13x2_rf]: couldn't change channel to %d\n",
              conf->channel);
        return ret;
    }

    if ((ret = cc26x2_cc13x2_rf_set_tx_pwr(conf->pow)) != 0) {
        DEBUG("[cc26x2_cc13x2_rf]: couldn't set transmit power to %d dBm",
              conf->pow);
        return ret;
    }

    DEBUG("[cc26x2_cc13x2_rf]: channel %d, TX pwr: %d dBm\n", conf->channel, conf->pow);

    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    (void) dev;
    const network_uint16_t *short_addr = value;
    const eui64_t *ext_addr = value;
    const uint16_t *pan_id = value;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            cc26x2_cc13x2_rf_set_short_addr(*short_addr);
            break;
        case IEEE802154_AF_EXT_ADDR:
            cc26x2_cc13x2_rf_set_long_addr(*ext_addr);
            break;
        case IEEE802154_AF_PANID:
            cc26x2_cc13x2_rf_set_pan_id(*pan_id);
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev,
                            const ieee802154_csma_be_t *bd,
                            int8_t retries)
{
    (void)dev;
    (void)bd;

    /* TODO: adapt and do a hack with CMD_PROP_CS to perform Auto CSMA,
     * by using CMD_PROP_TX_ADV as the next operation. */
    /* only direct mode for now */
    if (retries == -1) {
        return 0;
    }

    return -EINVAL;
}

static int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd, const void *value)
{
    (void)dev;

    switch(cmd) {
        case IEEE802154_SRC_MATCH_EN:
            if (*((const bool*) value) == true) {
                _ack[0] = IEEE802154_FCF_TYPE_ACK | IEEE802154_FCF_FRAME_PEND;
            }
            break;
        default:
            return -ENOTSUP;
    }
    return 0;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    (void)dev;

    bool ackf = true;

    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            _ack[0] = 0;
            break;
        case IEEE802154_FILTER_PROMISC:
            _cfg.promisc = true;
            break;
        case IEEE802154_FILTER_ACK_ONLY:
            ackf = false;
            break;
        default:
            return -ENOTSUP;
    }

    if (!ackf) {
        _cfg.ack_filter = ackf;
    }

    return 0;

}

void cc26x2_cc13x2_rf_hal_setup(ieee802154_dev_t *hal)
{
    /* We don't set hal->priv because the context of this device is global */
    /* We need to store a reference to the HAL descriptor though for the ISR */
    hal->driver = &cc26x2_cc13x2_rf_ops;
    cc26x2_cc13x2_rf_hal = hal;
}

static const ieee802154_radio_ops_t cc26x2_cc13x2_rf_ops = {
    .caps = IEEE802154_CAP_SUB_GHZ
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_PHY_MR_FSK,

    .write = _write,
    .read = _read,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_csma_params = _set_csma_params,
    .set_frame_filter_mode = _set_frame_filter_mode,
};

static void _timer_cb(void *arg, int chan)
{
    (void)arg;

    if (chan == MAC_TIMER_CHAN_ACK) {
        iolist_t iolist = {
            .iol_next = NULL,
            .iol_base = _ack,
            .iol_len = ACK_PSDU_LEN,
        };

        /* send the ACK frame */
        _tx_state = TX_STATE_TX_ACK;
        cc26x2_cc13x2_rf_write(&iolist);

        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
        RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_LAST_COMMAND_DONE;

        cc26x2_cc13x2_rf_request_transmit();
    }
    else if (chan == MAC_TIMER_CHAN_IFS) {
        _cfg.ifs = false;
    }

    timer_stop(CC26X2_CC13X2_RF_TIMER);
}

void cc26x2_cc13x2_rf_init(void)
{
    _ack[0] = IEEE802154_FCF_TYPE_ACK; /* FCF */
    _ack[1] = 0; /* FCF */

    cc26x2_cc13x2_rf_internal_init();

    int result = timer_init(CC26X2_CC13X2_RF_TIMER, TIMER_FREQ, _timer_cb, NULL);
    assert(result >= 0);
    (void)result;
    timer_stop(CC26X2_CC13X2_RF_TIMER);
}

void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_t *dev)
{
    (void)dev;

    cc26x2_cc13x2_rf_init();
}
