/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "at86rf2xx.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#include "periph/spi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_AT86RF212B
/* See: Table 9-15. Recommended Mapping of TX Power, Frequency Band, and
 * PHY_TX_PWR (register 0x05), AT86RF212B data sheet. */
static const uint8_t dbm_to_tx_pow_868[] = {0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18,
                                            0x17, 0x15, 0x14, 0x13, 0x12, 0x11,
                                            0x10, 0x0f, 0x31, 0x30, 0x2f, 0x94,
                                            0x93, 0x91, 0x90, 0x29, 0x49, 0x48,
                                            0x47, 0xad, 0xcd, 0xcc, 0xcb, 0xea,
                                            0xe9, 0xe8, 0xe7, 0xe6, 0xe4, 0x80,
                                            0xa0};
static const uint8_t dbm_to_tx_pow_915[] = {0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x17,
                                            0x16, 0x15, 0x14, 0x13, 0x12, 0x11,
                                            0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b,
                                            0x09, 0x91, 0x08, 0x07, 0x05, 0x27,
                                            0x04, 0x03, 0x02, 0x01, 0x00, 0x86,
                                            0x40, 0x84, 0x83, 0x82, 0x80, 0xc1,
                                            0xc0};
static int16_t _tx_pow_to_dbm_212b(uint8_t channel, uint8_t page, uint8_t reg) {
    const uint8_t *dbm_to_tx_pow;
    size_t nelem;

    if (page == 0 || page == 2) {
        /* Channel 0 is 868.3 MHz */
        if (channel == 0) {
            dbm_to_tx_pow = &dbm_to_tx_pow_868[0];
            nelem = sizeof(dbm_to_tx_pow_868) / sizeof(dbm_to_tx_pow_868[0]);
        }
        else {
            /* Channels 1+ are 915 MHz */
            dbm_to_tx_pow = &dbm_to_tx_pow_915[0];
            nelem = sizeof(dbm_to_tx_pow_915) / sizeof(dbm_to_tx_pow_915[0]);
        }
    }
    else {
        return 0;
    }

    for(size_t i = 0; i < nelem; i++){
        if (dbm_to_tx_pow[i] == reg) {
            return i - 25;
        }
    }
    return 0;
}

#elif MODULE_AT86RF233
static const int16_t tx_pow_to_dbm[] = {4, 4, 3, 3, 2, 2, 1,
                                        0, -1, -2, -3, -4, -6, -8, -12, -17};
static const uint8_t dbm_to_tx_pow[] = {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                        0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
                                        0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
                                        0x06, 0x05, 0x03,0x00};
#else
static const int16_t tx_pow_to_dbm[] = {3, 3, 2, 2, 1, 1, 0,
                                        -1, -2, -3, -4, -5, -7, -9, -12, -17};
static const uint8_t dbm_to_tx_pow[] = {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                        0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b,
                                        0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                        0x05, 0x03, 0x00};
#endif

uint16_t at86rf2xx_get_addr_short(at86rf2xx_t *dev)
{
    return (dev->addr_short[0] << 8) | dev->addr_short[1];
}

void at86rf2xx_set_addr_short(at86rf2xx_t *dev, uint16_t addr)
{
    dev->addr_short[0] = addr >> 8;
    dev->addr_short[1] = addr;
#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->addr_short[1] &= 0x7F;
#endif
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_0,
                        dev->addr_short[0]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_1,
                        dev->addr_short[1]);
}

uint64_t at86rf2xx_get_addr_long(at86rf2xx_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);
    for (int i = 0; i < 8; i++) {
        ap[i] = dev->addr_long[7 - i];
    }
    return addr;
}

void at86rf2xx_set_addr_long(at86rf2xx_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++) {
        dev->addr_long[i] = (addr >> ((7 - i) * 8));
        at86rf2xx_reg_write(dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                            dev->addr_long[i]);
    }
}

uint8_t at86rf2xx_get_chan(at86rf2xx_t *dev)
{
    return dev->chan;
}

void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t channel)
{
    if ((channel < AT86RF2XX_MIN_CHANNEL) ||
        (channel > AT86RF2XX_MAX_CHANNEL) ||
        (dev->chan == channel)) {
        return;
    }

    dev->chan = channel;

    at86rf2xx_configure_phy(dev);
}

uint8_t at86rf2xx_get_page(at86rf2xx_t *dev)
{
#ifdef MODULE_AT86RF212B
    return dev->page;
#else
    return 0;
#endif
}

void at86rf2xx_set_page(at86rf2xx_t *dev, uint8_t page)
{
#ifdef MODULE_AT86RF212B
    if ((page != 0) && (page != 2)) {
        return;
    }
    dev->page = page;

    at86rf2xx_configure_phy(dev);
#endif
}

