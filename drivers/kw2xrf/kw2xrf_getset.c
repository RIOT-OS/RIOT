/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file
 * @brief       get/set functionality of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include "log.h"
#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_getset.h"
#include "kw2xrf_intern.h"
#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define KW2XRF_LQI_HW_MAX           230      /**< LQI Saturation Level */

/* Modem_PA_PWR Register (PA Power Control) has a valid range from 3-31 */
#define KW2XRF_PA_RANGE_MAX      31       /**< Maximum value of PA Power Control Register */
#define KW2XRF_PA_RANGE_MIN      3        /**< Minimum value of PA Power Control Register */

#define KW2XRF_NUM_CHANNEL      (KW2XRF_MAX_CHANNEL - KW2XRF_MIN_CHANNEL + 1)

/* PLL integer and fractional lookup tables
 *
 * Fc = 2405 + 5(k - 11) , k = 11,12,...,26
 *
 * Equation for PLL frequency, MKW2xD Reference Manual, p.255 :
 * F = ((PLL_INT0 + 64) + (PLL_FRAC0/65536))32MHz
 *
 */
static const uint8_t pll_int_lt[16] = {
    11, 11, 11, 11,
    11, 11, 12, 12,
    12, 12, 12, 12,
    13, 13, 13, 13
};

static const uint16_t pll_frac_lt[16] = {
    10240, 20480, 30720, 40960,
    51200, 61440, 6144, 16384,
    26624, 36864, 47104, 57344,
    2048, 12288, 22528, 32768
};

static const uint8_t pow_lt[44] = {
    3, 4, 4, 5,
    6, 6, 7, 7,
    8, 9, 9, 10,
    11, 11, 12, 13,
    13, 14, 14, 15,
    16, 16, 17, 18,
    18, 19, 20, 20,
    21, 21, 22, 23,
    23, 24, 25, 25,
    26, 27, 27, 28,
    28, 29, 30, 31
};

void kw2xrf_set_tx_power(kw2xrf_t *dev, int16_t txpower)
{
    if (txpower > KW2XDRF_OUTPUT_POWER_MAX) {
        txpower = KW2XDRF_OUTPUT_POWER_MAX;
    }

    if (txpower < KW2XDRF_OUTPUT_POWER_MIN) {
        txpower = KW2XDRF_OUTPUT_POWER_MIN;
    }

    uint8_t level = pow_lt[txpower - KW2XDRF_OUTPUT_POWER_MIN];
    kw2xrf_write_dreg(dev, MKW2XDM_PA_PWR, MKW2XDM_PA_PWR(level));
    LOG_DEBUG("[kw2xrf] set txpower to: %d\n", txpower);
    dev->tx_power = txpower;
}

uint16_t kw2xrf_get_txpower(kw2xrf_t *dev)
{
    return dev->tx_power;
}

uint8_t kw2xrf_get_channel(kw2xrf_t *dev)
{
    uint8_t pll_int = kw2xrf_read_dreg(dev, MKW2XDM_PLL_INT0);
    uint16_t pll_frac = kw2xrf_read_dreg(dev, MKW2XDM_PLL_FRAC0_LSB);
    pll_frac |= ((uint16_t)kw2xrf_read_dreg(dev, MKW2XDM_PLL_FRAC0_MSB) << 8);

    for (unsigned i = 0; i < KW2XRF_NUM_CHANNEL; i++) {
        if ((pll_frac_lt[i] == pll_frac) && (pll_int_lt[i] == pll_int)) {
            return i + 11;
        }
    }
    return 0;
}

static int kw2xrf_get_sequence(kw2xrf_t *dev)
{
    int reg = 0;
    reg = kw2xrf_read_dreg(dev, MKW2XDM_PHY_CTRL1);
    reg &= MKW2XDM_PHY_CTRL1_XCVSEQ_MASK;
    return reg;
}

int kw2xrf_set_channel(kw2xrf_t *dev, uint8_t channel)
{
    /* Save old sequence to restore this state later */
    uint8_t old_seq = kw2xrf_get_sequence(dev);

    if (channel < KW2XRF_MIN_CHANNEL || channel > KW2XRF_MAX_CHANNEL) {
        LOG_ERROR("[kw2xrf] Invalid channel %u\n", channel);
        return -1;
    }

    if (old_seq) {
        kw2xrf_abort_sequence(dev);
    }

    uint8_t tmp = channel - 11;
    kw2xrf_write_dreg(dev, MKW2XDM_PLL_INT0, MKW2XDM_PLL_INT0_VAL(pll_int_lt[tmp]));
    kw2xrf_write_dreg(dev, MKW2XDM_PLL_FRAC0_LSB, (uint8_t)pll_frac_lt[tmp]);
    kw2xrf_write_dreg(dev, MKW2XDM_PLL_FRAC0_MSB, (uint8_t)(pll_frac_lt[tmp] >> 8));

    if (old_seq) {
        kw2xrf_set_sequence(dev, old_seq);
    }

    LOG_DEBUG("[kw2xrf] set channel to %u\n", channel);
    return 0;
}

