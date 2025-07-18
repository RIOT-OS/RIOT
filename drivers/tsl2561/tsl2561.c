/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tsl2561
 * @{
 *
 * @file
 * @brief       Device driver implementation for the TSL2561 Luminosity sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <math.h>
#include <string.h>

#include "log.h"
#include "tsl2561.h"
#include "tsl2561_internals.h"
#include "periph/i2c.h"
#include "ztimer.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define DEV_I2C             (dev->params.i2c_dev)
#define DEV_ADDR            (dev->params.addr)
#define DEV_GAIN            (dev->params.gain)
#define DEV_INTEGRATION     (dev->params.integration)

/* internal helpers */
static void _enable(const tsl2561_t *dev);
static void _disable(const tsl2561_t *dev);
static void _read_data(const tsl2561_t *dev, uint16_t *full, uint16_t *ir);
static void _print_init_info(const tsl2561_t *dev);

/*---------------------------------------------------------------------------*
 *                          TSL2561 Core API                                 *
 *---------------------------------------------------------------------------*/

int tsl2561_init(tsl2561_t *dev, const tsl2561_params_t *params)
{
    dev->params = *params;

    _print_init_info(dev);

    DEBUG("[Info] I2C device initialized with success!\n");

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    DEBUG("[Info] Access acquired !\n");

    /* Verify sensor ID */
    uint8_t id;
    i2c_read_reg(DEV_I2C, DEV_ADDR,
                 TSL2561_COMMAND_MODE | TSL2561_REGISTER_ID, &id, 0);
    DEBUG("[Info] ID ? %d\n", id);
    if (id != TSL2561_ID ) {
        DEBUG("[Error] not a TSL2561 sensor\n");
        return TSL2561_BADDEV;
    }

    _enable(dev);

    /* configuring gain and integration time */
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  TSL2561_COMMAND_MODE | TSL2561_REGISTER_TIMING,
                  DEV_INTEGRATION | DEV_GAIN, 0);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        uint8_t timing;
        i2c_read_reg(DEV_I2C, DEV_ADDR,
                     TSL2561_COMMAND_MODE | TSL2561_REGISTER_TIMING,
                     &timing, 0);
        DEBUG("[Info] Timing ? %d (expected: %d)\n",
              timing, DEV_INTEGRATION | DEV_GAIN);
    }

    _disable(dev);
    i2c_release(DEV_I2C);

    return TSL2561_OK;
}

uint16_t tsl2561_read_illuminance(const tsl2561_t *dev)
{
    /* Read IR and full spectrum values */
    uint16_t ir = 0;
    uint16_t full = 0;
    _read_data(dev, &full, &ir);

    DEBUG("[Info] Full spectrum value: %i\n", (int)full);
    DEBUG("[Info] IR spectrum value: %i\n", (int)ir);

    /* Compute illuminance */
    uint32_t channel_scale;
    uint32_t channel_1;
    uint32_t channel_0;

    switch (DEV_INTEGRATION) {
        case TSL2561_INTEGRATIONTIME_13MS:
            channel_scale = TSL2561_CHSCALE_TINT0;
            break;

        case TSL2561_INTEGRATIONTIME_101MS:
            channel_scale = TSL2561_CHSCALE_TINT1;
            break;

        default: /* No scaling ... integration time = 402ms */
            channel_scale = (1 << TSL2561_CHSCALE);
            break;
    }

    /* Scale for gain (1x or 16x) */
    if (!DEV_GAIN) {
        channel_scale = channel_scale << 4;
    }

    /* scale the channel values */
    channel_0 = (full * channel_scale) >> TSL2561_CHSCALE;
    channel_1 = (ir * channel_scale) >> TSL2561_CHSCALE;

    /* find the ratio of the channel values (Channel1/Channel0) */
    uint32_t ratio_1 = 0;
    if (channel_0 != 0) {
        ratio_1 = (channel_1 << (TSL2561_RATIOSCALE + 1)) / channel_0;
    }

    /* round the ratio value */
    uint32_t ratio = (ratio_1 + 1) >> 1;
    uint32_t b, m;

    if (ratio <= TSL2561_K1T) {
        b = TSL2561_B1T;
        m = TSL2561_M1T;
    }
    else if (ratio <= TSL2561_K2T) {
        b = TSL2561_B2T;
        m = TSL2561_M2T;
    }
    else if (ratio <= TSL2561_K3T) {
        b = TSL2561_B3T;
        m = TSL2561_M3T;
    }
    else if (ratio <= TSL2561_K4T) {
        b = TSL2561_B4T;
        m = TSL2561_M4T;
    }
    else if (ratio <= TSL2561_K5T) {
        b = TSL2561_B5T;
        m = TSL2561_M5T;
    }
    else if (ratio <= TSL2561_K6T) {
        b = TSL2561_B6T;
        m = TSL2561_M6T;
    }
    else if (ratio <= TSL2561_K7T) {
        b = TSL2561_B7T;
        m = TSL2561_M7T;
    }
    else {
        b = TSL2561_B8T;
        m = TSL2561_M8T;
    }

    uint32_t illuminance;
    illuminance = ((channel_0 * b) - (channel_1 * m));

    /* round lsb (2^(TSL2561_SCALE - 1)) */
    illuminance += (1 << (TSL2561_LUXSCALE - 1));

    /* return strip off fractional portion */
    return (uint16_t)(illuminance >> TSL2561_LUXSCALE);
}

