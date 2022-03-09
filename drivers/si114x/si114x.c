/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_si114x
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Si114x proximity/UV/Ambient
 *              light sensor with I2C interface.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *              Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <math.h>
#include <string.h>
#include <stdint.h>

#include "ztimer.h"

#include "periph/i2c.h"

#include "si114x.h"
#include "si114x_internals.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_I2C (dev->params.i2c_dev)

/* Internal function prototypes */
static void _reset(si114x_t *dev);
static void _initialize(si114x_t *dev);
static void _set_param(si114x_t *dev, uint8_t param, uint8_t value);

/*---------------------------------------------------------------------------*
 *                          Si114x Core API                                  *
 *---------------------------------------------------------------------------*/

int8_t si114x_init(si114x_t *dev, const si114x_params_t *params)
{
    dev->params = *params;

    /* wait before sensor is ready */
    ztimer_sleep(ZTIMER_MSEC, SI114X_STARTUP_TIME_MS);

    /* acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* check sensor ID */
    char checkid = 0;
    i2c_read_reg(DEV_I2C, SI114X_ADDR, SI114X_REG_PART_ID, &checkid, 0);
    if ((checkid != SI1145_ID) && (checkid != SI1146_ID) && (checkid != SI1147_ID)) {
        DEBUG("[Error] The given i2c is not enabled.\n");
        i2c_release(DEV_I2C);
        return -SI114X_ERR_NODEV;
    }

    /* reset sensor */
    _reset(dev);

    /* initialize internals registers */
    _initialize(dev);

    /* release I2C device */
    i2c_release(DEV_I2C);

    return SI114X_OK;
}

uint16_t si114x_read_uv(si114x_t *dev)
{
    /* acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t buffer[2];
    i2c_read_regs(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_UV_INDEX0, &buffer, 2, 0);

    /* release I2C device */
    i2c_release(DEV_I2C);

    return (uint16_t)(((buffer[1] << 8) | buffer[0]) / 100);
}

uint16_t si114x_read_ir(si114x_t *dev)
{
    /* acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t buffer[2];
    i2c_read_regs(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_ALS_IR_DATA0, &buffer, 2, 0);

    /* release I2C device */
    i2c_release(DEV_I2C);

    return (uint16_t)((buffer[1] << 8) | buffer[0]);
}

uint16_t si114x_read_visible(si114x_t *dev)
{
    /* acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t buffer[2];
    i2c_read_regs(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_ALS_VIS_DATA0, &buffer, 2, 0);

    /* release I2C device */
    i2c_release(DEV_I2C);

    return (uint16_t)((buffer[1] << 8) | buffer[0]);
}

uint16_t si114x_read_distance(si114x_t *dev)
{
    /* acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t buffer[2];
    i2c_read_regs(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_PS1_DATA0, &buffer, 2, 0);

    /* release I2C device */
    i2c_release(DEV_I2C);

    return (uint16_t)((buffer[1] << 8) | buffer[0]);
}

uint8_t si114x_read_response(si114x_t *dev)
{
    /* acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t buffer[1];
    i2c_read_regs(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_RESPONSE, &buffer, 1, 0);

    /* release I2C device */
    i2c_release(DEV_I2C);

    return buffer[0];
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

void _reset(si114x_t *dev)
{
    DEBUG("Resetting sensor.\n");

    /* write configuration values */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_MEAS_RATE0, 0, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_MEAS_RATE1, 0, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_IRQ_ENABLE, 0, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_IRQ_MODE1, 0, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_IRQ_MODE2, 0, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_INT_CFG, 0, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_IRQ_STATUS, 0xFF, 0);

    /* perform RESET command */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_COMMAND, SI114X_RESET, 0);
    ztimer_sleep(ZTIMER_MSEC, SI114X_WAIT_10MS);

    /* write HW_KEY for proper operation */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_HW_KEY, SI114X_INIT_VALUE, 0);
    ztimer_sleep(ZTIMER_MSEC, SI114X_WAIT_10MS);
}

void _initialize(si114x_t *dev)
{
    DEBUG("Initializing sensor.\n");

    /* set default UV measurement coefs */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_UCOEF0, SI114X_UCOEF0_DEFAULT, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_UCOEF1, SI114X_UCOEF1_DEFAULT, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_UCOEF2, SI114X_UCOEF2_DEFAULT, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_UCOEF3, SI114X_UCOEF3_DEFAULT, 0);

    /* enable measures */
    _set_param(dev, SI114X_PARAM_CHLIST,
               SI114X_PARAM_CHLIST_ENUV | SI114X_PARAM_CHLIST_ENALSIR |
               SI114X_PARAM_CHLIST_ENALSVIS | SI114X_PARAM_CHLIST_ENPS1);

    /* enable interrupt on every sample */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_INT_CFG, SI114X_INTCFG_INTOE, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_IRQ_ENABLE, SI114X_EN_ALS_IE | SI114X_EN_PS1_IE, 0);

    /* active LED current */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_PS_LED21, dev->params.led_current, 0);

    _set_param(dev, SI114X_PARAM_PS1ADCMUX, SI114X_PARAM_ADCMUX_LARGEIR);

    /* proximity sensor uses LED1 */
    _set_param(dev, SI114X_PARAM_PSLED12SEL, SI114X_PARAM_PSLED12SEL_PS1LED1);

    /* ADC gain */
    _set_param(dev, SI114X_PARAM_PSADCGAIN, 0);
    _set_param(dev, SI114X_PARAM_PSADCOUNTER, SI114X_ADC_REC_CLK);

    /* proximity range */
    _set_param(dev, SI114X_PARAM_PSADCMISC,
               SI114X_PARAM_PSADCMISC_RANGE | SI114X_PARAM_PSADCMISC_PSMODE);
    _set_param(dev, SI114X_PARAM_ALSIRADCMUX, SI114X_PARAM_ADCMUX_SMALLIR);

    /* clocks configuration */
    _set_param(dev, SI114X_PARAM_ALSIRADCGAIN, 0);
    _set_param(dev, SI114X_PARAM_ALSIRADCOUNTER, SI114X_ADC_REC_CLK);

    /* high range mode */
    _set_param(dev, SI114X_PARAM_ALSIRADCMISC, SI114X_PARAM_ALSIRADCMISC_RANGE);

    /* clocks config */
    _set_param(dev, SI114X_PARAM_ALSVISADCGAIN, 0);
    _set_param(dev, SI114X_PARAM_ALSVISADCOUNTER, SI114X_ADC_REC_CLK);

    _set_param(dev, SI114X_PARAM_ALSVISADCMISC,
               SI114X_PARAM_ALSVISADCMISC_VISRANGE);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_MEAS_RATE0, 0xFF, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_MEAS_RATE1, 0x00, 0);

    /* auto-run */
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_COMMAND, SI114X_PS_ALS_AUTO, 0);
}

void _set_param(si114x_t *dev, uint8_t param, uint8_t value)
{
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_PARAM_WR, value, 0);
    i2c_write_reg(DEV_I2C, SI114X_ADDR,
                  SI114X_REG_COMMAND, param | SI114X_PARAM_SET, 0);

    uint8_t ret;
    i2c_read_reg(DEV_I2C, SI114X_ADDR,
                 SI114X_REG_PARAM_RD, &ret, 0);
}
