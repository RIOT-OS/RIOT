/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mcp47xx
 * @brief       Device driver for the Microchip MCP47xx DAC with I2C interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "mcp47xx.h"

#include "irq.h"
#include "log.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    do { \
        if (!(cond)) { \
            DEBUG("[mcp47xx] %s: %s\n", \
                  __func__, "parameter condition (" #cond ") not fulfilled"); \
            assert(cond); \
        } \
    } while (0)

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[mcp47xx] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond)
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[mcp47xx] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, d->params.addr, ## __VA_ARGS__)

/** Forward declaration of functions for internal use */

static int _get(mcp47xx_t *dev, uint8_t chn, uint16_t* value);
static int _set(mcp47xx_t *dev, uint8_t chn, uint16_t value, bool pd);
static int _read(const mcp47xx_t *dev, uint8_t *data, size_t len);
static int _write(const mcp47xx_t *dev, uint8_t* data, size_t len);

static const uint8_t _mcp47xx_chn_nums[] = {
    MCP4706_CHN_NUM,
    MCP4716_CHN_NUM,
    MCP4725_CHN_NUM,
    MCP4726_CHN_NUM,
    MCP4728_CHN_NUM
};

int mcp47xx_init(mcp47xx_t *dev, const mcp47xx_params_t *params)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

    DEBUG_DEV("params=%p", dev, params);

    ASSERT_PARAM(params->gain <= MCP47XX_GAIN_2X);
    ASSERT_PARAM(params->vref <= MCP47XX_VREF_BUF);
    ASSERT_PARAM(params->pd_mode <= MCP47XX_PD_LARGE);
    ASSERT_PARAM(params->pd_mode != MCP47XX_NORMAL);

    /* MCP4725 supports only MCP47XX_GAIN_1X */
    ASSERT_PARAM(params->variant != MCP4725 || params->gain == MCP47XX_GAIN_1X);
    /* MCP4725 supports only MCP47XX_VDD */
    ASSERT_PARAM(params->variant != MCP4725 || params->vref == MCP47XX_VREF_VDD);

    /* MCP47XX_INT is only supported by MCP4728 */
    ASSERT_PARAM(params->vref != MCP47XX_VREF_INT || params->variant == MCP4728);
    /* MCP47XX_PIN is only supported by MCP47x6 */
    ASSERT_PARAM((params->vref != MCP47XX_VREF_PIN &&
                  params->vref != MCP47XX_VREF_BUF) || params->variant == MCP4706
                                                    || params->variant == MCP4716
                                                    || params->variant == MCP4726);
    /* init sensor data structure */
    dev->params = *params;

    /* check for availability */
    uint8_t bytes[3];
    if (_read(dev, bytes, 3) != MCP47XX_OK) {
        DEBUG_DEV("device not available", dev);
        return MCP47XX_ERROR_NOT_AVAIL;
    }

    /* power_off all channels */
    for (unsigned i = 0; i < _mcp47xx_chn_nums[dev->params.variant]; i++) {
        mcp47xx_dac_poweroff(dev, i);
    }

    return MCP47XX_OK;
}

int mcp47xx_dac_init(mcp47xx_t *dev, uint8_t chn)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(chn < _mcp47xx_chn_nums[dev->params.variant]);

    DEBUG_DEV("chn=%u", dev, chn);

    /* get the current value */
    uint16_t value;
    int res;
    if ((res = _get(dev, chn, &value))) {
        return res;
    }

    /* set the channel value */
    if ((res = _set(dev, chn, 0, false))) {
        return res;
    }

    return MCP47XX_OK;
}

void mcp47xx_dac_set(mcp47xx_t *dev, uint8_t chn, uint16_t value)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(chn < _mcp47xx_chn_nums[dev->params.variant]);

    DEBUG_DEV("chn=%u val=%u", dev, chn, value);

    dev->values[chn] = value;

    _set(dev, chn, value, false);
}

void mcp47xx_dac_get(mcp47xx_t *dev, uint8_t chn, uint16_t *value)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(value != NULL);
    ASSERT_PARAM(chn < _mcp47xx_chn_nums[dev->params.variant]);

    DEBUG_DEV("chn=%u val=%p", dev, chn, value);

    _get(dev, chn, value);

    dev->values[chn] = *value;
}

void mcp47xx_dac_poweroff(mcp47xx_t *dev, uint8_t chn)
{
    _set(dev, chn, dev->values[chn], true);
}

void mcp47xx_dac_poweron(mcp47xx_t *dev, uint8_t chn)
{
    _set(dev, chn, dev->values[chn], false);
}

