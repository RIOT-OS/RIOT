/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lps25h
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LPS25H pressure sensor
 *
 * @note The current driver implementation is very basic and allows only for polling the
 *       devices temperature and pressure values. Threshold values and interrupts are not
 *       used.
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "lps25h.h"
#include "lps25h_internals.h"

#define ENABLE_DEBUG       (0)
#include "debug.h"

/**
 * @brief pressure divider for norming pressure output
 */
#define LPS25H_PRES_DIVIDER       (4096U)

/**
 * @brief temperature base value and divider for norming temperature output
 */
#define LPS25H_TEMP_BASE          (42.5f)
#define LPS25H_TEMP_DIVIDER       (480U)

#define LPS25H_MASK_16BIT_MSB     (0x8000)
#define LPS25H_MASK_24BIT_MSB     (0x800000)

#define LPS25H_BUS                (dev->params.i2c)
#define LPS25H_ADDR               (dev->params.addr)
#define LPS25H_ODR                (dev->params.odr)

/**
  * @brief Takes an unsigned value representing a two's complement number
  *        and returns the signed number it represents
  *
  * @param[in] value    value which represents a two's complement number
  * @param[in] mask     bitmask to choose between the single bits
  *
  * @return             the converted signed number of 'value'
  */
static inline int16_t _twos_complement(int16_t value, uint32_t mask)
{
    if (value & mask) {
        value = ~(value & ~(mask)) + 1;
        return ~(value & ~(mask));
    }
    else {
        return value;
    }
}

int lps25h_init(lps25h_t *dev, const lps25h_params_t *params)
{
    /* check parameters */
    assert(dev && params);

    DEBUG("[DEBUG] Starting device initialization\n");

    /* initialize the device descriptor */
    memcpy(&dev->params, params, sizeof(lps25h_params_t));

    uint8_t tmp;

    i2c_acquire(LPS25H_BUS);
    DEBUG("[DEBUG] I2C bus acquired\n");

    if (i2c_init_master(LPS25H_BUS, I2C_SPEED_NORMAL) < 0) {
        DEBUG("[ERROR] Cannot initialize I2C bus\n");
        i2c_release(LPS25H_BUS);
        return -LPS25H_NOI2C;
    }

    i2c_read_reg(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_WHO_AM_I, &tmp);
    if (tmp & !LPS25H_CHIP_ID) {
        DEBUG("[ERROR] Invalid device found\n");
        i2c_release(LPS25H_BUS);
        return -LPS25H_NODEV;
    }

    tmp = (LPS25H_MASK_CTRL_REG1_PD     /* active mode     */
          | LPS25H_ODR);                /* output data rate */

    if (i2c_write_reg(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_CTRL_REG1, tmp) < 0) {
        DEBUG("[ERROR] Cannot configure device\n");
        i2c_release(LPS25H_BUS);
        return -LPS25H_ERRI2C;
    }

    i2c_release(LPS25H_BUS);
    
    DEBUG("[DEBUG] Device initialized with success\n");
    return LPS25H_OK;
}

int16_t lps25h_read_temperature(const lps25h_t *dev)
{
    char tmp[2] = {0, 0};
    int16_t val = 0;
    float res = 0.0;

    i2c_acquire(LPS25H_BUS);
    i2c_read_regs(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_TEMP_OUT_L, &tmp[0], 2);
    i2c_release(LPS25H_BUS);

    val = (tmp[1] << 8) | tmp[0];

    DEBUG("LPS25H: raw temperature: %i\n", val);
    val = _twos_complement(val, LPS25H_MASK_16BIT_MSB);


    res = LPS25H_TEMP_BASE + ((float)val / LPS25H_TEMP_DIVIDER);

    return (int16_t)res;
}

uint16_t lps25h_read_pressure(const lps25h_t *dev)
{
    uint8_t tmp[3] = {0, 0, 0};
    int32_t val = 0;

    i2c_acquire(LPS25H_BUS);
    i2c_read_regs(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_PRESS_OUT_XL, &tmp[0], 3);
    i2c_release(LPS25H_BUS);

    val = (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];

    val = _twos_complement(val, LPS25H_MASK_24BIT_MSB);

    DEBUG("LPS25H: raw pressure: %li\n", val);

    return (int)(((float)val) / LPS25H_PRES_DIVIDER);
}

void lps25h_enable(const lps25h_t *dev)
{
    uint8_t tmp;

    i2c_acquire(LPS25H_BUS);
    i2c_read_reg(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_CTRL_REG1, &tmp);

    /* adds the active mode flag */
    tmp |= (LPS25H_MASK_CTRL_REG1_PD);

    i2c_write_reg(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_CTRL_REG1, tmp);
    i2c_release(LPS25H_BUS);
}

void lps25h_disable(const lps25h_t *dev)
{
    uint8_t tmp;

    i2c_acquire(LPS25H_BUS);
    i2c_read_reg(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_CTRL_REG1, &tmp);

    /* removes the active mode flag */
    tmp &= ~(LPS25H_MASK_CTRL_REG1_PD);

    i2c_write_reg(LPS25H_BUS, LPS25H_ADDR, LPS25H_REG_CTRL_REG1, tmp);
    i2c_release(LPS25H_BUS);
}
