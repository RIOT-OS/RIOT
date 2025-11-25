/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

int cc110x_setup(cc110x_t *dev, const cc110x_params_t *params, uint8_t index)
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
    netdev_register(&dev->netdev, NETDEV_CC110X, index);
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

    cc110x_acquire(dev);

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

static void _set_tx_power(cc110x_t *dev, cc110x_tx_power_t power)
{
    uint8_t frend0 = 0x10 | (uint8_t)power;
    cc110x_write(dev, CC110X_REG_FREND0, frend0);
    dev->tx_power = power;
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

    cc110x_acquire(dev);

    switch (dev->state) {
        case CC110X_STATE_IDLE:
        /* falls through */
        case CC110X_STATE_RX_MODE:
            break;
        default:
            cc110x_release(dev);
            return -EAGAIN;
    }

    _set_tx_power(dev, power);

    cc110x_release(dev);
    return 0;
}

int cc110x_set_channel(cc110x_t *dev, uint8_t channel)
{
    DEBUG("[cc110x] Hopping to channel %i\n", (int)channel);
    if (!dev) {
        return -EINVAL;
    }

    cc110x_acquire(dev);

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

int cc110x_wakeup(cc110x_t *dev)
{
    int err = cc110x_power_on_and_acquire(dev);

    if (err) {
        return err;
    }

    /* PA_TABLE is lost on SLEEP, see 10.6 in the CC1101 data sheet */
    cc110x_burst_write(dev, CC110X_MULTIREG_PATABLE,
                       dev->params.patable->data, CC110X_PATABLE_LEN);
    _set_tx_power(dev, dev->tx_power);

    cc110x_enter_rx_mode(dev);
    cc110x_release(dev);
    return 0;
}

void cc110x_sleep(cc110x_t *dev)
{
    cc110x_acquire(dev);
    if (dev->state == CC110X_STATE_OFF) {
        cc110x_release(dev);
        return;
    }

    /*
     * Datasheet page 9 table 4.
     *
     * To achieve the lowest power consumption GDO's must
     * be programmed to 0x2F
     */
    cc110x_write(dev, CC110X_REG_IOCFG2, CC110X_GDO_CONSTANT_LOW);
    cc110x_write(dev, CC110X_REG_IOCFG1, CC110X_GDO_CONSTANT_LOW);
    cc110x_write(dev, CC110X_REG_IOCFG0, CC110X_GDO_CONSTANT_LOW);

    /* transition to SLEEP only from state IDLE possible */
    cc110x_cmd(dev, CC110X_STROBE_IDLE);
    /* go to SLEEP */
    cc110x_cmd(dev, CC110X_STROBE_OFF);
    dev->state = CC110X_STATE_OFF;
    cc110x_release(dev);
}
