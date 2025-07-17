
/*
 * Copyright (C) Baptiste Le Duc <baptiste.leduc38@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV (dev->params.i2c)
#define ADDR (dev->params.addr)

#define ADS1115_CONF_TEST_VALUE (1)


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

    // Test communication
    uint8_t test_conf[2] = {0x00, ADS1115_CONF_TEST_VALUE};
    if (i2c_write_regs(DEV, ADDR, ADS1115_REG_CONFIG, test_conf, 2, 0) < 0) {
        DEBUG("[ads1115] init - error: write test failed\n");
        res = ADS1115_NODEV;
    }

    uint8_t reg[2];
    if (i2c_read_regs(DEV, ADDR, ADS1115_REG_CONFIG, reg, 2, 0) < 0 ||
        (((uint16_t)reg[0] << 8) | reg[1]) != ADS1115_CONF_TEST_VALUE) {
        DEBUG("[ads1115] init - error: read test failed (reg=%02x%02x)\n", reg[0], reg[1]);
        res = ADS1115_NOI2C;
        goto release;
    }

    // Apply actual configuration
    uint16_t conf = _build_config_reg(&dev->params);
    uint8_t conf_bytes[2] = { conf >> 8, conf & 0xFF };

    if (i2c_write_regs(DEV, ADDR, ADS1115_REG_CONFIG, conf_bytes, 2, 0) < 0) {
        DEBUG("[ads1115] init - error: setting config failed\n");
        res = ADS1115_NOI2C;
        goto release;
    }

    res = ADS1115_OK;

release:
    i2c_release(DEV);
    return res;
}


