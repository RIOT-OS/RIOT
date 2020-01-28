/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "lpsxxx.h"
#include "lpsxxx_internal.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief pressure divider for norming pressure output
 */
#define PRES_DIVIDER        (12U)

/**
 * @brief temperature base value and divider for norming temperature output
 */
#if MODULE_LPS331AP || MODULE_LPS25HB
#define TEMP_BASE           (42.5f)
#define TEMP_DIVIDER        (480U)
#else
#define TEMP_BASE           (0.0f)
#define TEMP_DIVIDER        (100U)
#endif

int lpsxxx_init(lpsxxx_t *dev, const lpsxxx_params_t *params)
{
    dev->params = *params;
    common_bus_setup(&(dev->params.transport));

    const common_bus_params_t *bus_ptr = &(dev->params.transport.bus);
    const common_bus_function_t *func_ptr = &(dev->params.transport.f);

    func_ptr->common_bus_init(bus_ptr);

    /* Acquire exclusive access to the bus. */
    func_ptr->common_bus_acquire(bus_ptr);
    uint8_t id = 0;
    int ret = func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_WHO_AM_I, &id);
    if (ret < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] init: cannot read WHO_AM_I register err=%d\n", ret);
        return -LPSXXX_ERR_NOBUS;
    }

    if (id != LPSXXX_WHO_AM_I) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] init: not a valid device (got %02X, expected %02X)\n",
              id, LPSXXX_WHO_AM_I);
        return -LPSXXX_ERR_NODEV;
    }

    uint8_t tmp;

#if MODULE_LPS22HB
    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_CTRL_REG2, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] init: cannot read LPSXXX_REG_CTRL_REG2 register\n");
        return -LPSXXX_ERR_NOBUS;
    }

    /* Disable automatic increment of register address during byte access
       (recommended in datasheet (section 9.6 CTRL_REG2) */
    tmp &= ~LPSXXX_CTRL_REG2_ID_ADD_INC;

    DEBUG("[lpsxxx] init: update reg2, %02X\n", tmp);

    if (func_ptr->common_bus_write_reg(bus_ptr, LPSXXX_REG_CTRL_REG2, tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] init: cannot write in CTRL_REG2 register\n");
        return -LPSXXX_ERR_NOBUS;
    }
#endif

    const uint8_t rate = dev->params.rate << LPSXXX_CTRL_REG1_ODR_POS;
    /* configure device, for simple operation only CTRL_REG1 needs to be touched */
#if MODULE_LPS331AP
    tmp = LPSXXX_CTRL_REG1_DBDU | LPSXXX_CTRL_REG1_PD | rate;
#elif MODULE_LPS25HB
    tmp = LPSXXX_CTRL_REG1_BDU | LPSXXX_CTRL_REG1_PD | rate;
#elif MODULE_LPS22HB
    tmp = LPSXXX_CTRL_REG1_EN_LPFP | /* Low-pass filter configuration: ODR/9 */
        LPSXXX_CTRL_REG1_BDU | rate;
#else
    /* We should actually never enter here, but we want to keep cppcheck happy. */
    if (rate) { /* Use rate to keep cppcheck happy. */
        tmp = 0; /* Set tmp, to keep cppcheck happy. */
    }
#endif

    DEBUG("[lpsxxx] init: update reg1, value: %02X\n", tmp);

    if (func_ptr->common_bus_write_reg(bus_ptr, LPSXXX_REG_CTRL_REG1, tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] init: cannot write in CTRL_REG1 register\n");
        return -LPSXXX_ERR_NOBUS;
    }

    func_ptr->common_bus_release(bus_ptr);

    DEBUG("[lpsxxx] initialization successful\n");
    return LPSXXX_OK;
}

int lpsxxx_read_temp(const lpsxxx_t *dev, int16_t *temp)
{
    uint8_t tmp = 0;
    int16_t val = 0;
    float res = TEMP_BASE;      /* reference value -> see datasheet */

    const common_bus_params_t *bus_ptr = &(dev->params.transport.bus);
    const common_bus_function_t *func_ptr = &(dev->params.transport.f);

    func_ptr->common_bus_acquire(bus_ptr);
    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_TEMP_OUT_L, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] read_temp: cannot read TEMP_OUT_L register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    val |= tmp;

    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_TEMP_OUT_H, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] read_temp: cannot read TEMP_OUT_H register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    func_ptr->common_bus_release(bus_ptr);
    val |= ((uint16_t)tmp << 8);

    DEBUG("[lpsxxx] read_temp: raw data %08" PRIx32 "\n", (uint32_t)val);

    /* compute actual temperature value in °C */
    res += ((float)val) / TEMP_DIVIDER;

    /* return temperature in c°C */
    *temp = (int16_t)(res * 100);
    return LPSXXX_OK;
}

