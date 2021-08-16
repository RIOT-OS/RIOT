/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "net/ieee802154/submac.h"
#include "net/ieee802154.h"
#include "xtimer.h"
#include "random.h"
#include "luid.h"
#include "kernel_defines.h"
#include "errno.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define CSMA_SENDER_BACKOFF_PERIOD_UNIT_US  (320U)
#define ACK_TIMEOUT_US                      (864U)

static char *str_states[IEEE802154_FSM_STATE_NUMOF] = {
    "INVALID",
    "RX",
    "IDLE",
    "PREPARE",
    "TX",
    "WAIT_FOR_ACK",
};

static char *str_ev[IEEE802154_FSM_EV_NUMOF] = {
    "TX_DONE",
    "RX_DONE",
    "CRC_ERROR",
    "ACK_TIMEOUT",
    "BH",
    "REQUEST_TX",
    "REQUEST_SET_RX_ON",
    "REQUEST_SET_IDLE",
};

static inline void _req_set_trx_state_wait_busy(ieee802154_dev_t *dev,
                                                ieee802154_trx_state_t state)
{
    int res;

    /* Some radios will run some house keeping tasks on event suchs as RX_DONE
     * (e.g sending ACK frames) or TX_DONE. In such case we need to wait until
     * the radio is not busy.
     */
    do {
        res = ieee802154_radio_request_set_trx_state(dev, state);
    } while (res == -EBUSY);

    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
    assert(res >= 0);
}

static inline bool _does_handle_ack(ieee802154_dev_t *dev)
{
    return ieee802154_radio_has_frame_retrans(dev) ||
           ieee802154_radio_has_irq_ack_timeout(dev);
}

static inline bool _does_handle_csma(ieee802154_dev_t *dev)
{
    return ieee802154_radio_has_frame_retrans(dev) ||
           ieee802154_radio_has_auto_csma(dev);
}

static bool _has_retrans_left(ieee802154_submac_t *submac)
{
    return submac->retrans < IEEE802154_SUBMAC_MAX_RETRANSMISSIONS;
}

static ieee802154_fsm_state_t _tx_end(ieee802154_submac_t *submac, int status,
                                      ieee802154_tx_info_t *info)
{
    submac->wait_for_ack = false;
    _req_set_trx_state_wait_busy(&submac->dev, IEEE802154_TRX_STATE_TRX_OFF);
    submac->cb->tx_done(submac, status, info);
    return IEEE802154_FSM_STATE_IDLE;
}

static void _print_debug(ieee802154_fsm_state_t old, ieee802154_fsm_state_t new,
                         ieee802154_fsm_ev_t ev)
{
    DEBUG("%s--(%s)->%s\n", str_states[old], str_ev[ev], str_states[new]);
}

static ieee802154_fsm_state_t _handle_tx_no_ack(ieee802154_submac_t *submac)
{
    /* In case of ACK Timeout, either trigger retransmissions or end
     * the TX procedure */
    if (_has_retrans_left(submac)) {
        submac->retrans++;
        _req_set_trx_state_wait_busy(&submac->dev, IEEE802154_TRX_STATE_TX_ON);
        ieee802154_submac_bh_request(submac);
        return IEEE802154_FSM_STATE_PREPARE;
    }
    else {
        ieee802154_radio_set_frame_filter_mode(&submac->dev, IEEE802154_FILTER_ACCEPT);
        return _tx_end(submac, TX_STATUS_NO_ACK, NULL);
    }
}

static int _handle_fsm_ev_request_tx(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = &submac->dev;

    /* Set state to TX_ON */
    int res = ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_TX_ON);

    if (res < 0) {
        return res;
    }
    else {
        while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
        /* write frame to radio */
        ieee802154_radio_write(dev, submac->psdu);
        ieee802154_submac_bh_request(submac);
        return 0;
    }
}

