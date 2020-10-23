/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Implementation for the "public" API of the CC1100/CC1101 driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "cc110x.h"
#include "cc110x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int cc110x_setup(cc110x_t *dev, const cc110x_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    /* Zero out everything but RIOT's driver interface, which should be
     * managed by RIOT
     */
    memset((char *)dev + sizeof(netdev_t), 0x00,
           sizeof(cc110x_t) - sizeof(netdev_t));
    dev->params = *params;
    dev->netdev.driver = &cc110x_driver;
    dev->state = CC110X_STATE_OFF;
    return 0;
}

int cc110x_apply_config(cc110x_t *dev, const cc110x_config_t *conf,
                        const cc110x_chanmap_t *chanmap, uint8_t channel)
{
    DEBUG("[cc110x] Applying new configuration\n");
    if (!dev || !chanmap) {
        return -EINVAL;
    }

    if ((channel >= CC110X_MAX_CHANNELS) || (chanmap->map[channel] == 0xff)) {
        /* Channel out of range or not supported in current channel map */
        return -ERANGE;
    }

    if (cc110x_acquire(dev) != SPI_OK) {
        return -EIO;
    }

    gpio_irq_disable(dev->params.gdo0);
    gpio_irq_disable(dev->params.gdo2);

    /* Go to IDLE state to allow reconfiguration */
    cc110x_cmd(dev, CC110X_STROBE_IDLE);
    dev->state = CC110X_STATE_IDLE;

    if (conf != NULL) {
        /* Write all three base frequency configuration bytes in one burst */
        cc110x_burst_write(dev, CC110X_REG_FREQ2, &conf->base_freq, 3);

        cc110x_write(dev, CC110X_REG_FSCTRL1, conf->fsctrl1);
        cc110x_write(dev, CC110X_REG_MDMCFG4, conf->mdmcfg4);
        cc110x_write(dev, CC110X_REG_MDMCFG3, conf->mdmcfg3);
        cc110x_write(dev, CC110X_REG_DEVIATN, conf->deviatn);
    }

    /* We only need to store the channel, cc110x_full_calibration() will tune it
     * in after calibration.
     */
    dev->channel = channel;
    dev->channels = chanmap;
    cc110x_release(dev);

    /* prepare hopping will call cc110x_enter_rx_mode(), which restores the IRQs */
    return cc110x_full_calibration(dev);
}

int cc110x_set_tx_power(cc110x_t *dev, cc110x_tx_power_t power)
{
    DEBUG("[cc110x] Applying TX power setting at index %u\n", (unsigned)power);
    if (!dev) {
        return -EINVAL;
    }

    if ((unsigned)power >= CC110X_TX_POWER_NUMOF) {
        return -ERANGE;
    }

    if (cc110x_acquire(dev) != SPI_OK) {
        return -EIO;
    }

    switch (dev->state) {
        case CC110X_STATE_IDLE:
        /* falls through */
        case CC110X_STATE_RX_MODE:
            break;
        default:
            cc110x_release(dev);
            return -EAGAIN;
    }

    uint8_t frend0 = 0x10 | (uint8_t)power;
    cc110x_write(dev, CC110X_REG_FREND0, frend0);
    dev->tx_power = power;
    cc110x_release(dev);
    return 0;
}

int cc110x_set_channel(cc110x_t *dev, uint8_t channel)
{
    DEBUG("[cc110x] Hopping to channel %i\n", (int)channel);
    if (!dev) {
        return -EINVAL;
    }

    if (cc110x_acquire(dev) != SPI_OK) {
        return -EIO;
    }

    if ((channel >= CC110X_MAX_CHANNELS) || (dev->channels->map[channel] == 0xff)) {
        /* Channel out of range or not supported in current channel map */
        cc110x_release(dev);
        return -ERANGE;
    }

    switch (dev->state) {
        case CC110X_STATE_IDLE:
        /* falls through */
        case CC110X_STATE_RX_MODE:
        /* falls through */
        case CC110X_STATE_FSTXON:
            /* Above states are fine for hopping */
            break;
        default:
            /* All other states do not allow hopping right now */
            cc110x_release(dev);
            return -EAGAIN;
    }

    /* Disable IRQs, as e.g. PLL indicator will go LOW in IDLE state */
    gpio_irq_disable(dev->params.gdo0);
    gpio_irq_disable(dev->params.gdo2);

    /* Go to IDLE state to disable frequency synchronizer */
    cc110x_cmd(dev, CC110X_STROBE_IDLE);

    /* Upload new channel and corresponding calibration data */
    cc110x_write(dev, CC110X_REG_CHANNR, dev->channels->map[channel]);

    uint8_t caldata[] = {
        dev->fscal.fscal3, dev->fscal.fscal2, dev->fscal.fscal1[channel]
    };
    cc110x_burst_write(dev, CC110X_REG_FSCAL3, caldata, sizeof(caldata));

    /* Start listening on the new channel (restores IRQs) */
    cc110x_enter_rx_mode(dev);

    dev->channel = channel;
    cc110x_release(dev);

    dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_FHSS_CHANGE_CHANNEL);
    return 0;
}
