/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lps331ap
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LPS331AP pressure sensor
 *
 * @note The current driver implementation is very basic and allows only for polling the
 *       devices temperature and pressure values. Threshold values and interrupts are not
 *       used.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>

#include "periph/i2c.h"
#include "lps331ap.h"
#include "lps331ap-internal.h"

/**
 * @brief default I2C bus speed for this sensor
 */
#define BUS_SPEED           I2C_SPEED_FAST

/**
 * @brief pressure divider for norming pressure output
 */
#define PRES_DIVIDER        (4096U)

/**
 * @brief temperature base value and divider for norming temperature output
 */
#define TEMP_BASE           (42.5f)
#define TEMP_DIVIDER        (480U)


int lps331ap_init(lps331ap_t *dev, i2c_t i2c, uint8_t address, lps331ap_rate_t rate)
{
    char tmp;

    /* save device specifics */
    dev->i2c = i2c;
    dev->address = address;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    /* initialize underlying I2C bus */
    if (i2c_init_master(dev->i2c, BUS_SPEED) < 0) {
        /* Release the bus for other threads. */
        i2c_release(dev->i2c);
        return -1;
    }

    /* configure device, for simple operation only CTRL_REG1 needs to be touched */
    tmp = LPS331AP_CTRL_REG1_DBDU | LPS331AP_CTRL_REG1_PD |
          (rate << LPS331AP_CTRL_REG1_ODR_POS);
    if (i2c_write_reg(dev->i2c, dev->address, LPS331AP_REG_CTRL_REG1, tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int lps331ap_read_temp(lps331ap_t *dev)
{
    char tmp;
    int16_t val = 0;
    float res = TEMP_BASE;      /* reference value -> see datasheet */

    i2c_acquire(dev->i2c);
    i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_TEMP_OUT_L, &tmp);
    val |= tmp;
    i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_TEMP_OUT_H, &tmp);
    i2c_release(dev->i2c);
    val |= (tmp << 8);

    /* compute actual temperature value in °C */
    res += ((float)val) / TEMP_DIVIDER;

    /* return temperature in m°C */
    return (int)(res * 1000);
}

int lps331ap_read_pres(lps331ap_t *dev)
{
    char tmp;
    int32_t val = 0;
    float res;

    i2c_acquire(dev->i2c);
    i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_PRESS_OUT_XL, &tmp);
    val |= tmp;
    i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_PRESS_OUT_L, &tmp);
    val |= (tmp << 8);
    i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_PRESS_OUT_H, &tmp);
    i2c_release(dev->i2c);
    val |= (tmp << 16);
    /* see if value is negative */
    if (tmp & 0x80) {
        val |= (0xff << 24);
    }

    /* compute actual pressure value in mbar */
    res = ((float)val) / PRES_DIVIDER;

    return (int)res;
}


int lps331ap_enable(lps331ap_t *dev)
{
    char tmp;
    int status;

    i2c_acquire(dev->i2c);
    if (i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_CTRL_REG1, &tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    tmp |= (LPS331AP_CTRL_REG1_PD);
    status = i2c_write_reg(dev->i2c, dev->address, LPS331AP_REG_CTRL_REG1, tmp);
    i2c_release(dev->i2c);

    return status;
}

int lps331ap_disable(lps331ap_t *dev)
{
    char tmp;
    int status;

    i2c_acquire(dev->i2c);
    if (i2c_read_reg(dev->i2c, dev->address, LPS331AP_REG_CTRL_REG1, &tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    tmp &= ~(LPS331AP_CTRL_REG1_PD);
    status = i2c_write_reg(dev->i2c, dev->address, LPS331AP_REG_CTRL_REG1, tmp);
    i2c_release(dev->i2c);

    return status;
}
