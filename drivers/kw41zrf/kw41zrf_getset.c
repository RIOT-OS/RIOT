/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 * @file
 * @brief       get/set functionality of kw41zrf driver
 *
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include <errno.h>
#include "log.h"
#include "cpu.h"
#include "byteorder.h"
#include "kw41zrf.h"
#include "kw41zrf_intern.h"
#include "kw41zrf_getset.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define KW41ZRF_NUM_CHANNEL      (KW41ZRF_MAX_CHANNEL - KW41ZRF_MIN_CHANNEL + 1)

/* Lookup table for PA_PWR register */
/* Source: KW41Z data sheet, 5.3 Transmit and PLL Feature Summary,
 * Table 8. Transmit Output Power as a function of PA_POWER[5:0] */
static const uint8_t tx_power_dbm_to_pa_pwr[29] = {
    4, 4, 4, 4,        /* -19:-16 dBm */
    6, 6,              /* -15:-14 dBm */
    8, 8,              /* -13:-12 dBm */
    10, 10,            /* -11:-10 dBm */
    12,                /* -9 dBm */
    14,                /* -8 dBm */
    16,                /* -7 dBm */
    18,                /* -6 dBm */
    20,                /* -5 dBm */
    22,                /* -4 dBm */
    26,                /* -3 dBm */
    28,                /* -2 dBm */
    34,                /* -1 dBm */
    38,                /* 0 dBm */
    42,                /* 1 dBm */
    48,                /* 2 dBm */
    56,                /* 3 dBm */
    62,                /* 4 dBm */
};

void kw41zrf_set_tx_power(kw41zrf_t *dev, int16_t txpower_dbm)
{
    if (txpower_dbm < KW41ZRF_OUTPUT_POWER_MIN) {
        ZLL->PA_PWR = 1;
    }
    else if (txpower_dbm > KW41ZRF_OUTPUT_POWER_MAX) {
        ZLL->PA_PWR = 62;
    }
    else {
        ZLL->PA_PWR = tx_power_dbm_to_pa_pwr[txpower_dbm - KW41ZRF_OUTPUT_POWER_MIN];
    }

    LOG_DEBUG("[kw41zrf] set txpower to: %d\n", txpower_dbm);
    dev->tx_power = txpower_dbm;
}

uint16_t kw41zrf_get_txpower(kw41zrf_t *dev)
{
    return dev->tx_power;
}

uint8_t kw41zrf_get_channel(kw41zrf_t *dev)
{
    return (ZLL->CHANNEL_NUM0 & ZLL_CHANNEL_NUM0_CHANNEL_NUM0_MASK);
}

int kw41zrf_set_channel(kw41zrf_t *dev, uint8_t channel)
{
    if (channel < KW41ZRF_MIN_CHANNEL || channel > KW41ZRF_MAX_CHANNEL) {
        LOG_ERROR("[kw41zrf] Invalid channel %u\n", channel);
        return -EINVAL;
    }

    ZLL->CHANNEL_NUM0 = channel;
    dev->netdev.chan = channel;

    LOG_DEBUG("[kw41zrf] set channel to %u\n", channel);
    return 0;
}

void kw41zrf_set_pan(kw41zrf_t *dev, uint16_t pan)
{
    dev->netdev.pan = pan;

    ZLL->MACSHORTADDRS0 = (ZLL->MACSHORTADDRS0 & ~ZLL_MACSHORTADDRS0_MACPANID0_MASK) |
        ZLL_MACSHORTADDRS0_MACPANID0(pan);

    LOG_DEBUG("[kw41zrf] set pan to: 0x%x\n", pan);
    dev->netdev.pan = pan;
}

void kw41zrf_set_addr_short(kw41zrf_t *dev, uint16_t addr)
{
#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    addr &= 0x7fff;
#endif
    /* Network byte order */
    /* TODO use byteorder.h */
    dev->netdev.short_addr[0] = (addr & 0xff);
    dev->netdev.short_addr[1] = (addr >> 8);
    ZLL->MACSHORTADDRS0 = (ZLL->MACSHORTADDRS0 & ~ZLL_MACSHORTADDRS0_MACSHORTADDRS0_MASK) |
        ZLL_MACSHORTADDRS0_MACSHORTADDRS0(addr);
}

void kw41zrf_set_addr_long(kw41zrf_t *dev, uint64_t addr)
{
    (void) dev;
    for (unsigned i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        dev->netdev.long_addr[i] = (uint8_t)(addr >> (i * 8));
    }
    /* Network byte order */
    addr = byteorder_swapll(addr);
    ZLL->MACLONGADDRS0_LSB = (uint32_t)addr;
    ZLL->MACLONGADDRS0_MSB = (addr >> 32);
}

uint16_t kw41zrf_get_addr_short(kw41zrf_t *dev)
{
    (void) dev;
    return (ZLL->MACSHORTADDRS0 & ZLL_MACSHORTADDRS0_MACSHORTADDRS0_MASK) >>
        ZLL_MACSHORTADDRS0_MACSHORTADDRS0_SHIFT;
}

uint64_t kw41zrf_get_addr_long(kw41zrf_t *dev)
{
    (void) dev;
    uint64_t addr = ((uint64_t)ZLL->MACLONGADDRS0_MSB << 32) | ZLL->MACLONGADDRS0_LSB;
    /* Network byte order */
    addr = byteorder_swapll(addr);

    return addr;
}