static ieee802154_fsm_state_t _fsm_state_rx(ieee802154_submac_t *submac, ieee802154_fsm_ev_t ev)
{
    ieee802154_dev_t *dev = &submac->dev;

    switch (ev) {
    case IEEE802154_FSM_EV_REQUEST_TX:
        if (_handle_fsm_ev_request_tx(submac) < 0) {
            return IEEE802154_FSM_STATE_RX;
        }
        return IEEE802154_FSM_STATE_PREPARE;
    case IEEE802154_FSM_EV_RX_DONE:
        /* Make sure it's not an ACK frame */
        if (ieee802154_radio_len(&submac->dev) > (int)IEEE802154_MIN_FRAME_LEN) {
            _req_set_trx_state_wait_busy(&submac->dev, IEEE802154_TRX_STATE_TRX_OFF);
            submac->cb->rx_done(submac);
            return IEEE802154_FSM_STATE_IDLE;
        }
        else {
            ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
            /* Keep on current state */
            return IEEE802154_FSM_STATE_RX;
        }
    case IEEE802154_FSM_EV_CRC_ERROR:
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        /* Keep on current state */
        return IEEE802154_FSM_STATE_RX;

    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
        /* Try to turn off the transceiver */
        if ((ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_TRX_OFF)) < 0) {
            /* Keep on current state */
            return IEEE802154_FSM_STATE_RX;
        }
        while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
        return IEEE802154_FSM_STATE_IDLE;

    default:
        break;
    }

    return IEEE802154_FSM_STATE_INVALID;
}

static ieee802154_fsm_state_t _fsm_state_idle(ieee802154_submac_t *submac, ieee802154_fsm_ev_t ev)
{
    ieee802154_dev_t *dev = &submac->dev;

    switch (ev) {
    case IEEE802154_FSM_EV_REQUEST_TX:
        if (_handle_fsm_ev_request_tx(submac) < 0) {
            return IEEE802154_FSM_STATE_IDLE;
        }
        return IEEE802154_FSM_STATE_PREPARE;
    case IEEE802154_FSM_EV_REQUEST_SET_RX_ON:
        /* Try to go turn on the transceiver */
        if ((ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON)) < 0) {
            /* Keep on current state */
            return IEEE802154_FSM_STATE_IDLE;
        }
        while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
        return IEEE802154_FSM_STATE_RX;
    case IEEE802154_FSM_EV_RX_DONE:
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* This might happen in case there's a race condition between ACK_TIMEOUT
         * and TX_DONE. We simply discard the frame and keep the state as
         * it is
         */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_FSM_STATE_IDLE;
    default:
        break;
    }
    return IEEE802154_FSM_STATE_INVALID;
}

static ieee802154_fsm_state_t _fsm_state_prepare(ieee802154_submac_t *submac,
                                                 ieee802154_fsm_ev_t ev)
{
    ieee802154_dev_t *dev = &submac->dev;

    switch (ev) {
    case IEEE802154_FSM_EV_BH:
        if (!_does_handle_csma(dev)) {
            /* delay for an adequate random backoff period */
            uint32_t bp = (random_uint32() & submac->backoff_mask) *
                          CSMA_SENDER_BACKOFF_PERIOD_UNIT_US;

            xtimer_usleep(bp);
            /* Prepare for next iteration */
            uint8_t curr_be = (submac->backoff_mask + 1) >> 1;
            if (curr_be < submac->be.max) {
                submac->backoff_mask = (submac->backoff_mask << 1) | 1;
            }
        }

        while (ieee802154_radio_request_transmit(dev) == -EBUSY) {}
        return IEEE802154_FSM_STATE_TX;
    case IEEE802154_FSM_EV_RX_DONE:
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* This might happen in case there's a race condition between ACK_TIMEOUT
         * and TX_DONE. We simply discard the frame and keep the state as
         * it is
         */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_FSM_STATE_PREPARE;
    default:
        break;
    }

    return IEEE802154_FSM_STATE_INVALID;
}

