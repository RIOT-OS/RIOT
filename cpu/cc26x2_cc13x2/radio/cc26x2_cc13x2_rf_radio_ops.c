/*
 * Copyright (C) 2020 Locha Inc
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
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "bit.h"

#include "net/ieee802154.h"
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

ieee802154_dev_t cc26x2_cc13x2_rf_hal_dev = {
    .driver = &cc26x2_cc13x2_rf_ops,
};

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
    STATE_IDLE,         /**< We are not transmitting anything */
    STATE_TX,           /**< Normal TX */
    STATE_TX_ACK,       /**< Transmitting ACK */
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

    ieee802154_dev_t *dev = &cc26x2_cc13x2_rf_hal_dev;

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
                _tx_state = STATE_IDLE;
                _set_ifs_timer(cc26x2_cc13x2_rf_tx_psdu_len() > IEEE802154_SIFS_MAX_FRAME_SIZE);
                _tx_done++;
                dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
                break;
            case STATE_TX_ACK:
                _tx_state = STATE_IDLE;
                /* restore RX */
                cc26x2_cc13x2_rf_rx_start();

                _set_ifs_timer(false);
                dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                break;
            default:
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
            info->rssi = psdu[pktlen];
            info->lqi = 0;
        }
        memcpy(buf, psdu, pktlen);
        entry->status = RFC_DATA_ENTRY_PENDING;
    }

    return pktlen;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    int ret;
    (void)dev;

    if (_cfg.ifs) {
        DEBUG("[cc26x2_cc13x2_rf]: waiting IFS period\n");
        return -EBUSY;
    }

    _tx_state = STATE_TX;

    RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
    RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_LAST_COMMAND_DONE;

    if ((ret = cc26x2_cc13x2_rf_request_transmit()) != 0) {
        DEBUG("[cc26x2_cc13x2_rf]: failed to transmit\n");
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
    }

    return ret;
}

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    (void)dev;

    /* verify if we have any pending confirmation */
    if (_tx_done == 0) {
        return -EAGAIN;
    }
    _tx_done--;

    if (info) {
        info->status = TX_STATUS_SUCCESS;
        info->retrans = 0;
    }

    return 0;
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

static int _request_on(ieee802154_dev_t *dev)
{
    (void)dev;
    _tx_state = STATE_IDLE;
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

static int _request_set_trx_state(ieee802154_dev_t *dev,
                                  ieee802154_trx_state_t state)
{
    (void)dev;

    switch (state) {
        case IEEE802154_TRX_STATE_RX_ON:
            _tx_state = STATE_IDLE;
            return cc26x2_cc13x2_rf_rx_start();

        /* to start a TX we just need to stop receiving,
         * and for doing nothing just stop receiving too */
        case IEEE802154_TRX_STATE_TRX_OFF:
        case IEEE802154_TRX_STATE_TX_ON:
            if (cc26x2_cc13x2_rf_rx_is_on()) {
                cc26x2_cc13x2_rf_rx_stop();
            }
            return 0;

        default:
            assert(0);
    }

    return 0;
}

static int _confirm_set_trx_state(ieee802154_dev_t *dev)
{
    (void)dev;
    return 0;
}

static int _request_cca(ieee802154_dev_t *dev)
{
    int ret;
    (void)dev;

    RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
    RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_LAST_COMMAND_DONE;

    if ((ret = cc26x2_cc13x2_rf_request_cca()) != 0) {
        RFC_DBELL_NONBUF->RFCPEIEN &= ~CPE_IRQ_LAST_COMMAND_DONE;
    }
    return ret;
}

static int _confirm_cca(ieee802154_dev_t *dev)
{
    (void)dev;

    int res;
    switch (_cca_result) {
        case CCA_RESULT_IDLE:
            res = true;
            break;
        case CCA_RESULT_BUSY:
            res = false;
            break;
        case CCA_RESULT_NONE:
        default:
            res = -EAGAIN;
    }

    _cca_result = CCA_RESULT_NONE;
    return res;
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

    return 0;
}

static int _set_hw_addr_filter(ieee802154_dev_t *dev,
                               const network_uint16_t *short_addr,
                               const eui64_t *ext_addr,
                               const uint16_t *pan_id)
{
    (void)dev;

    if (short_addr) {
        cc26x2_cc13x2_rf_set_short_addr(*short_addr);
    }

    if (ext_addr) {
        cc26x2_cc13x2_rf_set_long_addr(*ext_addr);
    }

    if (pan_id) {
        cc26x2_cc13x2_rf_set_pan_id(*pan_id);
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

static int _set_rx_mode(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode)
{
    (void)dev;

    bool ackf = true;

    switch (mode) {
        case IEEE802154_RX_AACK_DISABLED:
            _ack[0] = 0;
            break;
        case IEEE802154_RX_AACK_ENABLED:
            _ack[0] = IEEE802154_FCF_TYPE_ACK;
            break;
        case IEEE802154_RX_AACK_FRAME_PENDING:
            _ack[0] = IEEE802154_FCF_TYPE_ACK | IEEE802154_FCF_FRAME_PEND;
            break;
        case IEEE802154_RX_PROMISC:
            _cfg.promisc = true;
            break;
        case IEEE802154_RX_WAIT_FOR_ACK:
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

static const ieee802154_radio_ops_t cc26x2_cc13x2_rf_ops = {
    .caps = IEEE802154_CAP_SUB_GHZ
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_PHY_MR_FSK,

    .write = _write,
    .read = _read,
    .request_transmit  = _request_transmit,
    .confirm_transmit = _confirm_transmit,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_set_trx_state = _request_set_trx_state,
    .confirm_set_trx_state = _confirm_set_trx_state,
    .request_cca = _request_cca,
    .confirm_cca = _confirm_cca,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_hw_addr_filter = _set_hw_addr_filter,
    .set_csma_params = _set_csma_params,
    .set_rx_mode = _set_rx_mode,
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
        _tx_state = STATE_TX_ACK;
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
#if IS_USED(MODULE_NETDEV_IEEE802154_SUBMAC)
    netdev_t *netdev = (netdev_t*) dev;
    netdev_register(netdev, NETDEV_CC26XX_CC13XX, 0);
    DEBUG("[cc26x2_cc13x2_rf]: init submac\n")
    netdev_ieee802154_submac_init(&dev->netdev, &cc26x2_cc13x2_rf_hal_dev);
#endif

    cc26x2_cc13x2_rf_init();
}
