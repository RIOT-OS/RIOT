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
 * @brief       Implementation of the manual calibration facility of the
 *              CC1100/CC1101 driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "xtimer.h"

#include "cc110x.h"
#include "cc110x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Read the calibration data from the transceiver and store it
 *
 * @param   dev     Device descriptor of the transceiver
 *
 * @pre     @p dev is acquired using @p cc110x_acquire
 */
static inline void get_calibration_data(cc110x_t *dev)
{
    char caldata[3];

    cc110x_burst_read(dev, CC110X_REG_FSCAL3, caldata, sizeof(caldata));
    dev->fscal.fscal3 = caldata[0];
    dev->fscal.fscal2 = caldata[1];
    dev->fscal.fscal1[dev->channel] = caldata[2];
}

int cc110x_recalibrate(cc110x_t *dev)
{
    /* Sadly we cannot use GDO0 to check for calibration, as it only
     * provides output in RX/TX state. But after successful manual
     * calibration, the device returns to IDLE state. Thus, we keep
     * calibrating until IDLE state is reached
     */
    do {
        /* Start calibration */
        cc110x_cmd(dev, CC110X_STROBE_CALIBRATE);
        /* Release SPI interface to give other threads a chance to use it */
        cc110x_release(dev);
        /* Manual calibration take 735 micro seconds (see Table 34 on page
         * 54 in the date sheet). We'll wait 750 to be sure
         */
        xtimer_usleep(750);

        /* Re-acquire SPI interface in order to check if calibration
         * succeeded
         */
        cc110x_acquire(dev);
    } while (cc110x_state_from_status(cc110x_status(dev)) != CC110X_STATE_IDLE);

    get_calibration_data(dev);

    return 0;
}

int cc110x_full_calibration(cc110x_t *dev)
{
    DEBUG("[cc110x] Obtaining calibration data for fast channel hopping\n");
    if (!dev) {
        return -EINVAL;
    }

    cc110x_acquire(dev);

    switch (dev->state) {
        case CC110X_STATE_IDLE:
        /* falls through */
        case CC110X_STATE_RX_MODE:
        /* falls through */
        case CC110X_STATE_FSTXON:
            /* Current state is fine for deliberate calibration */
            break;
        default:
            /* Current state prevents deliberate calibration */
            cc110x_release(dev);
            return -EAGAIN;
    }

    uint8_t old_channel = dev->channel;

    /* Disable interrupts on GDO pins */
    gpio_irq_disable(dev->params.gdo0);
    gpio_irq_disable(dev->params.gdo2);

    /* While waiting for calibration to be done, another thread could
     * be scheduled. Setting the state should prevent other threads from
     * messing around with the driver
     */
    dev->state = CC110X_STATE_CALIBRATE;

    /* Go to IDLE to allow setting the channel */
    cc110x_cmd(dev, CC110X_STROBE_IDLE);

    for (dev->channel = 0; dev->channel < CC110X_MAX_CHANNELS; dev->channel++) {
        uint8_t phy_chan = dev->channels->map[dev->channel];
        if (phy_chan == 0xff) {
            /* Channel not supported by channel map */
            continue;
        }
        /* Set the channel to calibrate for fast hopping */
        cc110x_write(dev, CC110X_REG_CHANNR, phy_chan);

        if (cc110x_recalibrate(dev)) {
            /* cc110x_recalibrate() release device on error */
            return -EIO;
        }
    }

    /* Update device to reflect current transceiver state */
    dev->state = CC110X_STATE_IDLE;
    cc110x_release(dev);

    /* Hop back to old channel, IRQs are restored by cc110x_set_channel */
    return cc110x_set_channel(dev, old_channel);
}
