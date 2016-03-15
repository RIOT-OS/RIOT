/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_mma8652
 * @{
 *
 * @file
 * @brief       MMA8652 adaption to the RIOT actuator/sensor interface
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "init.h"
#include "saul.h"
#include "saul_reg.h"
#include "mma8652.h"
#include "mma8652_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MMA8652_NUM    (sizeof(mma8652_params)/sizeof(mma8652_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static mma8652_t mma8652_devs[MMA8652_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MMA8652_NUM];

static int read(void *dev, phydat_t *res)
{
    mma8652_t *d = (mma8652_t *)dev;
    mma8652_read(d, &res->val[0], &res->val[1], &res->val[2], NULL);
    DEBUG("mma8652_read, in mg: X: %d Y: %d Z: %d\n",
          res->val[0], res->val[1], res->val[2]);
    /* scale to mg (1024 counts/g) */
    res->val[0] = (int)(res->val[0] * 1000) / 1024;
    res->val[1] = (int)(res->val[1] * 1000) / 1024;
    res->val[2] = (int)(res->val[2] * 1000) / 1024;
    res->unit = UNIT_G;
    res->scale = -3;
    return 3;
}

static int write(void *dev, phydat_t *state)
{
    (void) dev;
    (void) state;
    return -ENOTSUP;
}

const saul_driver_t mma8652_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_ACCEL,
};

int auto_init_mma8652(void)
{
    for (int i = 0; i < MMA8652_NUM; i++) {
        const mma8652_params_t *p = &mma8652_params[i];

        DEBUG("[auto_init_saul] initializing mma8652\n");
        int res = mma8652_init(&mma8652_devs[i], p->i2c, p->addr, p->rate, p->scale);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return -1;
        }
        if (mma8652_set_active(&mma8652_devs[i])) {
            DEBUG("[auto_init_saul] measurement start failed\n");
            return -1;
        }
        saul_entries[i].dev = &mma8652_devs[i];
        saul_entries[i].name = mma8652_saul_info[i].name;
        saul_entries[i].driver = &mma8652_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
    return 0;
}

#ifdef MODULE_PREINIT
device_init(auto_init_mma8652);
#endif
