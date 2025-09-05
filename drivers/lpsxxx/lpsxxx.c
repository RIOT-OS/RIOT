/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_lpsxxx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LPSXXX family of pressure sensors
 *
 * @note The current driver implementation is very basic and allows only for polling the
 *       devices temperature and pressure values. Threshold values and interrupts are not
 *       used.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdint.h>

#include "periph/i2c.h"
#include "lpsxxx.h"
#include "lpsxxx_internal.h"
#include "macros/math.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief pressure divider for norming pressure output
 */
#define PRES_DIVIDER        (12U)

/**
 * @brief temperature base value and divider for norming temperature output
 *
 * @details temperature base is given in centi-degree-celsius
 */
#if MODULE_LPS331AP || MODULE_LPS25HB
#define TEMP_BASE           (4250U) /* = 42.5 C */
#define TEMP_DIVIDER        (480U)
#else
#define TEMP_BASE           (0U)
#define TEMP_DIVIDER        (100U)
#endif

#define DEV_I2C      (dev->params.i2c)
#define DEV_ADDR     (dev->params.addr)
#define DEV_RATE     (dev->params.rate)

int lpsxxx_init(lpsxxx_t *dev, const lpsxxx_params_t * params)
{
    dev->params = *params;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(DEV_I2C);
    uint8_t id;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_WHO_AM_I, &id, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] init: cannot read WHO_AM_I register\n");
        return -LPSXXX_ERR_I2C;
    }

    if (id != LPSXXX_WHO_AM_I) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] init: not a valid device (got %02X, expected %02X)\n",
              id, LPSXXX_WHO_AM_I);
        return -LPSXXX_ERR_NODEV;
    }

    uint8_t tmp;

#if MODULE_LPS22HB
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG2, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] init: cannot read LPSXXX_REG_CTRL_REG2 register\n");
        return -LPSXXX_ERR_I2C;
    }

    /* Disable automatic increment of register address during byte access
       (recommended in datasheet (section 9.6 CTRL_REG2) */
    tmp &= ~LPSXXX_CTRL_REG2_ID_ADD_INC;

    DEBUG("[lpsxxx] init: update reg2, %02X\n", tmp);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG2, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] init: cannot write in CTRL_REG2 register\n");
        return -LPSXXX_ERR_I2C;
    }
#endif

    /* configure device, for simple operation only CTRL_REG1 needs to be touched */
#if MODULE_LPS331AP
    tmp = LPSXXX_CTRL_REG1_DBDU | LPSXXX_CTRL_REG1_PD |
            (DEV_RATE << LPSXXX_CTRL_REG1_ODR_POS);
#elif MODULE_LPS25HB
    tmp = LPSXXX_CTRL_REG1_BDU | LPSXXX_CTRL_REG1_PD |
            (DEV_RATE << LPSXXX_CTRL_REG1_ODR_POS);
#elif MODULE_LPS22HB
    tmp = LPSXXX_CTRL_REG1_EN_LPFP | /* Low-pass filter configuration: ODR/9 */
            LPSXXX_CTRL_REG1_BDU | (DEV_RATE << LPSXXX_CTRL_REG1_ODR_POS);
#elif MODULE_LPS22HH || MODULE_LPS22CH
    tmp = LPSXXX_CTRL_REG1_EN_LPFP | /* Low-pass filter configuration: ODR/9 */
            LPSXXX_CTRL_REG1_BDU | (DEV_RATE << LPSXXX_CTRL_REG1_ODR_POS);
#endif

    DEBUG("[lpsxxx] init: update reg1, value: %02X\n", tmp);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG1, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] init: cannot write in CTRL_REG1 register\n");
        return -LPSXXX_ERR_I2C;
    }

    i2c_release(DEV_I2C);

    DEBUG("[lpsxxx] initialization successful\n");
    return LPSXXX_OK;
}

