/*
 * SPDX-FileCopyrightText: 2016 MUTEX NZ Ltd.
 * SPDX-FileCopyrightText: 2015 Loci Controls Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the cc2538_rf driver
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include "cc2538_rf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* static const __flash uint8_t? */
static const uint8_t power_lut[NUM_POWER_LEVELS] = {
    0,   /**< -24 dBm */
    7,   /**< -23 dBm */
    15,  /**< -22 dBm */
    22,  /**< -21 dBm */
    29,  /**< -20 dBm */
    37,  /**< -19 dBm */
    44,  /**< -18 dBm */
    51,  /**< -17 dBm */
    59,  /**< -16 dBm */
    66,  /**< -15 dBm */
    77,  /**< -14 dBm */
    88,  /**< -13 dBm */
    93,  /**< -12 dBm */
    98,  /**< -11 dBm */
    106, /**< -10 dBm */
    114, /**<  -9 dBm */
    125, /**<  -8 dBm */
    136, /**<  -7 dBm */
    141, /**<  -6 dBm */
    145, /**<  -5 dBm */
    153, /**<  -4 dBm */
    161, /**<  -3 dBm */
    169, /**<  -2 dBm */
    176, /**<  -1 dBm */
    182, /**<   0 dBm */
    197, /**<   1 dBm */
    205, /**<   2 dBm */
    213, /**<   3 dBm */
    225, /**<   4 dBm */
    237, /**<   5 dBm */
    246, /**<   6 dBm */
    255, /**<   7 dBm */
};

void cc2538_get_addr_long(uint8_t *addr)
{
    addr[7] = RFCORE_FFSM_EXT_ADDR0;
    addr[6] = RFCORE_FFSM_EXT_ADDR1;
    addr[5] = RFCORE_FFSM_EXT_ADDR2;
    addr[4] = RFCORE_FFSM_EXT_ADDR3;
    addr[3] = RFCORE_FFSM_EXT_ADDR4;
    addr[2] = RFCORE_FFSM_EXT_ADDR5;
    addr[1] = RFCORE_FFSM_EXT_ADDR6;
    addr[0] = RFCORE_FFSM_EXT_ADDR7;
}

void cc2538_get_addr_short(uint8_t *addr)
{
    addr[1] = RFCORE_FFSM_SHORT_ADDR0;
    addr[0] = RFCORE_FFSM_SHORT_ADDR1;
}

unsigned int cc2538_get_chan(void)
{
    return IEEE802154_FREQ2CHAN(CC2538_MIN_FREQ + RFCORE_XREG_FREQCTRL);
}

bool cc2538_get_monitor(void)
{
    return NOT(RFCORE->XREG_FRMFILT0bits.FRAME_FILTER_EN);
}

uint16_t cc2538_get_pan(void)
{
    return (RFCORE_FFSM_PAN_ID1 << 8) | RFCORE_FFSM_PAN_ID0;
}

int cc2538_get_tx_power(void)
{
    int index;
    int best_index = 0;
    int best_delta = INT_MAX;
    int txpower;

    txpower = RFCORE_XREG_TXPOWER & 0xff;

    for (index = 0; index < NUM_POWER_LEVELS; index++) {
        int delta = ABS_DIFF(power_lut[index], txpower);

        if (delta < best_delta) {
            best_delta = delta;
            best_index = index;
        }
    }

    return OUTPUT_POWER_MIN + best_index;
}

void cc2538_set_addr_long(const uint8_t *addr)
{
    RFCORE_FFSM_EXT_ADDR0 = addr[7];
    RFCORE_FFSM_EXT_ADDR1 = addr[6];
    RFCORE_FFSM_EXT_ADDR2 = addr[5];
    RFCORE_FFSM_EXT_ADDR3 = addr[4];
    RFCORE_FFSM_EXT_ADDR4 = addr[3];
    RFCORE_FFSM_EXT_ADDR5 = addr[2];
    RFCORE_FFSM_EXT_ADDR6 = addr[1];
    RFCORE_FFSM_EXT_ADDR7 = addr[0];
}

void cc2538_set_addr_short(const uint8_t *addr)
{
    RFCORE_FFSM_SHORT_ADDR0 = addr[1];
    RFCORE_FFSM_SHORT_ADDR1 = addr[0];
}

void cc2538_set_chan(unsigned int chan)
{
    DEBUG("%s(%u): Setting channel to ", __FUNCTION__, chan);

    if (chan < IEEE802154_CHANNEL_MIN) {
        chan = IEEE802154_CHANNEL_MIN;
    }
    else if (chan > IEEE802154_CHANNEL_MAX) {
        chan = IEEE802154_CHANNEL_MAX;
    }

    DEBUG("%i (range %i-%i)\n", chan, IEEE802154_CHANNEL_MIN,
                                      IEEE802154_CHANNEL_MAX);

    cc2538_set_freq(IEEE802154_CHAN2FREQ(chan));
}

void cc2538_set_freq(unsigned int MHz)
{
    DEBUG("%s(%u): Setting frequency to ", __FUNCTION__, MHz);

    if (MHz < IEEE802154_MIN_FREQ) {
        MHz = IEEE802154_MIN_FREQ;
    }
    else if (MHz > IEEE802154_MAX_FREQ) {
        MHz = IEEE802154_MAX_FREQ;
    }

    DEBUG("%i (range %i-%i)\n", MHz, IEEE802154_MIN_FREQ, IEEE802154_MAX_FREQ);
    RFCORE_XREG_FREQCTRL = MHz - CC2538_MIN_FREQ;
}

void cc2538_set_monitor(bool mode)
{
    RFCORE->XREG_FRMFILT0bits.FRAME_FILTER_EN = NOT(mode);
}

void cc2538_set_state(cc2538_rf_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_OFF:
        case NETOPT_STATE_SLEEP:
            cc2538_off();
            dev->state = state;
            break;

        case NETOPT_STATE_RX:
        case NETOPT_STATE_IDLE:
            if (!cc2538_is_on()) {
                cc2538_on();
                RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE > FSM_STATE_RX_CALIBRATION);
            }
            dev->state = state;
            break;

        case NETOPT_STATE_TX:
            dev->state = NETOPT_STATE_IDLE;
            break;

        case NETOPT_STATE_RESET:
            cc2538_off();
            cc2538_on();
            RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE > FSM_STATE_RX_CALIBRATION);
            dev->state = NETOPT_STATE_IDLE;
            break;
        default:
            break;
    }
}

void cc2538_set_pan(uint16_t pan)
{
    RFCORE_FFSM_PAN_ID0 = pan;
    RFCORE_FFSM_PAN_ID1 = pan >> 8;
}

void cc2538_set_tx_power(int dBm)
{
    DEBUG("%s(%i): Setting TX power to ", __FUNCTION__, dBm);

    if (dBm < OUTPUT_POWER_MIN) {
        dBm = OUTPUT_POWER_MIN;
    }
    else if (dBm > OUTPUT_POWER_MAX) {
        dBm = OUTPUT_POWER_MAX;
    }

    DEBUG("%idBm (range %i-%i dBm)\n", dBm, OUTPUT_POWER_MIN, OUTPUT_POWER_MAX);
    RFCORE_XREG_TXPOWER = power_lut[dBm - OUTPUT_POWER_MIN];
}
