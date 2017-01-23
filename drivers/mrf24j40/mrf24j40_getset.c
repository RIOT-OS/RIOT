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
    // TODO
    // noch mal überprüfen
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



void mrf24j40_set_csma_retries(mrf24j40_t *dev, uint8_t retries)
{
    uint8_t tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp &= ~(MRF24J40_MAX_CSMA_RETRIES);
    tmp |= retries;
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp);
}
uint8_t mrf24j40_get_csma_retries(mrf24j40_t *dev)
{
    uint8_t retries = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    retries &= ~(MRF24J40_MAX_CSMA_RETRIES);
    
    return retries;
}

void mrf24j40_set_csma_backoff_exp(mrf24j40_t *dev, uint8_t exponent)
{
    uint8_t tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    tmp &= ~(MRF24J40_CSMA_BACKOFF_EXPONENT);
    tmp |= (exponent << 3);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, value);
}
uint8_t mrf24j40_get_csma_backoff_exp(mrf24j40_t *dev)
{
    uint8_t exponent = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
    exponent &= ~(MRF24J40_CSMA_BACKOFF_EXPONENT);
    
    return (exponent >> 3);
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
        DEBUG("mrf24j40: waking up from sleep mode\n");
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

int16_t mrf24j40_get_txpower(mrf24j40_t *dev)
{

}

void mrf24j40_set_txpower(mrf24j40_t *dev, int16_t txpower)
{
    uint8_t txpower_reg_value;
    if(txpower >= 1)
    {
        txpower += 1;
    }
    txpower_reg_value = (txpower<<3);
    mrf24j40_reg_write_long(dev,MRF24J40_REG_RFCON3, txpower_reg_value);
    // todo : txpower > 6
    
}

void mrf24j40_set_option(mrf24j40_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->netdev.flags |= option;
        /* trigger option specific actions */
        switch (option) {
            case MRF24J40_OPT_CSMA:

                /* Initialize CSMA seed with hardware address */
               
                at86rf2xx_set_csma_max_retries(dev, 5);
                at86rf2xx_set_csma_backoff_exp(dev, 3);
                break;
            case AT86RF2XX_OPT_PROMISCUOUS:
                mrf24j40_reg_write_short(dev->spi,MRF24J40_REG_RXMCR,1)
                break;
            case AT86RF2XX_OPT_ERROR:
                mrf24j40_reg_write_short(dev->spi,MRF24J40_REG_RXMCR,2)
                break;
            case AT86RF2XX_OPT_TELL_RX_START:
                
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_INTCON);
                tmp |= MRF24J40_INTCON_MASK__RX_START;
                mrf24j40_reg_read_write(dev, MRF24J40_REG_INTCON, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->netdev.flags &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case MRF24J40_OPT_CSMA:
                /* setting backoff exponent to 0 means CSMA disabled */
                mrf24j40_set_csma_backoff_exp(dev, 0);
                break;
            case AT86RF2XX_OPT_PROMISCUOUS:
                mrf24j40_reg_write_short(dev->spi,MRF24J40_REG_RXMCR,0)
                break;
            case AT86RF2XX_OPT_ERROR:
                mrf24j40_reg_write_short(dev->spi,MRF24J40_REG_RXMCR,0)
                break;
            case AT86RF2XX_OPT_TELL_RX_START:
                DEBUG("[at86rf2xx] opt: disabling SFD IRQ\n");
                tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_INTCON);
                tmp &= ~MRF24J40_INTCON_MASK__RX_START;
                mrf24j40_reg_read_write(dev, MRF24J40_REG_INTCON, tmp);
                break;
            default:
                /* do nothing */
                break;
        }
    }
}