void kw2xrf_abort_sequence(kw2xrf_t *dev)
{
    uint8_t regs[MKW2XDM_PHY_CTRL4 + 1];

    kw2xrf_mask_irq_b(dev);
    kw2xrf_read_dregs(dev, MKW2XDM_IRQSTS1, regs, (MKW2XDM_PHY_CTRL4 + 1));

    if ((regs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_XCVSEQ_MASK) != XCVSEQ_IDLE) {
        /* abort any ongoing sequence */
        regs[MKW2XDM_PHY_CTRL1] &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
        kw2xrf_write_dreg(dev, MKW2XDM_PHY_CTRL1, regs[MKW2XDM_PHY_CTRL1]);
    }

    uint8_t state;
    do {
        state = kw2xrf_read_dreg(dev, MKW2XDM_SEQ_STATE);
        DEBUG("[kw2xrf] abort SEQ_STATE: %x\n", state);
    } while ((state & 0x1F) != 0);

    /* clear all IRQ bits */
    regs[MKW2XDM_IRQSTS1] = 0x7f;
    regs[MKW2XDM_IRQSTS2] = 0x03;
    regs[MKW2XDM_IRQSTS3] |= 0x0f;
    kw2xrf_write_dregs(dev, MKW2XDM_IRQSTS1, regs, 3);

    kw2xrf_enable_irq_b(dev);
}

/*
 * Simplified version for irq handling where the state of
 * the sequenz manager is known.
 */
void kw2xrf_set_idle_sequence(kw2xrf_t *dev)
{
    kw2xrf_mask_irq_b(dev);
    uint8_t reg = kw2xrf_read_dreg(dev, MKW2XDM_PHY_CTRL1);

    /* reset sequenz manager */
    reg &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
    kw2xrf_write_dreg(dev, MKW2XDM_PHY_CTRL1, reg);

    if (dev->pending_tx) {
        DEBUG("[kw2xrf] pending tx, cannot set idle sequenz\n");
        return;
    }

    /* start new sequenz */
    reg |= MKW2XDM_PHY_CTRL1_XCVSEQ(dev->idle_state);
    kw2xrf_write_dreg(dev, MKW2XDM_PHY_CTRL1, reg);

    switch (dev->idle_state) {
        case XCVSEQ_IDLE:
        /* for inexplicable reasons, the receive mode is also idle mode */
        case XCVSEQ_RECEIVE:
            dev->state = NETOPT_STATE_IDLE;
            break;

        case XCVSEQ_CONTINUOUS_CCA:
        case XCVSEQ_CCA:
            dev->state = NETOPT_STATE_RX;
            break;

        case XCVSEQ_TRANSMIT:
        case XCVSEQ_TX_RX:
            dev->state = NETOPT_STATE_TX;
            break;

        default:
            dev->state = NETOPT_STATE_IDLE;
    }
    kw2xrf_enable_irq_b(dev);
}

void kw2xrf_set_sequence(kw2xrf_t *dev, kw2xrf_physeq_t seq)
{
    uint8_t reg = 0;

    kw2xrf_abort_sequence(dev);

    switch (seq) {
        case XCVSEQ_IDLE:
        /* for inexplicable reasons, the receive mode is also idle mode */
        case XCVSEQ_RECEIVE:
            dev->state = NETOPT_STATE_IDLE;
            break;

        case XCVSEQ_CONTINUOUS_CCA:
        case XCVSEQ_CCA:
            dev->state = NETOPT_STATE_RX;
            break;

        case XCVSEQ_TRANSMIT:
        case XCVSEQ_TX_RX:
            dev->state = NETOPT_STATE_TX;
            break;

        default:
            DEBUG("[kw2xrf] undefined state assigned to phy\n");
            dev->state = NETOPT_STATE_IDLE;
    }

    DEBUG("[kw2xrf] set sequence to %i\n", seq);
    reg = kw2xrf_read_dreg(dev, MKW2XDM_PHY_CTRL1);
    reg &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
    reg |= MKW2XDM_PHY_CTRL1_XCVSEQ(seq);
    kw2xrf_write_dreg(dev, MKW2XDM_PHY_CTRL1, reg);
}

