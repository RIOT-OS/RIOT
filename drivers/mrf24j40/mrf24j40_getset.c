/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the MRF24J40 drivers
 *
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include "mrf24j40.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "periph/spi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint16_t mrf24j40_get_addr_short(mrf24j40_t *dev)
{
    return (dev->short_addr[0] << 8) | dev->short_addr[1];
}

void mrf24j40_set_addr_short(mrf24j40_t *dev, uint16_t addr)
{
    dev->short_addr[0] = (uint8_t)(addr);
    dev->short_addr[1] = (uint8_t)(addr >> 8);
//#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
  //  dev->short_addr[0] &= 0x7F;
//#endif
    mrf24j40_reg_write_long(dev, MRF24J40_REG_ASSOSADR0,dev->short_addr[1]);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_ASSOSADR1,dev->short_addr[0]);
}

uint64_t mrf24j40_get_addr_long(mrf24j40_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);
    for (int i = 0; i < 8; i++) {
        ap[i] = dev->long_addr[i];
    }
    return addr;
}

void mrf24j40_set_addr_long(mrf24j40_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++) {
        dev->long_addr[i] = (uint8_t)(addr >> (i * 8));
		mrf24j40_reg_write_long(dev, (MRF24J40_REG_ASSOSADR0 + i),(addr >> ((7 - i) * 8)));
    }
}

uint8_t mrf24j40_get_chan(mrf24j40_t *dev)
{
    return dev->chan;
}

void mrf24j40_set_chan(mrf24j40_t *dev, uint8_t channel)
{
    if ((channel < MRF24J40_MIN_CHANNEL) ||
        (channel > MRF24J40_MAX_CHANNEL) ||
        (dev->chan == channel)) {
        return;
    }

    dev->chan = channel;

    mrf24j40_configure_phy(dev);
}

uint16_t mrf24j40_get_pan(mrf24j40_t *dev)
{
    return dev->pan;
}

void mrf24j40_set_pan(mrf24j40_t *dev, uint16_t pan)
{
    uint8_t pan0,pan1;
	pan0 = (uint8_t)(pan);
    pan1 = (uint8_t)(pan >> 8);
    dev->pan = pan;
    
    mtf24j40_reg_write_short(dev, MRF24J40_REG_PANIDL, pan0);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PANIDH, pan1);
}

/* // wegen krummen zahlen und beim at86rf2xx nur gerade zahlen
// muss noch bearbeitet werden // nachfragen
int16_t mrf24j40_get_txpower(mrf24j40_t *dev)
{
    uint8_t txpower = mrf24j40_reg_read_long(dev, AT86RF2XX_REG__PHY_TX_PWR)
                & AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
    return tx_pow_to_dbm[txpower];
}
// muss noch bearbeitet werden
void mrf24j40_set_txpower(mrf24j40_t *dev, int16_t txpower)
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
*/


void mrf24j40_set_csma_backoff_exp(mrf24j40_t *dev, uint8_t min, uint8_t max)
{
    max = (max > 5) ? 5 : max;
    min = (min > max) ? max : min;
    DEBUG("[at86rf2xx] opt: Set min BE=%u, max BE=%u\n", min, max);

	uint8_t tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp &= ~(MRF24J40_REG_TXMCR_CSMA_BACKOFF_MASK);
    tmp |= (min << 3) | (max);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
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



// muss noch implementiert werden
void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->flags |= option;
        /* trigger option specific actions */
        switch (option) {
            case AT86RF2XX_OPT_CSMA:
                DEBUG("[at86rf2xx] opt: enabling CSMA mode" \
                      "(4 retries, min BE: 3 max BE: 5)\n");
                /* Initialize CSMA seed with hardware address */
                at86rf2xx_set_csma_seed(dev, dev->long_addr);
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
        dev->flags &= ~(option);
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
                if (dev->flags & AT86RF2XX_OPT_AUTOACK) {
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

static inline void _set_state(mrf24j40_t *dev, uint8_t state)
{
    mrf24j40_reg_write(dev, MRF24J40_REG_RFCTL, state);
    while (mrf24j40_get_status(dev) != state);
    dev->state = state;
}

void mrf24j40_set_state(mrf24j40_t *dev, uint8_t state)
{
    uint8_t old_state = mrf24j40_get_status(dev);

    if (state == old_state) {
        return;
    }
   
   /* check if we need to wake up from sleep mode */
    else if (old_state == MRF24J40_RFSTATE_SLEEP) {
        DEBUG("at86rf2xx: waking up from sleep mode\n");
        mrf24j40_assert_awake(dev);
    }

    if (state == MRF24J40_RFSTATE_SLEEP) {
        /* reset */
        mrf24j40_force_reset(dev);
        /* Discard all IRQ flags */
        mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT);
        gpio_set(dev->sleep_pin);
        dev->state = state;
    } else {
        _set_state(dev, state);
    }
}

void mrf24j40_reset_state_machine(mrf24j40_t *dev)
{
    uint8_t old_state;

    mrf24j40_assert_awake(dev);

    /* Wait for any state transitions to complete before forcing TRX_OFF */
    do {
        old_state = mrf24j40_get_status(dev);
    } while (old_state == MRF24J40_RFSTATE_CALVCO || old_state == MRF24J40_RFSTATE_CALFIL);

    mrf24j40_force_reset(dev);
}