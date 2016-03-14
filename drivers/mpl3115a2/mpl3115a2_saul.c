/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_mpl3115a2
 * @{
 *
 * @file
 * @brief       mpl3115a2 adaption to the RIOT actuator/sensor interface
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "init.h"
#include "saul.h"
#include "saul_reg.h"
#include "mpl3115a2.h"
#include "mpl3115a2_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MPL3115A2_NUM    (sizeof(mpl3115a2_params)/sizeof(mpl3115a2_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static mpl3115a2_t mpl3115a2_devs[MPL3115A2_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MPL3115A2_NUM];

static int read(void *dev, phydat_t *res)
{
    uint32_t pressure;
    mpl3115a2_t *d = (mpl3115a2_t *)dev;
    mpl3115a2_read_pressure(d, &pressure, NULL);
    DEBUG("mpl3115a2_read_pressure : %u \n", (unsigned int)pressure);
    res->val[0] = (uint16_t)(pressure/10);
    res->unit = UNIT_PA;
    res->scale = 1;
    return 1;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t mpl3115a2_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_PRESS,
};

int auto_init_mpl3115a2(void)
{
    for (int i = 0; i < MPL3115A2_NUM; i++) {
        const mpl3115a2_params_t *p = &mpl3115a2_params[i];

        DEBUG("[auto_init_saul] initializing mpl3115a2\n");
        int res = mpl3115a2_init(&mpl3115a2_devs[i], p->i2c, p->addr, p->ratio);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return -1;
        }
        if (mpl3115a2_set_active(&mpl3115a2_devs[i])) {
            DEBUG("[auto_init_saul] measurement start failed\n");
            return -1;
        }
        saul_entries[i].dev = (&mpl3115a2_devs[i]);
        saul_entries[i].name = mpl3115a2_saul_info[i].name;
        saul_entries[i].driver = &mpl3115a2_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
    return 0;
}

#ifdef MODULE_PREINIT
device_init(auto_init_mpl3115a2);
#endif
