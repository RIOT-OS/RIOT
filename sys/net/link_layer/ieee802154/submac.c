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

#include <stdio.h>
#include <string.h>
#include "net/ieee802154/submac.h"
#include "net/ieee802154.h"
#include "xtimer.h"
#include "random.h"
#include "luid.h"
#include "kernel_defines.h"
#include "errno.h"
#include <assert.h>

#define CSMA_SENDER_BACKOFF_PERIOD_UNIT_MS  (320U)
#define ACK_TIMEOUT_US                      (864U)

static void _handle_tx_no_ack(ieee802154_submac_t *submac);

static void _tx_end(ieee802154_submac_t *submac, int status,
                    ieee802154_tx_info_t *info)
{
    ieee802154_dev_t *dev = submac->dev;

    ieee802154_radio_request_set_trx_state(dev, submac->state == IEEE802154_STATE_LISTEN ? IEEE802154_TRX_STATE_RX_ON : IEEE802154_TRX_STATE_TRX_OFF);

    submac->wait_for_ack = false;
    submac->tx = false;
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
    submac->cb->tx_done(submac, status, info);
}

static inline bool _does_handle_ack(ieee802154_dev_t *dev)
{
    return ieee802154_radio_has_frame_retrans(dev) ||
           ieee802154_radio_has_irq_ack_timeout(dev);
}

static int _perform_csma_ca(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;

    if (submac->csma_retries_nb <= submac->csma_retries) {
        ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_TX_ON);
        /* delay for an adequate random backoff period */
        uint32_t bp = (random_uint32() & submac->backoff_mask) *
                      CSMA_SENDER_BACKOFF_PERIOD_UNIT_MS;

        xtimer_usleep(bp);

        /* try to send after a CCA */
        while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

        while (ieee802154_radio_request_transmit(dev) == -EBUSY) {}

        /* Prepare for next iteration */
        if (submac->backoff_mask + 1 < submac->be.max) {
            submac->backoff_mask = (submac->backoff_mask << 1) | 1;
        }
        else {
            submac->backoff_mask = (1 << submac->be.max) - 1;
        }

        submac->csma_retries_nb++;
    }
    else {
        ieee802154_radio_set_frame_filter_mode(dev, IEEE802154_FILTER_ACCEPT);
        _tx_end(submac, TX_STATUS_MEDIUM_BUSY, NULL);
    }

    return 0;
}

/**
 * @brief Perform CSMA-CA transmission (possibly with retransmission)
 *
 * If radio supports @ref IEEE802154_CAP_FRAME_RETRANS, the device will automatically retransmit.
 * If radio supports @ref IEEE802154_CAP_AUTO_CSMA, this function will use the
 * internal CSMA-CA acceleration to perform the transmission.
 *
 * @param submac pointer to the SubMAC
 *
 * @return 0 on success
 * @return negative errno on error
 */
int ieee802154_csma_ca_transmit(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;

    /* If radio has Auto CSMA-CA or Frame Retransmissions, simply send and wait for the transmit confirmation. */
    if (ieee802154_radio_has_auto_csma(dev) ||
        ieee802154_radio_has_frame_retrans(dev)) {

        /* Make sure we are in TX_ON */
        ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_TX_ON);
        while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

        int res;
        while ((res = ieee802154_radio_request_transmit(dev)) == -EBUSY) {}
        return res;
    }
    else {
        submac->csma_retries_nb = 0;
        submac->backoff_mask = (1 << submac->be.min) - 1;
        _perform_csma_ca(submac);
    }

    return 0;
}

static bool _has_retrans_left(ieee802154_submac_t *submac)
{
    return submac->retrans < IEEE802154_SUBMAC_MAX_RETRANSMISSIONS;
}

static void _perform_retrans(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;

    if (_has_retrans_left(submac)) {
        submac->retrans++;
        ieee802154_csma_ca_transmit(submac);
    }
    else {
        ieee802154_radio_set_frame_filter_mode(dev, IEEE802154_FILTER_ACCEPT);
        _tx_end(submac, TX_STATUS_NO_ACK, NULL);
    }
}