uint8_t mcp47xx_dac_channels(mcp47xx_t *dev)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);

    return _mcp47xx_chn_nums[dev->params.variant];
}

/** Functions for internal use only */

/* Write DAC Register/Volatile Memory command */
#define MCP47XX_CMD_WRITE_DAC    (0x40)

static int _get(mcp47xx_t *dev, uint8_t chn, uint16_t* value)
{
    /*
     * read formats:
     *
     * MCP4706  BR0VVPPG DDDDDDDD --------   V - Voltage selection VREF1,VREF0
     * MCP4716  BR0VVPPG DDDDDDDD DD------   P - Power Mode seclection PD1,PD0
     * MCP4726  BR0VVPPG DDDDDDDD DDDD----   G - Gain selection
     * MCP4725  BR---PP- DDDDDDDD DDDD----   D - Data from MSB to LSB
     * MCP4728  BRCC0AAA VPPGDDDD DDDDDDDD   C - Channel selection CH1,CH0
     *                                       B - BSY/RDY EEPROM Write status
     *                                       R - Power on Reset
     */
    int res;

    if (dev->params.variant == MCP4728) {
        uint8_t bytes[24];
        res = _read(dev, bytes, 24);
        if (res) {
            return res;
        }
        *value  = ((uint16_t)bytes[chn * 6 + 1] & 0x000f) << 12;
        *value |= bytes[chn * 6 + 2] << 4;
    }
    else {
        uint8_t bytes[3];
        res = _read(dev, bytes, 3);
        if (res) {
            return res;
        }
        DEBUG_DEV("read %02x %02x %02x", dev, bytes[0], bytes[1], bytes[2]);
        *value = ((uint16_t)bytes[1] << 8) | bytes[2];
    }

    return MCP47XX_OK;
}

static int _set(mcp47xx_t *dev, uint8_t chn, uint16_t value, bool pd)
{
    /*
     * write command formats:
     *
     * MCP4706  010VVPPG DDDDDDDD --------  V - Voltage selection VREF1,VREF0
     * MCP4716  010VVPPG DDDDDDDD DD------  P - Power Mode seclection PD1,PD0
     * MCP4726  010VVPPG DDDDDDDD DDDD----  G - Gain selection
     * MCP4725  010--PP- DDDDDDDD DDDD----  D - Data from MSB to LSB
     * MCP4728  01000CCU VPPGDDDD DDDDDDDD  C - Channel selection CH1,CH0
     *                                      U - UDAC bit
     */
    uint8_t bytes[3] = { };

    if (dev->params.variant == MCP4728) {
        /* U=0 update V_OUT directly */
        bytes[0] = MCP47XX_CMD_WRITE_DAC | (chn << 1);
        bytes[1] = (value >> 12) | (dev->params.vref << 7)
                                 | (dev->params.gain << 4)
                                 | (pd ? dev->params.pd_mode << 5 : 0);
        bytes[2] = (value >> 4) & 0xff;
    }
    else {
        bytes[0] = MCP47XX_CMD_WRITE_DAC | (dev->params.vref << 3)
                                         | dev->params.gain
                                         | (pd ? dev->params.pd_mode << 1 : 0);
        /*
         * resolution handling is not required since only the n most
         * significant bits are used
         */
        bytes[1] = value >> 8;
        bytes[2] = value & 0xff;
    }
    DEBUG_DEV("write %02x %02x %02x", dev, bytes[0], bytes[1], bytes[2]);

    return _write(dev, bytes, 3);
}

static int _read(const mcp47xx_t *dev, uint8_t *data, size_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);

    DEBUG_DEV("", dev);

    i2c_acquire(dev->params.dev);
    int res = i2c_read_bytes(dev->params.dev, dev->params.addr, data, len, 0);
    i2c_release(dev->params.dev);

    if (res != 0) {
        DEBUG_DEV("could not read data, reason %d (%s)",
                  dev, res, strerror(res * -1));
        return -MCP47XX_ERROR_I2C;
    }

    return res;
}

static int _write(const mcp47xx_t *dev, uint8_t* data, size_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);

    DEBUG_DEV("", dev);

    i2c_acquire(dev->params.dev);
    int res = i2c_write_bytes(dev->params.dev, dev->params.addr, data, len, 0);
    i2c_release(dev->params.dev);

    if (res != 0) {
        DEBUG_DEV("could not write data, reason %d (%s)",
                  dev, res, strerror(res * -1));
       return -MCP47XX_ERROR_I2C;
    }

    return res;
}
