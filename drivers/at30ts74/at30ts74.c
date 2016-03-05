/*
 * Copyright (C) 2016 University of California, Berkeley
 * Copyright (C) 2016 Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at30ts74
 * @{
 *
 * @file
 * @brief       Driver for the AT30TS74 ±1°C Accurate, 12-Bit Digital I2C
 *              Temperature Sensor from Atmel
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * @}
 */

#include "at30ts74.h"
#include "periph/i2c.h"
#include <xtimer.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define CFG_BITS_SHIFT        (5)
#define REG_TMP_ADDR          (0x00)
#define REG_CFG_ADDR          (0x01)
#define CFG_SHDWN             (0x01)
#define CFG_ONESHOT           (0x80)
#define MILLI_CELSIUS_PER_LSB (625)
#define VALUE_REGISTER_SHIFT  (4)

int at30ts74_init(at30ts74_t *dev, i2c_t i2c, uint8_t address, at30ts74_bits_t precision)
{
    int res;
    char tmp;

    if (precision > AT30TS74_12BIT || precision < AT30TS74_9BIT) {
        return -1;
    }

    dev->i2c = i2c;
    dev->address = address;
    dev->bits = precision;

    /* This adds roughly 20% over what the datasheet suggests
     * but if our oscillator differs from the tmp sensor, we don't
     * want to spuriously retrieve old data. We will be in
     * sleep during sample anyway right?
     */
    dev->samplems = 30 << precision;

    i2c_acquire(dev->i2c);
    i2c_init_master(i2c, I2C_SPEED_NORMAL);

    /* configure and enable the sensor */
    tmp = (precision << CFG_BITS_SHIFT) | CFG_SHDWN;
    res = i2c_write_reg(dev->i2c, address, REG_CFG_ADDR, tmp);

    i2c_release(dev->i2c);
    if (res < 1) {
        return -1;
    }
    return 0;
}

int at30ts74_read(at30ts74_t *dev, int32_t *result)
{
    char res[2];
    int16_t ret;

    i2c_acquire(dev->i2c);

    /* Start the sample process */
    ret = i2c_write_reg(dev->i2c, dev->address, REG_CFG_ADDR,
        (dev->bits << CFG_BITS_SHIFT) | CFG_SHDWN | CFG_ONESHOT);

    /* Wait for the sample process to finish */
    i2c_release(dev->i2c);
    xtimer_usleep(dev->samplems * 1000);
    i2c_acquire(dev->i2c);

    /* Read temperature value */
    ret = i2c_read_regs(dev->i2c, dev->address, REG_TMP_ADDR, res, 2);
    i2c_release(dev->i2c);
    if (ret < 2) {
        return -1;
    }
    ret = (int16_t)(((uint16_t)res[0] << 8) | res[1]);

    /* Calculate and return actual temperature value in C * 10000 */
    *result = (int32_t)(ret >> VALUE_REGISTER_SHIFT) * MILLI_CELSIUS_PER_LSB;
    DEBUG("AT30TS74: Raw value: %d - high: %u, low: %u\n", *result, res[0], res[1]);
    return 0;
}
