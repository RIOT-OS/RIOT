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
#include "ztimer.h"
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
    return submac->retrans < CONFIG_IEEE802154_DEFAULT_MAX_FRAME_RETRANS;
}

static ieee802154_fsm_state_t _tx_end(ieee802154_submac_t *submac, int status,
                                      ieee802154_tx_info_t *info)
{
    int res;

    /* This is required to prevent unused variable warnings */
    (void) res;

    submac->wait_for_ack = false;

    res = ieee802154_radio_set_idle(&submac->dev, true);

    assert(res >= 0);
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
    int res;

    /* This is required to prevent unused variable warnings */
    (void) res;

    /* In case of ACK Timeout, either trigger retransmissions or end
     * the TX procedure */
    if (_has_retrans_left(submac)) {
        submac->retrans++;
        res = ieee802154_radio_set_idle(&submac->dev, true);
        assert(res >= 0);
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
    int res = ieee802154_radio_set_idle(dev, false);

    if (res < 0) {
        return res;
    }
    else {
        /* write frame to radio */
        ieee802154_radio_write(dev, submac->psdu);
        ieee802154_submac_bh_request(submac);
        return 0;
    }
}

static ieee802154_fsm_state_t _fsm_state_rx(ieee802154_submac_t *submac, ieee802154_fsm_ev_t ev)
{
    ieee802154_dev_t *dev = &submac->dev;
    int res;

    /* This is required to prevent unused variable warnings */
    (void) res;

    switch (ev) {
    case IEEE802154_FSM_EV_REQUEST_TX:
        if (_handle_fsm_ev_request_tx(submac) < 0) {
            return IEEE802154_FSM_STATE_RX;
        }
        return IEEE802154_FSM_STATE_PREPARE;
    case IEEE802154_FSM_EV_RX_DONE:
        /* Make sure it's not an ACK frame */
        while (ieee802154_radio_set_idle(&submac->dev, false) < 0) {}
        if (ieee802154_radio_len(&submac->dev) > (int)IEEE802154_MIN_FRAME_LEN) {
            submac->cb->rx_done(submac);
            return IEEE802154_FSM_STATE_IDLE;
        }
        else {
            ieee802154_radio_read(&submac->dev, NULL, 0, NULL);

            /* If the radio doesn't support RX Continuous, go to RX */
            res = ieee802154_radio_set_rx(&submac->dev);
            assert(res >= 0);

            /* Keep on current state */
            return IEEE802154_FSM_STATE_RX;
        }
    case IEEE802154_FSM_EV_CRC_ERROR:
        while (ieee802154_radio_set_idle(&submac->dev, false) < 0) {}
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        /* If the radio doesn't support RX Continuous, go to RX */
        res = ieee802154_radio_set_rx(&submac->dev);
        assert(res >= 0);
        /* Keep on current state */
        return IEEE802154_FSM_STATE_RX;

    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
        /* Try to turn off the transceiver */
        if ((ieee802154_radio_request_set_idle(dev, false)) < 0) {
            /* Keep on current state */
            return IEEE802154_FSM_STATE_RX;
        }
        while (ieee802154_radio_confirm_set_idle(dev) == -EAGAIN) {}
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
        if ((ieee802154_radio_set_rx(dev) < 0)) {
            /* Keep on current state */
            return IEEE802154_FSM_STATE_IDLE;
        }
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
                          submac->csma_backoff_us;

            ztimer_sleep(ZTIMER_USEC, bp);
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
    int res;

    /* This is required to prevent unused variable warnings */
    (void) res;

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
            res = ieee802154_radio_set_rx(dev);
            assert (res >= 0);

            /* Handle ACK reception */
            ieee802154_submac_ack_timer_set(submac);
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
    ieee802154_tx_info_t info;
    int res;

    /* This is required to prevent unused variable warnings */
    (void) res;

    switch (ev) {
    case IEEE802154_FSM_EV_TX_DONE:
        res = ieee802154_radio_confirm_transmit(&submac->dev, &info);
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

    if (iolist == NULL) {
        return 0;
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

/*
 * MR-OQPSK timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */

static uint8_t _mr_oqpsk_spreading(uint8_t chips, uint8_t mode)
{
    if (mode == 4) {
        return 1;
    }

    uint8_t spread = 1 << (3 - mode);

    if (chips == IEEE802154_MR_OQPSK_CHIPS_1000) {
        return 2 * spread;
    }

    if (chips == IEEE802154_MR_OQPSK_CHIPS_2000) {
        return 4 * spread;
    }

    return spread;
}

static inline uint16_t _mr_oqpsk_symbol_duration_us(uint8_t chips)
{
    /* 802.15.4g, Table 183 / Table 165 */
    switch (chips) {
    case IEEE802154_MR_OQPSK_CHIPS_100:
        return 320;
    case IEEE802154_MR_OQPSK_CHIPS_200:
        return 160;
    case IEEE802154_MR_OQPSK_CHIPS_1000:
    case IEEE802154_MR_OQPSK_CHIPS_2000:
    default:
        return 64;
    }
}

static inline uint8_t _mr_oqpsk_cca_duration_syms(uint8_t chips)
{
    /* 802.15.4g, Table 188 */
    return (chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 4 : 8;
}

static inline uint8_t _mr_oqpsk_shr_duration_syms(uint8_t chips)
{
    /* 802.15.4g, Table 184 / Table 165 */
    return (chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 48 : 72;
}

static inline uint8_t _mr_oqpsk_ack_psdu_duration_syms(uint8_t chips, uint8_t mode)
{
    /* pg. 119, section 18.3.2.14 */
    static const uint8_t sym_len[] = { 32, 32, 64, 128 };
    const uint8_t Ns = sym_len[chips];
    const uint8_t Rspread = _mr_oqpsk_spreading(chips, mode);
    /* Nd == 63, since ACK length is 5 or 7 octets only */
    const uint16_t Npsdu = Rspread * 2 * 63;

    /* phyPSDUDuration = ceiling(Npsdu / Ns) + ceiling(Npsdu / Mp) */
    /* with Mp = Np * 16, see Table 182 */
    return (Npsdu + Ns/2) / Ns + (Npsdu + 8 * Ns) / (16 * Ns);
}

static inline uint16_t _mr_oqpsk_ack_timeout_us(const ieee802154_mr_oqpks_conf_t *conf)
{
    /* see 802.15.4g-2012, p. 30 */
    uint16_t symbols = _mr_oqpsk_cca_duration_syms(conf->chips)
                     + _mr_oqpsk_shr_duration_syms(conf->chips)
                     + 15   /* PHR duration */
                     + _mr_oqpsk_ack_psdu_duration_syms(conf->chips, conf->rate_mode);

    return _mr_oqpsk_symbol_duration_us(conf->chips) * symbols
         + IEEE802154G_ATURNAROUNDTIME_US;
}

static inline uint16_t _mr_oqpsk_csma_backoff_period_us(const ieee802154_mr_oqpks_conf_t *conf)
{
    return _mr_oqpsk_cca_duration_syms(conf->chips) * _mr_oqpsk_symbol_duration_us(conf->chips)
         + IEEE802154G_ATURNAROUNDTIME_US;
}

/*
 * MR-OFDM timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */

static unsigned _mr_ofdm_frame_duration(uint8_t option, uint8_t scheme, uint8_t bytes)
{
    /* Table 150 - phySymbolsPerOctet values for MR-OFDM PHY, IEEE 802.15.4g-2012 */
    static const uint8_t quot[] = { 3, 3, 6, 12, 18, 24, 36 };

    --option;
    /* phyMaxFrameDuration = phySHRDuration + phyPHRDuration
     *                     + ceiling [(aMaxPHYPacketSize + 1) x phySymbolsPerOctet] */
    const unsigned phySHRDuration = 6;
    const unsigned phyPHRDuration = option ? 6 : 3;
    const unsigned phyPDUDuration = ((bytes + 1) * (1 << option) + quot[scheme] - 1)
                                  / quot[scheme];

    return (phySHRDuration + phyPHRDuration + phyPDUDuration) * IEEE802154_MR_OFDM_SYMBOL_TIME_US;
}

static inline uint16_t _mr_ofdm_csma_backoff_period_us(const ieee802154_mr_odmf_conf_t *conf)
{
    (void)conf;

    return IEEE802154_CCA_DURATION_IN_SYMBOLS * IEEE802154_MR_OFDM_SYMBOL_TIME_US
         + IEEE802154G_ATURNAROUNDTIME_US;
}

static inline uint16_t _mr_ofdm_ack_timeout_us(const ieee802154_mr_odmf_conf_t *conf)
{
    return _mr_ofdm_csma_backoff_period_us(conf)
         + IEEE802154G_ATURNAROUNDTIME_US
         + _mr_ofdm_frame_duration(conf->option, conf->scheme, IEEE802154_ACK_FRAME_LEN);
}

static int ieee802154_submac_config_phy(ieee802154_submac_t *submac,
                                        const ieee802154_phy_conf_t *conf)
{
    switch (conf->phy_mode) {
    case IEEE802154_PHY_OQPSK:
        submac->ack_timeout_us = ACK_TIMEOUT_US;
        submac->csma_backoff_us = CSMA_SENDER_BACKOFF_PERIOD_UNIT_US;
        break;
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    case IEEE802154_PHY_MR_OQPSK:
        submac->ack_timeout_us = _mr_oqpsk_ack_timeout_us((void *)conf);
        submac->csma_backoff_us = _mr_oqpsk_csma_backoff_period_us((void *)conf);
        break;
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
    case IEEE802154_PHY_MR_OFDM:
        submac->ack_timeout_us = _mr_ofdm_ack_timeout_us((void *)conf);
        submac->csma_backoff_us = _mr_ofdm_csma_backoff_period_us((void *)conf);
        break;
#endif
    case IEEE802154_PHY_NO_OP:
    case IEEE802154_PHY_DISABLED:
        break;
    default:
        return -EINVAL;
    }

    return ieee802154_radio_config_phy(&submac->dev, conf);
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
    union {
        ieee802154_phy_conf_t super;
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
        ieee802154_mr_oqpks_conf_t mr_oqpsk;
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
        ieee802154_mr_odmf_conf_t mr_ofdm;
#endif
    } conf;

#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    if (submac->phy_mode == IEEE802154_PHY_MR_OQPSK) {
        conf.mr_oqpsk.chips = CONFIG_IEEE802154_MR_OQPSK_DEFAULT_CHIPS;
        conf.mr_oqpsk.rate_mode = CONFIG_IEEE802154_MR_OQPSK_DEFAULT_RATE;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
    if (submac->phy_mode == IEEE802154_PHY_MR_OFDM) {
        conf.mr_ofdm.option = CONFIG_IEEE802154_MR_OFDM_DEFAULT_OPTION;
        conf.mr_ofdm.scheme = CONFIG_IEEE802154_MR_OFDM_DEFAULT_SCHEME;
    }
#endif

    conf.super.phy_mode = submac->phy_mode;
    conf.super.channel = submac->channel_num;
    conf.super.page = submac->channel_page;
    conf.super.pow = submac->tx_pow;

    ieee802154_submac_config_phy(submac, &conf.super);
    ieee802154_radio_set_cca_threshold(dev,
                                       CONFIG_IEEE802154_CCA_THRESH_DEFAULT);
    assert(res >= 0);

    while (ieee802154_radio_set_rx(dev) < 0) {}

    return res;
}

int ieee802154_set_phy_conf(ieee802154_submac_t *submac, const ieee802154_phy_conf_t *conf)
{
    ieee802154_dev_t *dev = &submac->dev;
    int res;
    ieee802154_fsm_state_t current_state = submac->fsm_state;

    /* Changing state can be only performed on IDLE or RX state */
    if (current_state != IEEE802154_FSM_STATE_RX && current_state != IEEE802154_FSM_STATE_IDLE) {
        return -EBUSY;
    }

    /* If the radio is listening, turn it off first */
    if (current_state == IEEE802154_FSM_STATE_RX) {
        if ((res = ieee802154_radio_request_set_idle(dev, false)) < 0) {
            return res;
        }
    }

    res = ieee802154_submac_config_phy(submac, conf);

    if (res >= 0) {
        submac->channel_num = conf->channel;
        submac->channel_page = conf->page;
        submac->tx_pow = conf->pow;
        if (conf->phy_mode != IEEE802154_PHY_NO_OP) {
            submac->phy_mode = conf->phy_mode;
        }
    }
    while (ieee802154_radio_confirm_set_idle(dev) == -EAGAIN) {}

    /* Go back to RX if needed */
    if (current_state == IEEE802154_FSM_STATE_RX) {
        res = ieee802154_radio_set_rx(dev);
        assert (res >= 0);
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