void ieee802154_submac_ack_timeout_fired(ieee802154_submac_t *submac)
{
    /* This is required to avoid race conditions */
    if (submac->wait_for_ack) {
        _handle_tx_no_ack(submac);
    }
}

void ieee802154_submac_crc_error_cb(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;
    /* switch back to RX_ON state */
    ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON);
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
}

/* All callbacks run in the same context */
void ieee802154_submac_rx_done_cb(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;

    if (!_does_handle_ack(dev) && submac->wait_for_ack) {
        uint8_t ack[3];

        if (ieee802154_radio_read(dev, ack, 3, NULL) &&
            ack[0] & IEEE802154_FCF_TYPE_ACK) {
            ieee802154_submac_ack_timer_cancel(submac);
            ieee802154_tx_info_t tx_info;
            tx_info.retrans = submac->retrans;
            bool fp = (ack[0] & IEEE802154_FCF_FRAME_PEND);
            ieee802154_radio_set_frame_filter_mode(submac->dev, IEEE802154_FILTER_ACCEPT);
            _tx_end(submac, fp ? TX_STATUS_FRAME_PENDING : TX_STATUS_SUCCESS,
                    &tx_info);
        }
    }
    else {
        submac->cb->rx_done(submac);

        /* Only set the radio to the SubMAC default state only if the upper
         * layer didn't try to send more data. Otherwise there's risk of not
         * being compliant with the Radio HAL API (e.g the radio might try
         * to set a different state in the middle of a transmission).
         */
        if (submac->tx) {
            return;
        }

        /* The Radio HAL will be in "FB Lock" state. We need to do a state
         * transition here in order to release it */
        ieee802154_trx_state_t next_state = submac->state == IEEE802154_STATE_LISTEN ? IEEE802154_TRX_STATE_RX_ON : IEEE802154_TRX_STATE_TRX_OFF;

        /* Some radios will run some house keeping tasks on RX_DONE (e.g
         * sending ACK frames). In such case we need to wait until the radio is
         * not busy
         */
        while (ieee802154_radio_request_set_trx_state(submac->dev, next_state) == -EBUSY);
        while (ieee802154_radio_confirm_set_trx_state(submac->dev) == -EAGAIN) {}
    }
}

static void _handle_tx_success(ieee802154_submac_t *submac,
                               ieee802154_tx_info_t *info)
{
    ieee802154_dev_t *dev = submac->dev;

    ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON);
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

    if (ieee802154_radio_has_frame_retrans(dev) ||
        ieee802154_radio_has_irq_ack_timeout(dev) || !submac->wait_for_ack) {
        if (!ieee802154_radio_has_frame_retrans_info(dev)) {
            info->retrans = -1;
        }
        _tx_end(submac, info->status, info);
    }
    else {
        ieee802154_radio_set_frame_filter_mode(dev, IEEE802154_FILTER_ACK_ONLY);

        /* Handle ACK reception */
        ieee802154_submac_ack_timer_set(submac, ACK_TIMEOUT_US);
    }
}

static void _handle_tx_medium_busy(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;

    if (ieee802154_radio_has_frame_retrans(dev) ||
        ieee802154_radio_has_auto_csma(dev)) {
        ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON);
        _tx_end(submac, TX_STATUS_MEDIUM_BUSY, NULL);
    }
    else {
        /* CCA failed. Continue with the CSMA-CA algorithm */
        _perform_csma_ca(submac);
    }
}

static void _handle_tx_no_ack(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;

    if (ieee802154_radio_has_frame_retrans(dev)) {
        ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON);
        _tx_end(submac, TX_STATUS_NO_ACK, NULL);
    }
    else {
        /* Perform retransmissions */
        _perform_retrans(submac);
    }
}

void ieee802154_submac_tx_done_cb(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = submac->dev;
    ieee802154_tx_info_t info;

    ieee802154_radio_confirm_transmit(dev, &info);

    switch (info.status) {
    case TX_STATUS_MEDIUM_BUSY:
        _handle_tx_medium_busy(submac);
        break;
    case TX_STATUS_NO_ACK:
        _handle_tx_no_ack(submac);
        break;
    case TX_STATUS_SUCCESS:
    case TX_STATUS_FRAME_PENDING:
        _handle_tx_success(submac, &info);
        break;
    default:
        assert(false);
        break;
    }
}