static ieee802154_fsm_state_t _fsm_state_tx_process_tx_done(ieee802154_submac_t *submac,
                                                            ieee802154_tx_info_t *info)
{
    ieee802154_dev_t *dev = &submac->dev;

    switch (info->status) {
    case TX_STATUS_FRAME_PENDING:
        assert(_does_handle_ack(&submac->dev));
    /* FALL-THRU */
    case TX_STATUS_SUCCESS:
        submac->csma_retries_nb = 0;
        /* If the radio handles ACK, the TX_DONE event marks completion of
        * the transmission procedure. Report TX done to the upper layer */
        if (_does_handle_ack(&submac->dev) || !submac->wait_for_ack) {
            return _tx_end(submac, info->status, info);
        }
        /* If the radio doesn't handle ACK, set the transceiver state to RX_ON
         * and enable the ACK filter */
        else {
            ieee802154_radio_set_frame_filter_mode(dev, IEEE802154_FILTER_ACK_ONLY);
            _req_set_trx_state_wait_busy(dev, IEEE802154_TRX_STATE_RX_ON);

            /* Handle ACK reception */
            ieee802154_submac_ack_timer_set(submac, ACK_TIMEOUT_US);
            return IEEE802154_FSM_STATE_WAIT_FOR_ACK;
        }
        break;
    case TX_STATUS_NO_ACK:
        assert(_does_handle_ack(&submac->dev));
        submac->csma_retries_nb = 0;
        return _handle_tx_no_ack(submac);
    case TX_STATUS_MEDIUM_BUSY:
        /* If radio has retransmissions or CSMA-CA, this means the CSMA-CA
         * procedure failed. We finish the SubMAC operation and report
         * medium busy
         */
        if (_does_handle_csma(&submac->dev)
            || submac->csma_retries_nb++ >= submac->csma_retries) {
            return _tx_end(submac, info->status, info);
        }
        /* Otherwise, this is a failed CCA attempt. Proceed with CSMA-CA */
        else {
            /* The HAL should guarantee that's still possible to transmit
             * in the current state, since the radio is still in TX_ON.
             * Therefore, this is valid */
            ieee802154_submac_bh_request(submac);
            return IEEE802154_FSM_STATE_PREPARE;
        }
    }
    return IEEE802154_FSM_STATE_INVALID;
}

static ieee802154_fsm_state_t _fsm_state_tx(ieee802154_submac_t *submac, ieee802154_fsm_ev_t ev)
{
    int res;
    ieee802154_tx_info_t info;

    switch (ev) {
    case IEEE802154_FSM_EV_TX_DONE:
        res = ieee802154_radio_confirm_transmit(&submac->dev, &info);
        (void)res;
        assert(res >= 0);
        return _fsm_state_tx_process_tx_done(submac, &info);
    case IEEE802154_FSM_EV_RX_DONE:
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* This might happen in case there's a race condition between ACK_TIMEOUT
         * and TX_DONE. We simply discard the frame and keep the state as
         * it is
         */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_FSM_STATE_TX;
    default:
        break;
    }

    return IEEE802154_FSM_STATE_INVALID;
}

static ieee802154_fsm_state_t _fsm_state_wait_for_ack(ieee802154_submac_t *submac,
                                                      ieee802154_fsm_ev_t ev)
{
    uint8_t ack[3];

    switch (ev) {
    case IEEE802154_FSM_EV_RX_DONE:
        assert(!ieee802154_radio_has_irq_ack_timeout(&submac->dev));
        if (ieee802154_radio_read(&submac->dev, ack, 3, NULL) &&
            ack[0] & IEEE802154_FCF_TYPE_ACK) {
            ieee802154_submac_ack_timer_cancel(submac);
            ieee802154_tx_info_t tx_info;
            tx_info.retrans = submac->retrans;
            bool fp = (ack[0] & IEEE802154_FCF_FRAME_PEND);
            ieee802154_radio_set_frame_filter_mode(&submac->dev, IEEE802154_FILTER_ACCEPT);
            return _tx_end(submac, fp ? TX_STATUS_FRAME_PENDING : TX_STATUS_SUCCESS,
                           &tx_info);
        }
        return IEEE802154_FSM_STATE_WAIT_FOR_ACK;
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* Received invalid ACK. Drop frame */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_FSM_STATE_WAIT_FOR_ACK;
    case IEEE802154_FSM_EV_ACK_TIMEOUT:
        return _handle_tx_no_ack(submac);
    default:
        break;
    }
    return IEEE802154_FSM_STATE_INVALID;
}