void kw2xrf_set_pan(kw2xrf_t *dev, uint16_t pan)
{
    uint8_t val_ar[2];
    val_ar[1] = (pan >> 8);
    val_ar[0] = (uint8_t)pan;
    kw2xrf_write_iregs(dev, MKW2XDMI_MACPANID0_LSB, val_ar, 2);
    LOG_DEBUG("[kw2xrf] set pan to: 0x%x\n", pan);
}

void kw2xrf_set_addr_short(kw2xrf_t *dev, uint16_t addr)
{
    uint8_t val_ar[2];
    val_ar[0] = (addr >> 8);
    val_ar[1] = (uint8_t)addr;
#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    val_ar[0] &= 0x7F;
#endif
    kw2xrf_write_iregs(dev, MKW2XDMI_MACSHORTADDRS0_LSB, val_ar,
                       IEEE802154_SHORT_ADDRESS_LEN);
}

void kw2xrf_set_addr_long(kw2xrf_t *dev, uint64_t addr)
{
    uint64_t tmp;
    uint8_t *ap = (uint8_t *)(&tmp);

    for (unsigned i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        ap[i] = (addr >> ((IEEE802154_LONG_ADDRESS_LEN - 1 - i) * 8));
    }

    kw2xrf_write_iregs(dev, MKW2XDMI_MACLONGADDRS0_0, ap,
                       IEEE802154_LONG_ADDRESS_LEN);
}

uint16_t kw2xrf_get_addr_short(kw2xrf_t *dev)
{
    uint16_t addr;
    uint8_t *ap = (uint8_t *)(&addr);
    kw2xrf_read_iregs(dev, MKW2XDMI_MACSHORTADDRS0_LSB, ap,
                      IEEE802154_SHORT_ADDRESS_LEN);
    return byteorder_swaps(addr);
}

uint64_t kw2xrf_get_addr_long(kw2xrf_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);

    kw2xrf_read_iregs(dev, MKW2XDMI_MACLONGADDRS0_0, ap,
                      IEEE802154_LONG_ADDRESS_LEN);

    /* Address is always read as little endian and API specifies big endian */
    return byteorder_swapll(addr);
}

int8_t kw2xrf_get_cca_threshold(kw2xrf_t *dev)
{
    uint8_t tmp;
    kw2xrf_read_iregs(dev, MKW2XDMI_CCA1_THRESH, &tmp, 1);
    /* KW2x register value represents absolute value in dBm
     * default value: -75 dBm
     */
    return (-tmp);
}

void kw2xrf_set_cca_threshold(kw2xrf_t *dev, int8_t value)
{
    /* normalize to absolute value */
    if (value < 0) {
        value = -value;
    }
    kw2xrf_write_iregs(dev, MKW2XDMI_CCA1_THRESH, (uint8_t*)&value, 1);
}

void kw2xrf_set_cca_mode(kw2xrf_t *dev, uint8_t mode)
{
    uint8_t tmp;
    tmp = kw2xrf_read_dreg(dev, MKW2XDM_PHY_CTRL4);
    tmp &= ~MKW2XDM_PHY_CTRL4_CCATYPE_MASK;
    tmp |= MKW2XDM_PHY_CTRL4_CCATYPE(mode);
    kw2xrf_write_dreg(dev, MKW2XDM_PHY_CTRL4, tmp);
}

uint8_t kw2xrf_get_cca_mode(kw2xrf_t *dev)
{
    uint8_t tmp;
    tmp = kw2xrf_read_dreg(dev, MKW2XDM_PHY_CTRL4);
    return (tmp & MKW2XDM_PHY_CTRL4_CCATYPE_MASK) >> MKW2XDM_PHY_CTRL4_CCATYPE_SHIFT;
}

int16_t kw2xrf_get_rssi(uint32_t value)
{
    /* Get rssi (Received Signal Strength Indicator, unit is dBm)
     * from lqi (Link Quality Indicator) value.
     * There are two different equations for RSSI:
     * RF = (LQI - 286.6) / 2.69333 (MKW2xD Reference Manual)
     * RF = (LQI - 295.4) / 2.84 (MCR20A Reference Manual)
     * The last appears more to match the graphic (Figure 3-10).
     * Since RSSI value is always positive and we want to
     * avoid the floating point computation:
     * -RF * 65536 = (LQI / 2.84 - 295.4 / 2.84) * 65536
     * RF * 65536 = (295.4 * 65536 / 2.84) - (LQI * 65536 / 2.84)
     */
    uint32_t a = (uint32_t)(295.4 * 65536 / 2.84);
    uint32_t b = (uint32_t)(65536 / 2.84);
    return ((b * value) - a) >> 16;
}

