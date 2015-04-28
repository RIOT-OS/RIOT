/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "ng_at86rf2xx.h"
#include "ng_at86rf2xx_internal.h"
#include "ng_at86rf2xx_registers.h"
#include "periph/spi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


static const int16_t tx_pow_to_dbm[] = {3, 3, 2, 2, 1, 1, 0,
                                        -1, -2, -3, -4, -5, -7, -9, -12, -17};

static const uint8_t dbm_to_tx_pow[] = {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                        0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b,
                                        0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                        0x05, 0x03, 0x00};

uint16_t ng_at86rf2xx_get_addr_short(ng_at86rf2xx_t *dev)
{
    return (dev->addr_short[0] << 8) | dev->addr_short[1];
}

void ng_at86rf2xx_set_addr_short(ng_at86rf2xx_t *dev, uint16_t addr)
{
    dev->addr_short[0] = addr >> 8;
    dev->addr_short[1] = addr & 0xff;
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__SHORT_ADDR_0,
                           dev->addr_short[0]);
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__SHORT_ADDR_1,
                           dev->addr_short[1]);
}

uint64_t ng_at86rf2xx_get_addr_long(ng_at86rf2xx_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);
    for (int i = 0; i < 8; i++) {
        ap[i] = dev->addr_long[7 - i];
    }
    return addr;
}

void ng_at86rf2xx_set_addr_long(ng_at86rf2xx_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++) {
        dev->addr_long[i] = (addr >> ((7 - i) * 8));
        ng_at86rf2xx_reg_write(dev, (NG_AT86RF2XX_REG__IEEE_ADDR_0 + i),
                               dev->addr_long[i]);
    }
}

uint8_t ng_at86rf2xx_get_chan(ng_at86rf2xx_t *dev)
{
    return dev->chan;
}

void ng_at86rf2xx_set_chan(ng_at86rf2xx_t *dev, uint8_t channel)
{
    uint8_t tmp;

    if (channel < NG_AT86RF2XX_MIN_CHANNEL
        || channel > NG_AT86RF2XX_MAX_CHANNEL) {
        return;
    }
    dev->chan = channel;
    tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= ~(NG_AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    tmp |= (channel & NG_AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__PHY_CC_CCA, tmp);
}

#ifdef MODULE_NG_AT86RF212B
ng_at86rf2xx_freq_t ng_at86rf2xx_get_freq(ng_at86rf2xx_t *dev)
{
    return dev->freq;
}

void ng_at86rf2xx_set_freq(ng_at86rf2xx_t *dev, ng_at86rf2xx_freq_t freq)
{
    uint8_t tmp;
    tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__TRX_CTRL_2);
    tmp &= ~(NG_AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE);

    if (freq == NG_AT86RF2XX_FREQ_915MHZ) {
        dev->freq = NG_AT86RF2XX_FREQ_915MHZ;
        /* settings used by Linux 4.0rc at86rf212b driver  - BPSK-40*/
        tmp |= NG_AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE
              | NG_AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_SCRAM_EN;
        if (dev->chan == 0) {
            ng_at86rf2xx_set_chan(dev,NG_AT86RF2XX_DEFAULT_CHANNEL);
        } else {
            ng_at86rf2xx_set_chan(dev,dev->chan);
        }
    } else if (freq == NG_AT86RF2XX_FREQ_868MHZ) {
        dev->freq = NG_AT86RF2XX_FREQ_868MHZ;
        /* OQPSK-SIN-RC-100 IEEE802.15.4 for 868,3MHz */
        tmp |= NG_AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK;
        /* Channel = 0 for 868MHz means 868.3MHz, only one available */
        ng_at86rf2xx_set_chan(dev,0x00);
    } else {
        return;
    }
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_CTRL_2, tmp);
}
#endif

uint16_t ng_at86rf2xx_get_pan(ng_at86rf2xx_t *dev)
{
    return dev->pan;
}

void ng_at86rf2xx_set_pan(ng_at86rf2xx_t *dev, uint16_t pan)
{
    dev->pan = pan;
    DEBUG("pan0: %u, pan1: %u\n", (uint8_t)pan, pan >> 8);
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__PAN_ID_0, (uint8_t)pan);
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__PAN_ID_1, (pan >> 8));
}

int16_t ng_at86rf2xx_get_txpower(ng_at86rf2xx_t *dev)
{
    uint8_t txpower = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_TX_PWR)
                & NG_AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
    return tx_pow_to_dbm[txpower];
}

void ng_at86rf2xx_set_txpower(ng_at86rf2xx_t *dev, int16_t txpower)
{
    txpower += 17;
    if (txpower < 0) {
        txpower = 0;
    } else if (txpower > 20) {
        txpower = 20;
    }
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__PHY_TX_PWR,
                           dbm_to_tx_pow[txpower]);
}