ieee802154_fsm_state_t ieee802154_submac_process_ev(ieee802154_submac_t *submac,
                                                    ieee802154_fsm_ev_t ev)
{
    ieee802154_fsm_state_t new_state;

    switch (submac->fsm_state) {
    case IEEE802154_FSM_STATE_RX:
        new_state = _fsm_state_rx(submac, ev);
        break;
    case IEEE802154_FSM_STATE_IDLE:
        new_state = _fsm_state_idle(submac, ev);
        break;
    case IEEE802154_FSM_STATE_PREPARE:
        new_state = _fsm_state_prepare(submac, ev);
        break;
    case IEEE802154_FSM_STATE_TX:
        new_state = _fsm_state_tx(submac, ev);
        break;
    case IEEE802154_FSM_STATE_WAIT_FOR_ACK:
        new_state = _fsm_state_wait_for_ack(submac, ev);
        break;
    default:
        new_state = IEEE802154_FSM_STATE_INVALID;
    }

    if (new_state == IEEE802154_FSM_STATE_INVALID) {
        _print_debug(submac->fsm_state, new_state, ev);
        assert(false);
    }
    submac->fsm_state = new_state;
    return submac->fsm_state;
}

int ieee802154_send(ieee802154_submac_t *submac, const iolist_t *iolist)
{
    ieee802154_fsm_state_t current_state = submac->fsm_state;

    if (current_state != IEEE802154_FSM_STATE_RX && current_state != IEEE802154_FSM_STATE_IDLE) {
        return -EBUSY;
    }

    uint8_t *buf = iolist->iol_base;
    bool cnf = buf[0] & IEEE802154_FCF_ACK_REQ;

    submac->wait_for_ack = cnf;
    submac->psdu = iolist;
    submac->retrans = 0;
    submac->csma_retries_nb = 0;
    submac->backoff_mask = (1 << submac->be.min) - 1;

    if (ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_REQUEST_TX)
        != IEEE802154_FSM_STATE_PREPARE) {
        return -EBUSY;
    }
    return 0;
}

int ieee802154_submac_init(ieee802154_submac_t *submac, const network_uint16_t *short_addr,
                           const eui64_t *ext_addr)
{
    ieee802154_dev_t *dev = &submac->dev;

    submac->fsm_state = IEEE802154_FSM_STATE_RX;

    int res;

    if ((res = ieee802154_radio_request_on(dev)) < 0) {
        return res;
    }

    /* generate EUI-64 and short address */
    memcpy(&submac->ext_addr, ext_addr, sizeof(eui64_t));
    memcpy(&submac->short_addr, short_addr, sizeof(network_uint16_t));
    submac->panid = CONFIG_IEEE802154_DEFAULT_PANID;

    submac->be.min = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE;
    submac->csma_retries = CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES;
    submac->be.max = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE;

    submac->tx_pow = CONFIG_IEEE802154_DEFAULT_TXPOWER;

    if (ieee802154_radio_has_24_ghz(dev)) {
        submac->channel_num = CONFIG_IEEE802154_DEFAULT_CHANNEL;

        /* 2.4 GHz only use page 0 */
        submac->channel_page = 0;
    }
    else {
        submac->channel_num = CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL;
        submac->channel_page = CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE;
    }

    /* Get supported PHY modes */
    int supported_phy_modes = ieee802154_radio_get_phy_modes(dev);

    assert(supported_phy_modes != 0);

    uint32_t default_phy_cap = ieee802154_phy_mode_to_cap(CONFIG_IEEE802154_DEFAULT_PHY_MODE);

    /* Check if configuration provides valid PHY */
    if (CONFIG_IEEE802154_DEFAULT_PHY_MODE != IEEE802154_PHY_DISABLED &&
        (supported_phy_modes & default_phy_cap)) {
        /* Check if default PHY is supported */
        submac->phy_mode = CONFIG_IEEE802154_DEFAULT_PHY_MODE;
    }
    else {
        /* Get first set bit, and use it as the default,
         *
         * by this order, the priority is defined on the ieee802154_rf_caps_t
         * definition, first IEEE 802.15.4-2006 PHY modes, then
         * IEEE 802.15.4g-2012 PHY modes. */
        unsigned bit = bitarithm_lsb(supported_phy_modes);

        submac->phy_mode = ieee802154_cap_to_phy_mode(1 << bit);
    }

    /* If the radio is still not in TRX_OFF state, spin */
    while (ieee802154_radio_confirm_on(dev) == -EAGAIN) {}

    /* Configure address filter */
    ieee802154_radio_config_addr_filter(dev, IEEE802154_AF_SHORT_ADDR, &submac->short_addr);
    ieee802154_radio_config_addr_filter(dev, IEEE802154_AF_EXT_ADDR, &submac->ext_addr);
    ieee802154_radio_config_addr_filter(dev, IEEE802154_AF_PANID, &submac->panid);

    /* Configure PHY settings (mode, channel, TX power) */
    ieee802154_phy_conf_t conf =
    { .phy_mode = submac->phy_mode,
      .channel = submac->channel_num,
      .page = submac->channel_page,
      .pow = submac->tx_pow };

    ieee802154_radio_config_phy(dev, &conf);
    ieee802154_radio_set_cca_threshold(dev,
                                       CONFIG_IEEE802154_CCA_THRESH_DEFAULT);
    assert(res >= 0);

    _req_set_trx_state_wait_busy(dev, IEEE802154_TRX_STATE_RX_ON);

    return res;
}

