/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_hdc2010
 * @{
 *
 * @file
 * @brief       Driver for the TI HDC2010 Humidity and Temperature Sensor.
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "byteorder.h"
#include "xtimer.h"
#include "periph/i2c.h"
#include "hdc2010.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_HDC2010_IRQ
static void _hdc2010_irq_callback(void *arg);
#endif

int hdc2010_init(hdc2010_t *dev, const hdc2010_params_t *params)
{
    uint16_t tmp;

    /* write device descriptor */
    dev->p = *params;

    /* try if we can interact with the device by reading its manufacturer ID */
    i2c_acquire(dev->p.i2c);
    if (i2c_read_regs(dev->p.i2c, dev->p.addr,
                      HDC2010_MANUFACTURER_ID, &tmp, sizeof(tmp), 0) < 0) {
        DEBUG("[HDC2010] Could not read manufacture id.\n");
        i2c_release(dev->p.i2c);
        return HDC2010_NOBUS;
    }

    tmp = ntohs(tmp);
    if (tmp != HDC2010_MID_VALUE) {
        DEBUG("[HDC2010] Wrong manufacture id: %x\n", tmp);
        i2c_release(dev->p.i2c);
        return HDC2010_NODEV;
    }

    /* operation mode */
    uint8_t mode = dev->p.amm;

    /* check if current device has irq pin connected */
    if (IS_USED(MODULE_HDC2010_IRQ) && (dev->p.irq_pin != GPIO_UNDEF)) {
        /* enable DRDY/INT pin */
        mode |= HDC2010_IRQ_ENABLE;

        /* configure interrupt register */
        if (i2c_write_reg(dev->p.i2c, dev->p.addr, HDC2010_INTERRUPT_CONFIG,
                        HDC2010_DRDY_ENABLE, 0) < 0) {
            DEBUG("[HDC2010] Error while setting IRQ register!\n");
            i2c_release(dev->p.i2c);
            return HDC2010_NOBUS;
        }

        mutex_init(&dev->mutex);
        /* lock mutex initially to be unlocked when interrupt is raised */
        mutex_lock(&dev->mutex);
        /* Interrupt set to trigger on falling edge of interrupt pin */
        gpio_init_int(params->irq_pin, GPIO_IN, GPIO_FALLING,
                      _hdc2010_irq_callback, dev);
    }

    /* set operation mode */
    if (i2c_write_reg(dev->p.i2c, dev->p.addr, HDC2010_CONFIG, mode, 0) < 0) {
        DEBUG("[HDC2010] Error while setting operation mode!\n");
        i2c_release(dev->p.i2c);
        return HDC2010_NOBUS;
    }

    /* set resolution and mode */
    if (i2c_write_reg(dev->p.i2c, dev->p.addr, HDC2010_MEASUREMENT_CONFIG,
                      dev->p.res | dev->p.mode, 0) < 0) {
        DEBUG("[HDC2010] Error while setting resolution!\n");
        i2c_release(dev->p.i2c);
        return HDC2010_NOBUS;
    }
    i2c_release(dev->p.i2c);

    /* all set */
    return HDC2010_OK;
}

int hdc2010_trigger_conversion(const hdc2010_t *dev)
{
    int status = HDC2010_OK;
    assert(dev);

    i2c_acquire(dev->p.i2c);
    /* Trigger the measurements */
    if (i2c_write_reg(dev->p.i2c, dev->p.addr, HDC2010_MEASUREMENT_CONFIG,
                      (dev->p.res | dev->p.mode | 0x1), 0) < 0) {
        status = HDC2010_NOBUS;
    }
    i2c_release(dev->p.i2c);
    return status;
}

int hdc2010_get_results(const hdc2010_t *dev, int16_t *temp, int16_t *hum)
{
    int status = HDC2010_OK;
    assert(dev);

    uint16_t buf[2];
    i2c_acquire(dev->p.i2c);

    /* first read the temperature register */
    if (i2c_read_regs(dev->p.i2c, dev->p.addr,
                      HDC2010_TEMPERATURE, buf, 4, 0) < 0) {
        status = HDC2010_BUSERR;
    }
    i2c_release(dev->p.i2c);

    if (status == HDC2010_OK) {
        /* if all ok, we convert the values to their physical representation */
        if (temp) {
            *temp = (int16_t)((((int32_t)buf[0] * 16500) >> 16) - 4000);
        }
        if (hum) {
            *hum  = (int16_t)(((int32_t)buf[1] * 10000) >> 16);
        }
    }
    return status;
}

int hdc2010_read(hdc2010_t *dev, int16_t *temp, int16_t *hum)
{
    if (dev->p.amm == HDC2010_TRIGGER) {
        if (hdc2010_trigger_conversion(dev) != HDC2010_OK) {
            DEBUG("[HDC2010] Error while triggering new measurement!\n");
            return HDC2010_BUSERR;
        }
    }

    if (!IS_USED(MODULE_HDC2010_IRQ) || dev->p.irq_pin == GPIO_UNDEF) {
        /* Wait for measurement to be ready if irq pin not used */
        xtimer_usleep(CONFIG_HDC2010_CONVERSION_TIME);
    } else {
        /* Try to lock mutex till the interrupt is raised or till timeut happens */
        xtimer_mutex_lock_timeout(&dev->mutex, CONFIG_HDC2010_CONVERSION_TIME);
    }

    return hdc2010_get_results(dev, temp, hum);
}

#ifdef MODULE_HDC2010_IRQ
static void _hdc2010_irq_callback(void *arg)
{
    hdc2010_t *dev = (hdc2010_t *)arg;
    mutex_unlock(&(dev->mutex));

    /* call user defined cb */
    if (dev->p.user_cb) {
        (dev->p.user_cb)(arg);
    }
}
#endif