int lpsxxx_read_temp(const lpsxxx_t *dev, int16_t *temp)
{
    uint8_t tmp;
    int16_t val;
    uint16_t res = TEMP_BASE;      /* reference value -> see datasheet */

    i2c_acquire(DEV_I2C);
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_TEMP_OUT_L, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] read_temp: cannot read TEMP_OUT_L register\n");
        return -LPSXXX_ERR_I2C;
    }
    val = tmp;

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_TEMP_OUT_H, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] read_temp: cannot read TEMP_OUT_H register\n");
        return -LPSXXX_ERR_I2C;
    }
    i2c_release(DEV_I2C);
    val |= ((uint16_t)tmp << 8);

    DEBUG("[lpsxxx] read_temp: raw data %08" PRIx16 "\n", val);

    /* compute actual temperature value in c°C */
    res += DIV_ROUND((int32_t)val * 100, TEMP_DIVIDER);

    *temp = res;
    return LPSXXX_OK;
}

int lpsxxx_read_pres(const lpsxxx_t *dev, uint16_t *pres)
{
    uint8_t tmp;
    int32_t val = 0;

    i2c_acquire(DEV_I2C);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_PRESS_OUT_XL, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] read_pres: cannot read PRES_OUT_XL register\n");
        return -LPSXXX_ERR_I2C;
    }
    val |= tmp;

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_PRESS_OUT_L, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] read_pres: cannot read PRES_OUT_L register\n");
        return -LPSXXX_ERR_I2C;
    }
    val |= ((uint32_t)tmp << 8);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_PRESS_OUT_H, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] read_pres: cannot read PRES_OUT_H register\n");
        return -LPSXXX_ERR_I2C;
    }
    i2c_release(DEV_I2C);

    val |= ((uint32_t)tmp << 16);

    DEBUG("[lpsxxx] read_pres: raw data %08" PRIx32 "\n", (uint32_t)val);

    /* see if value is negative */
    if (tmp & 0x80) {
        val |= ((uint32_t)0xff << 24);
    }

    /* compute actual pressure value in hPa */
    *pres = (uint16_t)(val >> PRES_DIVIDER);

    return LPSXXX_OK;
}

int lpsxxx_enable(const lpsxxx_t *dev)
{
    uint8_t tmp;

    i2c_acquire(DEV_I2C);
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG1, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] enable: cannot read CTRL_REG1 register\n");
        return -LPSXXX_ERR_I2C;
    }
#if MODULE_LPS331AP || MODULE_LPS25HB
    tmp |= LPSXXX_CTRL_REG1_PD;
#else
    tmp |= LPSXXX_CTRL_REG1_EN_LPFP | /* Low-pass filter configuration: ODR/9 */
            LPSXXX_CTRL_REG1_BDU | (DEV_RATE << LPSXXX_CTRL_REG1_ODR_POS);
#endif

    DEBUG("[lpsxxx] enable: update reg1 with %02X\n", tmp);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG1, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] enable: cannot write CTRL_REG1 register\n");
        return -LPSXXX_ERR_I2C;
    }
    i2c_release(DEV_I2C);

    return LPSXXX_OK;
}

int lpsxxx_disable(const lpsxxx_t *dev)
{
    uint8_t tmp;

    i2c_acquire(DEV_I2C);
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG1, &tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] disable: cannot read CTRL_REG1 register\n");
        return -LPSXXX_ERR_I2C;
    }
#if MODULE_LPS331AP || MODULE_LPS25HB
    tmp &= ~LPSXXX_CTRL_REG1_PD;
#else
    tmp &= ~(7 << LPSXXX_CTRL_REG1_ODR_POS);
#endif

    DEBUG("[lpsxxx] disable: update reg1 with %02X\n", tmp);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, LPSXXX_REG_CTRL_REG1, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[lpsxxx] disable: cannot write CTRL_REG1 register\n");
        return -LPSXXX_ERR_I2C;
    }
    i2c_release(DEV_I2C);

    return LPSXXX_OK;
}
