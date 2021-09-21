/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Driver for the LM75 temperature sensor.
 *
 * A general driver for the lm75 temperature sensor including support for the
 * lm75a and tmp1075 sensors as well.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 *
 * @}
 */

#include "board.h"
#include "lm75.h"
#include "lm75_regs.h"
#include "lm75_params.h"
#include "log.h"
#include "kernel_defines.h"
#include "periph/gpio.h"
#include <stdint.h>
#include <byteorder.h>
#include "xtimer.h"
#include "debug.h"

#define I2C_BUS                        dev->lm75_params.i2c_bus
#define I2C_ADDR                       dev->lm75_params.i2c_addr
#define LM75_CONFIG_SHUTDOWN_MODE      0x01
#define TMP1075_CONFIG_ONE_SHOT_MODE   0x81 /* also sets the shutdown register to 1 */

#if IS_ACTIVE(MODULE_LM75A)
lm75_properties_t lm75a_properties = {
    .os_res         = LM75A_OS_RES,
    .os_mult        = LM75A_OS_MULT,
    .temp_res       = LM75A_TEMP_RES,
    .temp_mult      = LM75A_TEMP_MULT,
    .os_shift       = LM75A_OS_SHIFT,
    .temp_shift     = LM75A_TEMP_SHIFT,
};
#endif

#if IS_ACTIVE(MODULE_TMP1075)
lm75_properties_t tmp1075_properties = {
    .os_res         = TMP1075_OS_RES,
    .os_mult        = TMP1075_OS_MULT,
    .temp_res       = TMP1075_TEMP_RES,
    .temp_mult      = TMP1075_TEMP_MULT,
    .os_shift       = TMP1075_OS_SHIFT,
    .temp_shift     = TMP1075_TEMP_SHIFT,
 };
#endif

int lm75_init(lm75_t *dev, const lm75_params_t *params) {

    dev->lm75_params = *params;
    uint8_t config = (params->shutdown_mode) | (params->tm_mode << 1) \
                   | (params->polarity << 2) | (params->fault_q << 3);

    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }

    /* read the device ID register of the TMP1075 sensor to confirm it is a TMP1075 */
    if (IS_USED(MODULE_TMP1075) && (dev->lm75_params.res == &tmp1075_properties)) {
        uint16_t deid = 0;
        if (i2c_read_regs(I2C_BUS, I2C_ADDR, TMP1075_DEVICE_ID_REG, &deid, 2, 0) != 0) {
            LOG_ERROR("lm75: error reading device ID\n");
         }

        deid = (uint16_t)ntohs(deid);
        /* checks if the device ID corresponds to the TMP1075 sensor
         * and extends the parameter configuration if so */
        if (deid == 0x7500) {
            DEBUG("lm75: device is a TMP1075\n");
            config |= (params->conv_rate_reg << 5);
        }
        else {
            LOG_ERROR("lm75: device ID Register doesnt match");
            i2c_release(I2C_BUS);
            return LM75_ERROR;
        }
    }
    else if (IS_USED(MODULE_LM75A) && (dev->lm75_params.res == &lm75a_properties)) {
        DEBUG("lm75: device is an LM75A\n");
    }
    else {
        LOG_ERROR("lm75: device not supported\n");
        i2c_release(I2C_BUS);
        return LM75_ERROR;
    }

    /* write the config byte into the configuration register */
    if (i2c_write_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, config, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR_I2C;
    }

    i2c_release(I2C_BUS);
    return LM75_SUCCESS;
}

int lm75_get_temperature_raw(lm75_t *dev, int *temperature) {

    int16_t temp;
    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }
    /* read the temperature register */
    if (i2c_read_regs(I2C_BUS, I2C_ADDR, LM75_TEMP_REG, &temp, 2, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR_I2C;
    }
    i2c_release(I2C_BUS);

    /* since the value is read in big endian, it must be converted to little endian
    * then it must be multiplied by its resolution
    * and then shifted so only the correct number of bits is used */
    *temperature = (int16_t)ntohs(temp) * dev->lm75_params.res->temp_res \
                    >> dev->lm75_params.res->temp_shift;

    return LM75_SUCCESS;
}