int8_t kw41zrf_get_cca_threshold(kw41zrf_t *dev)
{
    (void) dev;
    uint8_t tmp = (ZLL->CCA_LQI_CTRL & ZLL_CCA_LQI_CTRL_CCA1_THRESH_MASK) >>
        ZLL_CCA_LQI_CTRL_CCA1_THRESH_SHIFT;
    return (int8_t)tmp;
}

void kw41zrf_set_cca_threshold(kw41zrf_t *dev, int8_t value)
{
    (void) dev;
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_CCA1_THRESH_MASK) |
        ZLL_CCA_LQI_CTRL_CCA1_THRESH(value);
}

void kw41zrf_set_cca_mode(kw41zrf_t *dev, uint8_t mode)
{
    (void) dev;
    ZLL->PHY_CTRL = (ZLL->PHY_CTRL & ~ZLL_PHY_CTRL_CCATYPE_MASK) |
        ZLL_PHY_CTRL_CCATYPE(mode);
}

uint8_t kw41zrf_get_cca_mode(kw41zrf_t *dev)
{
    (void) dev;
    return (ZLL->PHY_CTRL & ZLL_PHY_CTRL_CCATYPE_MASK) >> ZLL_PHY_CTRL_CCATYPE_SHIFT;
}

int8_t kw41zrf_get_ed_level(kw41zrf_t *dev)
{
    (void) dev;
    return (ZLL->LQI_AND_RSSI & ZLL_LQI_AND_RSSI_CCA1_ED_FNL_MASK) >> ZLL_LQI_AND_RSSI_CCA1_ED_FNL_SHIFT;
}

void kw41zrf_set_option(kw41zrf_t *dev, uint16_t option, bool state)
{
    DEBUG("[kw41zrf] set option 0x%04x to %x\n", option, state);

    /* set option field */
    if (state) {
        dev->netdev.flags |= option;

        /* trigger option specific actions */
        switch (option) {
            case KW41ZRF_OPT_CSMA:
                LOG_DEBUG("[kw41zrf] enable: CSMA\n");
                bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CCABFRTX_SHIFT);
                break;

            case KW41ZRF_OPT_PROMISCUOUS:
                LOG_DEBUG("[kw41zrf] enable: PROMISCUOUS\n");
                /* enable promiscuous mode */
                bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_PROMISCUOUS_SHIFT);
                /* auto ACK is always disabled in promiscuous mode by the hardware */
                break;

            case KW41ZRF_OPT_AUTOACK:
                LOG_DEBUG("[kw41zrf] enable: AUTOACK\n");
                bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_AUTOACK_SHIFT);
                break;

            case KW41ZRF_OPT_ACK_REQ:
                LOG_DEBUG("[kw41zrf] enable: ACK_REQ\n");
                bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RXACKRQD_SHIFT);
                break;

            case KW41ZRF_OPT_TELL_RX_START:
                LOG_DEBUG("[kw41zrf] enable: TELL_RX_START\n");
                bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RX_WMRK_MSK_SHIFT);
                break;

            case KW41ZRF_OPT_TELL_RX_END:
                LOG_DEBUG("[kw41zrf] enable: TELL_RX_END\n");
                break;

            case KW41ZRF_OPT_TELL_TX_END:
                LOG_DEBUG("[kw41zrf] enable: TELL_TX_END\n");
                break;

            case KW41ZRF_OPT_TELL_TX_START:
                LOG_DEBUG("[kw41zrf] enable: TELL_TX_START (ignored)\n");
            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->netdev.flags &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case KW41ZRF_OPT_CSMA:
                LOG_DEBUG("[kw41zrf] disable: CSMA\n");
                bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_CCABFRTX_SHIFT);
                break;

            case KW41ZRF_OPT_PROMISCUOUS:
                LOG_DEBUG("[kw41zrf] disable: PROMISCUOUS\n");
                /* disable promiscuous mode */
                bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_PROMISCUOUS_SHIFT);
                break;

            case KW41ZRF_OPT_AUTOACK:
                LOG_DEBUG("[kw41zrf] disable: AUTOACK\n");
                bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_AUTOACK_SHIFT);
                break;

            case KW41ZRF_OPT_ACK_REQ:
                LOG_DEBUG("[kw41zrf] disable: ACK_REQ\n");
                bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RXACKRQD_SHIFT);
                break;

            case KW41ZRF_OPT_TELL_RX_START:
                LOG_DEBUG("[kw41zrf] disable: TELL_RX_START\n");
                bit_set32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RX_WMRK_MSK_SHIFT);
                break;

            case KW41ZRF_OPT_TELL_RX_END:
                LOG_DEBUG("[kw41zrf] disable: TELL_RX_END\n");
                break;

            case KW41ZRF_OPT_TELL_TX_END:
                LOG_DEBUG("[kw41zrf] disable: TELL_TX_END\n");
                break;

            case KW41ZRF_OPT_TELL_TX_START:
                LOG_DEBUG("[kw41zrf] disable: TELL_TX_START (ignored)\n");
            default:
                /* do nothing */
                break;
        }
    }
}

void kw41zrf_set_rx_watermark(kw41zrf_t *dev, uint8_t value)
{
    ZLL->RX_WTR_MARK = ZLL_RX_WTR_MARK_RX_WTR_MARK(value);
}