int ieee802154_set_phy_conf(ieee802154_submac_t *submac, uint16_t channel_num,
                            uint8_t channel_page, int8_t tx_pow)
{
    ieee802154_dev_t *dev = &submac->dev;
    const ieee802154_phy_conf_t conf =
    { .phy_mode = submac->phy_mode,
      .channel = channel_num,
      .page = channel_page,
      .pow = tx_pow };
    int res;
    ieee802154_fsm_state_t current_state = submac->fsm_state;

    /* Changing state can be only performed on IDLE or RX state */
    if (current_state != IEEE802154_FSM_STATE_RX && current_state != IEEE802154_FSM_STATE_IDLE) {
        return -EBUSY;
    }

    /* If the radio is listening, turn it off first */
    if (current_state == IEEE802154_FSM_STATE_RX) {
        if ((res = ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_TRX_OFF)) < 0) {
            return res;
        }
    }

    res = ieee802154_radio_config_phy(dev, &conf);

    if (res >= 0) {
        submac->channel_num = channel_num;
        submac->channel_page = channel_page;
        submac->tx_pow = tx_pow;
    }
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

    /* Go back to RX if needed */
    if (current_state == IEEE802154_FSM_STATE_RX) {
        _req_set_trx_state_wait_busy(dev, IEEE802154_TRX_STATE_RX_ON);
    }

    return res;
}

int ieee802154_set_rx(ieee802154_submac_t *submac)
{
    ieee802154_fsm_state_t current_state = submac->fsm_state;
    ieee802154_fsm_state_t next_state;
    int res = -EBUSY;

    switch (current_state) {
    case IEEE802154_FSM_STATE_RX:
        res = -EALREADY;
        break;
    case IEEE802154_FSM_STATE_IDLE:
        next_state = ieee802154_submac_process_ev(submac,
                                                  IEEE802154_FSM_EV_REQUEST_SET_RX_ON);
        if (next_state == IEEE802154_FSM_STATE_RX) {
            res = 0;
        }
        break;
    default:
        break;
    }

    return res;
}

int ieee802154_set_idle(ieee802154_submac_t *submac)
{
    ieee802154_fsm_state_t current_state = submac->fsm_state;
    ieee802154_fsm_state_t next_state;
    int res = -EBUSY;

    switch (current_state) {
    case IEEE802154_FSM_STATE_IDLE:
        res = -EALREADY;
        break;
    case IEEE802154_FSM_STATE_RX:
        next_state = ieee802154_submac_process_ev(submac,
                                                  IEEE802154_FSM_EV_REQUEST_SET_IDLE);
        if (next_state == IEEE802154_FSM_STATE_IDLE) {
            res = 0;
        }
        break;
    default:
        break;
    }

    return res;

}

/** @} */
