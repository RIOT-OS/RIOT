/*
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ads1115
 * @{
 * @file
 * @brief       ADS1115 Analog-to-digital converter driver
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * @}
 */

#include <assert.h>

#include "ads1115.h"
#include "ads1115_params.h"
#include "ads1115_internal.h"

#include "periph/i2c.h"
#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV (dev->params.i2c)
#define ADDR (dev->params.addr)

#define ADS1115_CONF_TEST_VALUE (1)

static inline int _ads1115_get_pga_voltage(ads1115_pga_t pga)
{
    switch (pga) {
    case ADS1115_PGA_6_144V: return 6144;
    case ADS1115_PGA_4_096V: return 4096;
    case ADS1115_PGA_2_048V: return 2048;
    case ADS1115_PGA_1_024V: return 1024;
    case ADS1115_PGA_0_512V: return 512;
    case ADS1115_PGA_0_256V:
    case ADS1115_PGA_0_256V_B:
    case ADS1115_PGA_0_256V_C:
        return 256;
    default:
        return 0;
    }
}

/**
 * @brief Builds the configuration register value from the parameters
 *
 * @param[in] params  ADS1115 parameters
 *
 * @return Configuration register value
 */
static uint16_t _build_config_reg(const ads1115_params_t *params)
{
    uint16_t conf = 0;

    conf |= (params->mux << ADS1115_CONF_MUX_BIT);
    conf |= (params->pga << ADS1115_CONF_PGA_BIT);
    conf |= (params->mode << ADS1115_CONF_MODE_BIT);
    conf |= (params->dr << ADS1115_CONF_DR_BIT);
    conf |= (params->comp_mode << ADS1115_CONF_COMP_MODE_BIT);
    conf |= (params->comp_polarity << ADS1115_CONF_COMP_POLARITY_BIT);
    conf |= (params->comp_latch << ADS1115_CONF_COMP_LATCH_BIT);
    conf |= (params->comp_queue << ADS1115_CONF_COMP_QUEUE_BIT);

    return conf;
}

int ads1115_init(ads1115_t *dev, const ads1115_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    DEBUG("[ads1115] init - i2c=%d, addr=0x%02x\n", dev->params.i2c, dev->params.addr);

    int res = ADS1115_NOI2C;

    i2c_acquire(DEV);

    /* Test communication */
    uint16_t test_conf = htons(ADS1115_CONF_TEST_VALUE);
    if (i2c_write_regs(DEV, ADDR, ADS1115_REG_CONFIG, &test_conf, sizeof(test_conf), 0) < 0) {
        DEBUG("[ads1115] init - error: write test failed\n");
        res = ADS1115_NODEV;
    }

    uint16_t reg;
    if (i2c_read_regs(DEV, ADDR, ADS1115_REG_CONFIG, &reg, sizeof(reg), 0) < 0 ||
        ntohs(reg) != ADS1115_CONF_TEST_VALUE) {
        DEBUG("[ads1115] init - error: read test failed (reg=%04x)\n", ntohs(reg));
        res = ADS1115_NOI2C;
        goto release;
    }

    /* Apply actual configuration */
    uint16_t conf = htons(_build_config_reg(&dev->params));
    if (i2c_write_regs(DEV, ADDR, ADS1115_REG_CONFIG, &conf, sizeof(conf), 0) < 0) {
        DEBUG("[ads1115] init - error: setting config failed\n");
        res = ADS1115_NOI2C;
        goto release;
    }

    res = ADS1115_OK;

release:
    i2c_release(DEV);
    return res;
}

int ads1115_set_ain_ch_input(ads1115_t *dev, ads1115_mux_t mux)
{
    assert(dev);

    int res = ADS1115_NOI2C;

    i2c_acquire(DEV);

    /* Read current configuration */
    uint16_t reg;
    if (i2c_read_regs(DEV, ADDR, ADS1115_REG_CONFIG, &reg, sizeof(reg), 0) < 0) {
        i2c_release(DEV);
        goto release;
    }

    /* Update MUX bits */
    uint16_t conf = ntohs(reg);
    conf &= ~(0x07 << ADS1115_CONF_MUX_BIT);    /* Clear MUX bits */
    conf |= (mux << ADS1115_CONF_MUX_BIT);      /* Set new MUX */

    /* Write back updated configuration */
    reg = htons(conf);

    if (i2c_write_regs(DEV, ADDR, ADS1115_REG_CONFIG, &reg, sizeof(reg), 0) < 0) {
        goto release;
    }

    res = ADS1115_OK;
    dev->params.mux = mux;

release:
    i2c_release(DEV);
    return res;
}

int ads1115_read_conversion(ads1115_t *dev, uint16_t *value)
{
    assert(dev && value);

    int res = ADS1115_NOI2C;
    uint16_t buf;

    i2c_acquire(DEV);

    /* Read conversion register */
    if (i2c_read_regs(DEV, ADDR, ADS1115_REG_CONVERSION, &buf, sizeof(buf), 0) < 0) {
        goto release;
    }

    /* Combine bytes into a single value */
    *value = ntohs(buf);
    res = ADS1115_OK;

release:
    i2c_release(DEV);
    return res;
}

int ads1115_convert_to_mv(ads1115_t *dev, uint16_t value)
{
    assert(dev);
    return value * _ads1115_get_pga_voltage(dev->params.pga) / (1 << 15); /* Msb is sign bit */
}