static void _enable(const tsl2561_t *dev)
{
    /* enabling device */
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  TSL2561_COMMAND_MODE | TSL2561_REGISTER_CONTROL,
                  TSL2561_CONTROL_POWERON, 0);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        uint8_t en;
        i2c_read_reg(DEV_I2C, DEV_ADDR,
                     TSL2561_COMMAND_MODE | TSL2561_REGISTER_CONTROL, &en, 0);
        DEBUG("[Info] Enabled ? %s\n", en == 3 ? "true" : "false");
    }
}

static void _disable(const tsl2561_t *dev)
{
    /* disabling device */
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  TSL2561_COMMAND_MODE | TSL2561_REGISTER_CONTROL,
                  TSL2561_CONTROL_POWEROFF, 0);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        uint8_t dis;
        i2c_read_reg(DEV_I2C, DEV_ADDR,
                     TSL2561_COMMAND_MODE | TSL2561_REGISTER_CONTROL, &dis, 0);
        DEBUG("[Info] Disabled ? %s\n", dis == 0 ? "true": "false");
    }
}

static void _read_data(const tsl2561_t *dev, uint16_t *full, uint16_t *ir)
{
    /* acquire bus */
    i2c_acquire(DEV_I2C);
    /* Enable the device */
    _enable(dev);

    /* Wait integration time in ms for ADC to complete */
    switch (DEV_INTEGRATION) {
        case TSL2561_INTEGRATIONTIME_13MS: /* 13700us */
            ztimer_sleep(ZTIMER_MSEC, 14);
            break;

        case TSL2561_INTEGRATIONTIME_101MS:
            ztimer_sleep(ZTIMER_MSEC, 101);
            break;

        default: /* TSL2561_INTEGRATIONTIME_402MS */
            ztimer_sleep(ZTIMER_MSEC, 402);
            break;
    }

    char buffer[2] = { 0 };
    /* Read full spectrum channel */
    i2c_read_regs(DEV_I2C, DEV_ADDR,
                  TSL2561_COMMAND_MODE | TSL2561_COMMAND_WORD | TSL2561_REGISTER_CHAN0,
                  buffer, 2, 0);
    *full = (buffer[1] << 8) | buffer[0];

    memset(buffer, 0, sizeof(buffer));

    /* Read infrared spectrum channel */
    i2c_read_regs(DEV_I2C, DEV_ADDR,
                  TSL2561_COMMAND_MODE | TSL2561_COMMAND_WORD | TSL2561_REGISTER_CHAN1,
                  buffer, 2, 0);
    *ir = (buffer[1] << 8) | buffer[0];

    /* Turn the device off to save power */
    _disable(dev);
    /* release bus */
    i2c_release(DEV_I2C);
}

static void _print_init_info(const tsl2561_t *dev)
{
    DEBUG("[Info] I2C device: %d\n", DEV_I2C);
    DEBUG("[Info] Address: %d\n", DEV_ADDR);
    switch(DEV_GAIN) {
        case TSL2561_GAIN_1X:
            DEBUG("[Info] Gain: 1X\n");
            break;

        case TSL2561_GAIN_16X:
            DEBUG("[Info] Gain: 16X\n");
            break;

        default:
            DEBUG("[Info] Invalid gain %d\n", DEV_GAIN);
            break;
    }

    switch(DEV_INTEGRATION) {
    case TSL2561_INTEGRATIONTIME_13MS:
        DEBUG("[Info] Integration time: 13ms\n");
        break;
    case TSL2561_INTEGRATIONTIME_101MS:
        DEBUG("[Info] Integration time: 101ms\n");
        break;
    case TSL2561_INTEGRATIONTIME_402MS:
        DEBUG("[Info] Integration time: 402ms\n");
        break;
    case TSL2561_INTEGRATIONTIME_NA:
        DEBUG("[Info] Integration time: n/a\n");
        break;
    default:
        DEBUG("[Info] Invalid integration time %d\n", DEV_INTEGRATION);
        break;
    }
}