void kw2xrf_set_option(kw2xrf_t *dev, uint16_t option, bool state)
{
    DEBUG("[kw2xrf] set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->netdev.flags |= option;

        /* trigger option specific actions */
        switch (option) {
            case KW2XRF_OPT_AUTOCCA:
                LOG_DEBUG("[kw2xrf] opt: enabling CCA before TX mode\n");
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_CCABFRTX);
                break;

            case KW2XRF_OPT_PROMISCUOUS:
                LOG_DEBUG("[kw2xrf] opt: enabling PROMISCUOUS mode\n");
                /* disable auto ACKs in promiscuous mode */
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_AUTOACK | MKW2XDM_PHY_CTRL1_RXACKRQD);
                /* enable promiscuous mode */
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL4,
                    MKW2XDM_PHY_CTRL4_PROMISCUOUS);
                break;

            case KW2XRF_OPT_AUTOACK:
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_AUTOACK);
                break;

            case KW2XRF_OPT_ACK_REQ:
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_RXACKRQD);
                break;

            case KW2XRF_OPT_TELL_RX_START:
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2,
                    MKW2XDM_PHY_CTRL2_RX_WMRK_MSK);
                break;

            case KW2XRF_OPT_TELL_RX_END:
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2,
                    MKW2XDM_PHY_CTRL2_RXMSK);
                break;

            case KW2XRF_OPT_TELL_TX_END:
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2,
                    MKW2XDM_PHY_CTRL2_TXMSK);
                break;

            case KW2XRF_OPT_TELL_TX_START:
            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->netdev.flags &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case KW2XRF_OPT_AUTOCCA:
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_CCABFRTX);
                break;

            case KW2XRF_OPT_PROMISCUOUS:
                /* disable promiscuous mode */
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL4,
                    MKW2XDM_PHY_CTRL4_PROMISCUOUS);
                /* re-enable AUTOACK only if the option is set */
                if (dev->netdev.flags & KW2XRF_OPT_AUTOACK) {
                    kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                        MKW2XDM_PHY_CTRL1_AUTOACK);
                }
                if (dev->netdev.flags & KW2XRF_OPT_ACK_REQ) {
                    kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                        MKW2XDM_PHY_CTRL1_RXACKRQD);
                }
                break;

            case KW2XRF_OPT_AUTOACK:
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_AUTOACK);
                break;

            case KW2XRF_OPT_ACK_REQ:
                kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL1,
                    MKW2XDM_PHY_CTRL1_RXACKRQD);
                break;

            case KW2XRF_OPT_TELL_RX_START:
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL2,
                    MKW2XDM_PHY_CTRL2_RX_WMRK_MSK);
                break;

            case KW2XRF_OPT_TELL_RX_END:
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL2,
                    MKW2XDM_PHY_CTRL2_RXMSK);
                break;

            case KW2XRF_OPT_TELL_TX_END:
                kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL2,
                    MKW2XDM_PHY_CTRL2_TXMSK);
                break;


            case KW2XRF_OPT_TELL_TX_START:
            default:
                /* do nothing */
                break;
        }
    }
}

netopt_state_t kw2xrf_get_status(kw2xrf_t *dev)
{
    uint8_t reg = kw2xrf_read_dreg(dev, MKW2XDM_PHY_CTRL1);

    switch (reg & MKW2XDM_PHY_CTRL1_XCVSEQ_MASK) {
        case XCVSEQ_RECEIVE:
            return NETOPT_STATE_RX;

        case XCVSEQ_TRANSMIT:
            return NETOPT_STATE_TX;

        case XCVSEQ_CCA:
            return NETOPT_STATE_RX;

        case XCVSEQ_TX_RX:
            return NETOPT_STATE_TX;

        case XCVSEQ_CONTINUOUS_CCA:
            return NETOPT_STATE_RX;

        case XCVSEQ_IDLE:
            return NETOPT_STATE_IDLE;
        default:
            break;
    }
    return NETOPT_STATE_IDLE;
}

int kw2xrf_cca(kw2xrf_t *dev)
{
    /* TODO: add Standalone CCA here */
    kw2xrf_seq_timeout_on(dev, 0x3ffff);
    kw2xrf_set_sequence(dev, XCVSEQ_CONTINUOUS_CCA);
    return 0;
}

void kw2xrf_set_rx_watermark(kw2xrf_t *dev, uint8_t value)
{
    kw2xrf_write_iregs(dev, MKW2XDMI_RX_WTR_MARK, &value, 1);
}
