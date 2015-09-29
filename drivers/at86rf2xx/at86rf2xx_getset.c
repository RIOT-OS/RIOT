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
int16_t tx_pow_to_dbm(at86rf2xx_freq_t freq, uint8_t reg) {
    for(int i = 0; i < 37; i++){
        if(freq == AT86RF2XX_FREQ_868MHZ){
            if (dbm_to_tx_pow_868[i] == reg) {
                return i -25;
            }
        } else if (freq == AT86RF2XX_FREQ_915MHZ){
            if (dbm_to_tx_pow_915[i] == reg) {
                return i -25;
            }
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
    dev->addr_short[1] = addr & 0xff;
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
    uint8_t tmp;

    if (channel < AT86RF2XX_MIN_CHANNEL
        || channel > AT86RF2XX_MAX_CHANNEL) {
        return;
    }
    dev->chan = channel;
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= ~(AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    tmp |= (channel & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, tmp);
}

#ifdef MODULE_AT86RF212B
at86rf2xx_freq_t at86rf2xx_get_freq(at86rf2xx_t *dev)
{
    return dev->freq;
}

void at86rf2xx_set_freq(at86rf2xx_t *dev, at86rf2xx_freq_t freq)
{
    uint8_t trx_ctrl2 = 0, rf_ctrl0 = 0;
    trx_ctrl2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
    trx_ctrl2 &= ~(AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE);
    rf_ctrl0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RF_CTRL_0);
    /* Erase previous conf for GC_TX_OFFS */
    rf_ctrl0 &= ~AT86RF2XX_RF_CTRL_0_MASK__GC_TX_OFFS;

    trx_ctrl2 |= AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE;
    rf_ctrl0 |= AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__2DB;

    switch(freq) {
        case AT86RF2XX_FREQ_915MHZ:
            if (dev->chan == 0) {
                at86rf2xx_set_chan(dev,AT86RF2XX_DEFAULT_CHANNEL);
            } else {
                at86rf2xx_set_chan(dev,dev->chan);
            }
            break;

        case AT86RF2XX_FREQ_868MHZ:
            /* Channel = 0 for 868MHz means 868.3MHz, only one available */
            at86rf2xx_set_chan(dev,0x00);
            break;

        default:
            DEBUG("at86rf2xx: Trying to set unknown frequency 0x%lx\n",
                (unsigned long) freq);
            return;
    }
    dev->freq = freq;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, trx_ctrl2);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RF_CTRL_0, rf_ctrl0);
}
#endif

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
    return tx_pow_to_dbm(dev->freq, txpower);
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
    if (dev->freq == AT86RF2XX_FREQ_915MHZ) {
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                            dbm_to_tx_pow_915[txpower]);
    }
    else if (dev->freq == AT86RF2XX_FREQ_868MHZ) {
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                            dbm_to_tx_pow_868[txpower]);
    }
    else {
        return;
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
        gpio_clear(dev->sleep_pin);
        while (at86rf2xx_get_status(dev) != AT86RF2XX_STATE_TRX_OFF);
    }

    if (state == AT86RF2XX_STATE_SLEEP) {
        /* First go to TRX_OFF */
        at86rf2xx_force_trx_off(dev);
        /* Go to SLEEP mode from TRX_OFF */
        gpio_set(dev->sleep_pin);
    } else {
        _set_state(dev, state);
    }
}

void at86rf2xx_reset_state_machine(at86rf2xx_t *dev)
{
    uint8_t old_state;

    /* Wake up */
    gpio_clear(dev->sleep_pin);

    /* Wait for any state transitions to complete before forcing TRX_OFF */
    do {
        old_state = at86rf2xx_get_status(dev);
    } while (old_state == AT86RF2XX_STATE_IN_PROGRESS);

    at86rf2xx_force_trx_off(dev);
}