int ieee802154_send(ieee802154_submac_t *submac, const iolist_t *iolist)
{
    ieee802154_dev_t *dev = submac->dev;

    uint8_t *buf = iolist->iol_base;
    bool cnf = buf[0] & IEEE802154_FCF_ACK_REQ;

    if (submac->state == IEEE802154_STATE_OFF) {
        return -ENETDOWN;
    }

    if (submac->tx ||
        ieee802154_radio_request_set_trx_state(dev,
                                               IEEE802154_TRX_STATE_TX_ON) < 0) {
        return -EBUSY;
    }

    submac->tx = true;

    ieee802154_radio_write(dev, iolist);
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

    submac->wait_for_ack = cnf;
    submac->retrans = 0;

    ieee802154_csma_ca_transmit(submac);
    return 0;
}

int ieee802154_submac_init(ieee802154_submac_t *submac, const network_uint16_t *short_addr,
                           const eui64_t *ext_addr)
{
    ieee802154_dev_t *dev = submac->dev;

    submac->tx = false;
    submac->state = IEEE802154_STATE_LISTEN;

    ieee802154_radio_request_on(dev);

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
      .channel = CONFIG_IEEE802154_DEFAULT_CHANNEL,
      .page = CONFIG_IEEE802154_DEFAULT_CHANNEL,
      .pow = CONFIG_IEEE802154_DEFAULT_TXPOWER };

    ieee802154_radio_config_phy(dev, &conf);
    assert(ieee802154_radio_set_cca_threshold(dev,
                                              CONFIG_IEEE802154_CCA_THRESH_DEFAULT) >= 0);

    ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON);

    return 0;
}

int ieee802154_set_phy_conf(ieee802154_submac_t *submac, uint16_t channel_num,
                            uint8_t channel_page, int8_t tx_pow)
{
    ieee802154_dev_t *dev = submac->dev;
    const ieee802154_phy_conf_t conf =
    { .phy_mode = submac->phy_mode,
      .channel = channel_num,
      .page = channel_page,
      .pow = tx_pow };

    if (submac->state == IEEE802154_STATE_OFF) {
        return -ENETDOWN;
    }

    int res;
    if ((res = ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_TRX_OFF)) < 0) {
        return res;
    }

    res = ieee802154_radio_config_phy(dev, &conf);

    if (res >= 0) {
        submac->channel_num = channel_num;
        submac->channel_page = channel_page;
        submac->tx_pow = tx_pow;
    }
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

    ieee802154_radio_request_set_trx_state(dev, submac->state == IEEE802154_STATE_LISTEN ? IEEE802154_TRX_STATE_RX_ON : IEEE802154_TRX_STATE_TRX_OFF);
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}

    return res;
}

int ieee802154_set_state(ieee802154_submac_t *submac, ieee802154_submac_state_t state)
{
    int res;

    ieee802154_dev_t *dev = submac->dev;

    if (submac->tx) {
        return -EBUSY;
    }

    if (state == submac->state) {
        return -EALREADY;
    }

    /* Wake up the radio if it was off */
    if (submac->state == IEEE802154_STATE_OFF) {
        if ((res = ieee802154_radio_request_on(dev)) < 0) {
            return res;
        }
        while (ieee802154_radio_confirm_on(dev) == -EAGAIN);
    }

    if (state == IEEE802154_STATE_OFF) {
        res = ieee802154_radio_off(dev);
    }
    else {
        ieee802154_trx_state_t new_state =
                    state == IEEE802154_STATE_IDLE
                    ? IEEE802154_TRX_STATE_TRX_OFF
                    : IEEE802154_TRX_STATE_RX_ON;

        if ((res = ieee802154_radio_request_set_trx_state(dev, new_state)) < 0) {
            return res;
        }

        while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN);
    }

    submac->state = state;
    return res;
}

/** @} */