void ng_at86rf2xx_set_option(ng_at86rf2xx_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->options |= option;
        /* trigger option specific actions */
        switch (option) {
            case NG_AT86RF2XX_OPT_CSMA:
                DEBUG("[ng_at86rf2xx] opt: enabling CSMA mode\n");
                /* TODO: en/disable csma */
                break;
            case NG_AT86RF2XX_OPT_PROMISCUOUS:
                DEBUG("[ng_at86rf2xx] opt: enabling PROMISCUOUS mode\n");
                /* disable auto ACKs in promiscuous mode */
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__CSMA_SEED_1);
                tmp |= NG_AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK;
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__CSMA_SEED_1, tmp);
                /* enable promiscuous mode */
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__XAH_CTRL_1);
                tmp |= NG_AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE;
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__XAH_CTRL_1, tmp);
                break;
            case NG_AT86RF2XX_OPT_AUTOACK:
                DEBUG("[ng_at86rf2xx] opt: enabling auto ACKs\n");
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__CSMA_SEED_1);
                tmp &= ~(NG_AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__CSMA_SEED_1, tmp);
                break;
            case NG_AT86RF2XX_OPT_TELL_RX_START:
                DEBUG("[ng_at86rf2xx] opt: enabling SFD IRQ\n");
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__IRQ_MASK);
                tmp |= NG_AT86RF2XX_IRQ_STATUS_MASK__RX_START;
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__IRQ_MASK, tmp);
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
            case NG_AT86RF2XX_OPT_CSMA:
                /* TODO: en/disable csma */
                break;
            case NG_AT86RF2XX_OPT_PROMISCUOUS:
                /* disable promiscuous mode */
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__XAH_CTRL_1);
                tmp &= ~(NG_AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE);
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__XAH_CTRL_1, tmp);
                /* re-enable AUTOACK only if the option is set */
                if (dev->options & NG_AT86RF2XX_OPT_AUTOACK) {
                    tmp = ng_at86rf2xx_reg_read(dev,
                                                NG_AT86RF2XX_REG__CSMA_SEED_1);
                    tmp &= ~(NG_AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
                    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__CSMA_SEED_1,
                                           tmp);
                }
                break;
            case NG_AT86RF2XX_OPT_AUTOACK:
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__CSMA_SEED_1);
                tmp |= NG_AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK;
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__CSMA_SEED_1, tmp);
                break;
            case NG_AT86RF2XX_OPT_TELL_RX_START:
                DEBUG("[ng_at86rf2xx] opt: disabling SFD IRQ\n");
                tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__IRQ_MASK);
                tmp &= ~NG_AT86RF2XX_IRQ_STATUS_MASK__RX_START;
                ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__IRQ_MASK, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
}

uint8_t ng_at86rf2xx_get_state(ng_at86rf2xx_t *dev)
{
    uint8_t status = ng_at86rf2xx_get_status(dev);
    return (status & 0x1f);
}

static inline void _set_state(ng_at86rf2xx_t *dev, uint8_t state)
{
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, state);
    while (ng_at86rf2xx_get_state(dev) != state);
}

void ng_at86rf2xx_set_state(ng_at86rf2xx_t *dev, uint8_t state)
{
    uint8_t old_state = ng_at86rf2xx_get_state(dev);

    if (state == old_state) {
        return;
    }
    /* make sure there is no ongoing transmission */
    while (old_state == NG_AT86RF2XX_STATE_BUSY_RX_AACK ||
           old_state == NG_AT86RF2XX_STATE_BUSY_TX_ARET) {
        old_state = ng_at86rf2xx_get_state(dev);
    }
    /* check if we need to wake up from sleep mode */
    if (old_state == NG_AT86RF2XX_STATE_SLEEP) {
        DEBUG("at86rf2xx: waking up from sleep mode\n");
        gpio_clear(dev->sleep_pin);
        while (ng_at86rf2xx_get_state(dev) != NG_AT86RF2XX_STATE_TRX_OFF);
    }
    /* go to neutral TRX_OFF state */
    _set_state(dev, NG_AT86RF2XX_STATE_TRX_OFF);
    if (state == NG_AT86RF2XX_STATE_RX_AACK_ON ||
        state == NG_AT86RF2XX_STATE_TX_ARET_ON) {
        _set_state(dev, state);
    } else if (state == NG_AT86RF2XX_STATE_SLEEP) {
        gpio_set(dev->sleep_pin);
        while (ng_at86rf2xx_get_state(dev) != NG_AT86RF2XX_STATE_SLEEP);
    }
}