uint16_t at86rf2xx_get_pan(at86rf2xx_t *dev)
{
    return dev->pan;
}

void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan)
{
    dev->pan = pan;
    DEBUG("pan0: %u, pan1: %u\n", (uint8_t)pan, pan >> 8);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_0, (uint8_t)pan);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_1, (pan >> 8));
}

int16_t at86rf2xx_get_txpower(at86rf2xx_t *dev)
{
#ifdef MODULE_AT86RF212B
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR);
    DEBUG("txpower value: %x\n", txpower);
    return _tx_pow_to_dbm_212b(dev->chan, dev->page, txpower);
#else
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR)
                & AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
    return tx_pow_to_dbm[txpower];
#endif
}

void at86rf2xx_set_txpower(at86rf2xx_t *dev, int16_t txpower)
{
#ifdef MODULE_AT86RF212B
    txpower += 25;
#else
    txpower += 17;
#endif
    if (txpower < 0) {
        txpower = 0;
#ifdef MODULE_AT86RF212B
    }
    else if (txpower > 36) {
        txpower = 36;
#elif MODULE_AT86RF233
    }
    else if (txpower > 21) {
        txpower = 21;
#else
    }
    else if (txpower > 20) {
        txpower = 20;
#endif
    }
#ifdef MODULE_AT86RF212B
    if (dev->chan == 0) {
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                            dbm_to_tx_pow_868[txpower]);
    }
    else if (dev->chan < 11) {
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                            dbm_to_tx_pow_915[txpower]);
    }
#else
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                        dbm_to_tx_pow[txpower]);
#endif
}

uint8_t at86rf2xx_get_max_retries(at86rf2xx_t *dev)
{
    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0) >> 4);
}

void at86rf2xx_set_max_retries(at86rf2xx_t *dev, uint8_t max)
{
    max = (max > 7) ? 7 : max;
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_FRAME_RETRIES);
    tmp |= (max << 4);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

uint8_t at86rf2xx_get_csma_max_retries(at86rf2xx_t *dev)
{
    uint8_t tmp;
    tmp  = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES;
    tmp >>= 1;
    return tmp;
}

void at86rf2xx_set_csma_max_retries(at86rf2xx_t *dev, int8_t retries)
{
    retries = (retries > 5) ? 5 : retries; /* valid values: 0-5 */
    retries = (retries < 0) ? 7 : retries; /* max < 0 => disable CSMA (set to 7) */
    DEBUG("[at86rf2xx] opt: Set CSMA retries to %u\n", retries);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

void at86rf2xx_set_csma_backoff_exp(at86rf2xx_t *dev, uint8_t min, uint8_t max)
{
    max = (max > 8) ? 8 : max;
    min = (min > max) ? max : min;
    DEBUG("[at86rf2xx] opt: Set min BE=%u, max BE=%u\n", min, max);

    at86rf2xx_reg_write(dev,
            AT86RF2XX_REG__CSMA_BE,
            (max << 4) | (min));
}

void at86rf2xx_set_csma_seed(at86rf2xx_t *dev, uint8_t entropy[2])
{
    if(entropy == NULL) {
        DEBUG("[at86rf2xx] opt: CSMA seed entropy is nullpointer\n");
        return;
    }
    DEBUG("[at86rf2xx] opt: Set CSMA seed to 0x%x 0x%x\n", entropy[0], entropy[1]);

    at86rf2xx_reg_write(dev,
                           AT86RF2XX_REG__CSMA_SEED_0,
                           entropy[0]);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    tmp &= ~(AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1);
    tmp |= entropy[1] & AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

int8_t at86rf2xx_get_cca_threshold(at86rf2xx_t *dev)
{
    int8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CCA_THRES);
    tmp &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    tmp <<= 1;
    return (RSSI_BASE_VAL + tmp);
}

void at86rf2xx_set_cca_threshold(at86rf2xx_t *dev, int8_t value)
{
    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (value > 0) {
        value = -value;
    }
    /* transform the dBm value in the form
       that will fit in the AT86RF2XX_REG__CCA_THRES register */
    value -= RSSI_BASE_VAL;
    value >>= 1;
    value &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    value |= AT86RF2XX_CCA_THRES_MASK__RSVD_HI_NIBBLE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CCA_THRES, value);
}