int lpsxxx_read_pres(const lpsxxx_t *dev, uint16_t *pressure)
{
    uint8_t tmp = 0;
    int32_t val = 0;

    const common_bus_params_t *bus_ptr = &(dev->params.transport.bus);
    const common_bus_function_t *func_ptr = &(dev->params.transport.f);

    func_ptr->common_bus_acquire(bus_ptr);

    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_PRESS_OUT_XL, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] read_pres: cannot read PRES_OUT_XL register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    val |= tmp;

    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_PRESS_OUT_L, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] read_pres: cannot read PRES_OUT_L register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    val |= ((uint32_t)tmp << 8);

    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_PRESS_OUT_H, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] read_pres: cannot read PRES_OUT_H register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    func_ptr->common_bus_release(bus_ptr);

    val |= ((uint32_t)tmp << 16);

    DEBUG("[lpsxxx] read_pres: raw data %08" PRIx32 "\n", (uint32_t)val);

    /* see if value is negative */
    if (tmp & 0x80) {
        val |= ((uint32_t)0xff << 24);
    }

    /* compute actual pressure value in hPa */
    *pressure = (uint16_t)(val >> PRES_DIVIDER);

    return LPSXXX_OK;
}

int lpsxxx_enable(const lpsxxx_t *dev)
{
    uint8_t tmp = 0;

    const common_bus_params_t *bus_ptr = &(dev->params.transport.bus);
    const common_bus_function_t *func_ptr = &(dev->params.transport.f);

    func_ptr->common_bus_acquire(bus_ptr);
    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_CTRL_REG1, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] enable: cannot read CTRL_REG1 register\n");
        return -LPSXXX_ERR_NOBUS;
    }

#if MODULE_LPS331AP || MODULE_LPS25HB
    tmp |= LPSXXX_CTRL_REG1_PD;
#else
    const uint8_t rate = dev->params.rate << LPSXXX_CTRL_REG1_ODR_POS;
    tmp |= LPSXXX_CTRL_REG1_EN_LPFP | /* Low-pass filter configuration: ODR/9 */
        LPSXXX_CTRL_REG1_BDU | rate;
#endif


    DEBUG("[lpsxxx] enable: update reg1 with %02X\n", tmp);

    if (func_ptr->common_bus_write_reg(bus_ptr, LPSXXX_REG_CTRL_REG1, tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] enable: cannot write CTRL_REG1 register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    func_ptr->common_bus_release(bus_ptr);

    return LPSXXX_OK;
}

int lpsxxx_disable(const lpsxxx_t *dev)
{
    uint8_t tmp = 0;

    const common_bus_params_t *bus_ptr = &(dev->params.transport.bus);
    const common_bus_function_t *func_ptr = &(dev->params.transport.f);

    func_ptr->common_bus_acquire(bus_ptr);
    if (func_ptr->common_bus_read_reg(bus_ptr, LPSXXX_REG_CTRL_REG1, &tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] disable: cannot read CTRL_REG1 register\n");
        return -LPSXXX_ERR_NOBUS;
    }
#if MODULE_LPS331AP || MODULE_LPS25HB
    tmp &= ~LPSXXX_CTRL_REG1_PD;
#else
    tmp &= ~(7 << LPSXXX_CTRL_REG1_ODR_POS);
#endif

    DEBUG("[lpsxxx] disable: update reg1 with %02X\n", tmp);

    if (func_ptr->common_bus_write_reg(bus_ptr, LPSXXX_REG_CTRL_REG1, tmp) < 0) {
        func_ptr->common_bus_release(bus_ptr);
        DEBUG("[lpsxxx] disable: cannot write CTRL_REG1 register\n");
        return -LPSXXX_ERR_NOBUS;
    }
    func_ptr->common_bus_release(bus_ptr);

    return LPSXXX_OK;
}