int lm75_get_temperature(lm75_t *dev, int *temperature) {

    if (lm75_get_temperature_raw(dev, temperature) != 0) {
        return LM75_ERROR;
    }
    /* if the device's resolution is lower than mºC convert the temp to mºC */
    if (dev->lm75_params.res->temp_mult < 1000) {
        *temperature *= 1000 / dev->lm75_params.res->temp_mult;
    }

    /* if the device's resolution is greater than mºC
     * truncates the device's values lower than the mºC range */
    else if (dev->lm75_params.res->temp_mult > 1000) {
        *temperature /= dev->lm75_params.res->temp_mult / 1000;
    }

    return LM75_SUCCESS;
}

int lm75_set_temp_limits(lm75_t *dev, int temp_hyst, int temp_os, gpio_cb_t cb, void *arg) {

    /* Check if the OS alert pin is valid */
    if (!gpio_is_valid(dev->lm75_params.gpio_alarm)) {
        return LM75_ERROR;
    }

    /* Enable OS interrupt */
    if (gpio_init_int(dev->lm75_params.gpio_alarm, GPIO_IN, \
        dev->lm75_params.polarity ? GPIO_FALLING : GPIO_RISING, cb, arg) != 0) {
            return LM75_ERROR;
    }

    if (temp_hyst >= temp_os) {
        LOG_ERROR("lm75: THYST must be lower than TOS\n");
        return LM75_ERROR;
    }
    int16_t temp_hyst_short, temp_os_short;

    /* getting into the correct precision value in units of 10 */
    temp_hyst = (temp_hyst * dev->lm75_params.res->os_mult) / 1000;
    temp_os   = (temp_os * dev->lm75_params.res->os_mult) / 1000;

    /* temp must first be converted to 16 bit format, and sampled to its resolution
     * then shifted by the number of unused bits and finally reversed
     * into little endian for writing into the register.
     * NOTE: values smaller than the resolution steps are truncated */
    temp_hyst_short = (int16_t) (temp_hyst / dev->lm75_params.res->os_res);
    temp_hyst_short = temp_hyst_short << dev->lm75_params.res->os_shift;
    temp_hyst_short = ntohs(temp_hyst_short);
    temp_os_short   = (int16_t) (temp_os / dev->lm75_params.res->os_res);
    temp_os_short   = temp_os_short << dev->lm75_params.res->os_shift;
    temp_os_short   = ntohs(temp_os_short);

    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }

    if (i2c_write_regs(I2C_BUS, I2C_ADDR, LM75_THYST_REG, &temp_hyst_short, 2, 0) != 0) {
        i2c_release(I2C_BUS);
        LOG_ERROR("lm75: ERROR wrtiting Hyst temp\n");
        return LM75_ERROR_I2C;
    }

    if (i2c_write_regs(I2C_BUS, I2C_ADDR, LM75_TOS_REG, &temp_os_short, 2, 0) != 0) {
        i2c_release(I2C_BUS);
        LOG_ERROR("lm75: ERROR writing OS temp\n");
        return LM75_ERROR_I2C;
    }

    i2c_release(I2C_BUS);
    return LM75_SUCCESS;
}

int lm75_get_os_temp(lm75_t *dev, int *temperature) {

    int16_t temp;
    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }
    /* read the temperature register */
    if (i2c_read_regs(I2C_BUS, I2C_ADDR, LM75_TOS_REG, &temp, 2, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR_I2C;
    }

    i2c_release(I2C_BUS);

    /* since the value is read in big endian, it must be converted into little endian
     * then it must be multiplied by its resolution
     * and then shifted by the number of unused bits that must be discarded */
     *temperature = (int16_t)ntohs(temp) * dev->lm75_params.res->os_res \
                   >> dev->lm75_params.res->os_shift;

    return LM75_SUCCESS;
}

int lm75_get_hyst_temp(lm75_t *dev, int *temperature) {

    int16_t temp;
    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }

    /* read the temperature register */
    if (i2c_read_regs(I2C_BUS, I2C_ADDR, LM75_THYST_REG, &temp, 2, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR_I2C;
    }

    i2c_release(I2C_BUS);
    *temperature = (int16_t)ntohs(temp) * dev->lm75_params.res->os_res \
                   >> dev->lm75_params.res->os_shift;

    return LM75_SUCCESS;
}