void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->options |= option;
        /* trigger option specific actions */
        switch (option) {
            case AT86RF2XX_OPT_CSMA:
                DEBUG("[at86rf2xx] opt: enabling CSMA mode" \
                      "(4 retries, min BE: 3 max BE: 5)\n");
                /* Initialize CSMA seed with hardware address */
                at86rf2xx_set_csma_seed(dev, dev->addr_long);
                at86rf2xx_set_csma_max_retries(dev, 4);
                at86rf2xx_set_csma_backoff_exp(dev, 3, 5);
                break;
            case AT86RF2XX_OPT_PROMISCUOUS:
                DEBUG("[at86rf2xx] opt: enabling PROMISCUOUS mode\n");
                /* disable auto ACKs in promiscuous mode */
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
                tmp |= AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK;
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
                /* enable promiscuous mode */
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
                tmp |= AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE;
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
                break;
            case AT86RF2XX_OPT_AUTOACK:
                DEBUG("[at86rf2xx] opt: enabling auto ACKs\n");
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
                tmp &= ~(AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
                break;
            case AT86RF2XX_OPT_TELL_RX_START:
                DEBUG("[at86rf2xx] opt: enabling SFD IRQ\n");
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
                tmp |= AT86RF2XX_IRQ_STATUS_MASK__RX_START;
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->options &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case AT86RF2XX_OPT_CSMA:
                DEBUG("[at86rf2xx] opt: disabling CSMA mode\n");
                /* setting retries to -1 means CSMA disabled */
                at86rf2xx_set_csma_max_retries(dev, -1);
                break;
            case AT86RF2XX_OPT_PROMISCUOUS:
                DEBUG("[at86rf2xx] opt: disabling PROMISCUOUS mode\n");
                /* disable promiscuous mode */
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
                tmp &= ~(AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE);
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
                /* re-enable AUTOACK only if the option is set */
                if (dev->options & AT86RF2XX_OPT_AUTOACK) {
                    tmp = at86rf2xx_reg_read(dev,
                                             AT86RF2XX_REG__CSMA_SEED_1);
                    tmp &= ~(AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
                    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1,
                                        tmp);
                }
                break;
            case AT86RF2XX_OPT_AUTOACK:
                DEBUG("[at86rf2xx] opt: disabling auto ACKs\n");
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
                tmp |= AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK;
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
                break;
            case AT86RF2XX_OPT_TELL_RX_START:
                DEBUG("[at86rf2xx] opt: disabling SFD IRQ\n");
                tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
                tmp &= ~AT86RF2XX_IRQ_STATUS_MASK__RX_START;
                at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
}

static inline void _set_state(at86rf2xx_t *dev, uint8_t state)
{
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, state);
    while (at86rf2xx_get_status(dev) != state);
    dev->state = state;
}

void at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state)
{
    uint8_t old_state = at86rf2xx_get_status(dev);

    if (state == old_state) {
        return;
    }
    /* make sure there is no ongoing transmission, or state transition already
     * in progress */
    while (old_state == AT86RF2XX_STATE_BUSY_RX_AACK ||
           old_state == AT86RF2XX_STATE_BUSY_TX_ARET ||
           old_state == AT86RF2XX_STATE_IN_PROGRESS) {
        old_state = at86rf2xx_get_status(dev);
    }

    /* we need to go via PLL_ON if we are moving between RX_AACK_ON <-> TX_ARET_ON */
    if ((old_state == AT86RF2XX_STATE_RX_AACK_ON &&
             state == AT86RF2XX_STATE_TX_ARET_ON) ||
        (old_state == AT86RF2XX_STATE_TX_ARET_ON &&
             state == AT86RF2XX_STATE_RX_AACK_ON)) {
        _set_state(dev, AT86RF2XX_STATE_PLL_ON);
    }
    /* check if we need to wake up from sleep mode */
    else if (old_state == AT86RF2XX_STATE_SLEEP) {
        DEBUG("at86rf2xx: waking up from sleep mode\n");
        at86rf2xx_assert_awake(dev);
    }

    if (state == AT86RF2XX_STATE_SLEEP) {
        /* First go to TRX_OFF */
        at86rf2xx_force_trx_off(dev);
        /* Discard all IRQ flags, framebuffer is lost anyway */
        at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
        /* Go to SLEEP mode from TRX_OFF */
        gpio_set(dev->sleep_pin);
        dev->state = state;
    } else {
        _set_state(dev, state);
    }
}

void at86rf2xx_reset_state_machine(at86rf2xx_t *dev)
{
    uint8_t old_state;

    at86rf2xx_assert_awake(dev);

    /* Wait for any state transitions to complete before forcing TRX_OFF */
    do {
        old_state = at86rf2xx_get_status(dev);
    } while (old_state == AT86RF2XX_STATE_IN_PROGRESS);

    at86rf2xx_force_trx_off(dev);
}