int lm75_get_os_pin(lm75_t *dev, bool *os_pin_state) {

    if (!gpio_is_valid(dev->lm75_params.gpio_alarm)) {
        LOG_ERROR("lm75: OS alert pin not connected or defined\n");
        return LM75_ERROR;
    }

    *os_pin_state = !!gpio_read(dev->lm75_params.gpio_alarm) == dev->lm75_params.polarity;
    return LM75_SUCCESS;
}

int lm75_poweroff(lm75_t *dev) {

    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }

    uint8_t config;

    if (i2c_read_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, &config, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR;
    }

    /* sets every register to 0 except the shutdown reg and sees if it is active */
    if ((config & LM75_CONFIG_SHUTDOWN_MODE) != 0) {
        LOG_ERROR("lm75: device already in shutdown mode\n");
        i2c_release(I2C_BUS);
        return LM75_SUCCESS;
    }

    /* set the shutdown register to 1 (shutdown mode) and keeps every other intact */
    config |= LM75_CONFIG_SHUTDOWN_MODE;
    if (i2c_write_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, config, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR;
    }

    i2c_release(I2C_BUS);
    return LM75_SUCCESS;
}

int lm75_poweron(lm75_t *dev) {

    if (i2c_acquire(I2C_BUS) != 0) {
        return LM75_ERROR_I2C;
    }

    uint8_t config;
    if (i2c_read_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, &config, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR;
    }

    /* sets every reg to 0 except the shutdown register and sees if it is active */
    if ((config & LM75_CONFIG_SHUTDOWN_MODE) == 0) {
        LOG_INFO("lm75: device is already awake\n");
        i2c_release(I2C_BUS);
        return LM75_SUCCESS;
    }
    /* set the shutdown bit to 0 (continuous conversion mode) and keep every other reg intact */
    config &= ~LM75_CONFIG_SHUTDOWN_MODE;

    if (i2c_write_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, config, 0) != 0) {
        i2c_release(I2C_BUS);
        return LM75_ERROR;
    }

    i2c_release(I2C_BUS);
    return LM75_SUCCESS;
}

/* Performs a single temperature conversion from shutdown mode and goes back into shutdown */
int tmp1075_one_shot(lm75_t *dev) {

    if (!IS_USED(MODULE_TMP1075) && (dev->lm75_params.res != &tmp1075_properties)) {
        LOG_ERROR("lm75: device incompatible with the one shot conversion function\n");
        return LM75_ERROR;
    }

    else {
        if (i2c_acquire(I2C_BUS) != 0) {
            return LM75_ERROR_I2C;
        }

        uint8_t config;
        if (i2c_read_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, &config, 0) != 0) {
            i2c_release(I2C_BUS);
            return LM75_ERROR;
        }

        /* set the shutdown and one shot mode bits to 1 and keep every other register intact */
        config |= TMP1075_CONFIG_ONE_SHOT_MODE;
        if (i2c_write_reg(I2C_BUS, I2C_ADDR, LM75_CONF_REG, config, 0) != 0) {
            i2c_release(I2C_BUS);
            return LM75_ERROR;
         }

        i2c_release(I2C_BUS);
    }

    return LM75_SUCCESS;
}

int lm75_low_power_mode(lm75_t *dev, uint16_t interval) {

    if (IS_USED(MODULE_TMP1075) && (dev->lm75_params.res == &tmp1075_properties)) {
        if (tmp1075_one_shot(dev) != 0) {
            return LM75_ERROR;
        }
        xtimer_msleep(interval);
    }
    else {
        if (lm75_poweron(dev) != 0) {
                return LM75_ERROR;
        }
        /* this is required to ensure the temp register updates for followup readings
        * otherwise the temperature register will have outdated and possibly bogus values */
        if (interval < dev->lm75_params.conv_rate) {
            xtimer_msleep(dev->lm75_params.conv_rate);
        }
        else {
            xtimer_msleep(interval);
        }

        if (lm75_poweroff(dev) != 0) {
            return LM75_ERROR;
        }
    }

    return LM75_SUCCESS;
}
