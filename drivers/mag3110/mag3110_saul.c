/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_mag3110
 * @{
 *
 * @file
 * @brief       mag3110 adaption to the RIOT actuator/sensor interface
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "init.h"
#include "saul.h"
#include "saul_reg.h"
#include "mag3110.h"
#include "mag3110_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MAG3110_NUM    (sizeof(mag3110_params)/sizeof(mag3110_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static mag3110_t mag3110_devs[MAG3110_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MAG3110_NUM];

static int read(void *dev, phydat_t *res)
{
    mag3110_t *d = (mag3110_t *)dev;
    mag3110_read(d, &res->val[0], &res->val[1], &res->val[2], NULL);
    DEBUG("mag3110_read, in mg: X: %d Y: %d Z: %d\n",
          res->val[0], res->val[1], res->val[2]);
    res->unit = UNIT_GS;
    res->scale = -3;
    return 3;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t mag3110_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_MAG,
};

int auto_init_mag3110(void)
{
    for (int i = 0; i < MAG3110_NUM; i++) {
        const mag3110_params_t *p = &mag3110_params[i];

        DEBUG("[auto_init_saul] initializing mag3110\n");
        int res = mag3110_init(&mag3110_devs[i], p->i2c, p->addr, p->ratio);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return -1;
        }
        if (mag3110_set_active(&mag3110_devs[i])) {
            DEBUG("[auto_init_saul] measurement start failed\n");
            return -1;
        }
        saul_entries[i].dev = &mag3110_devs[i];
        saul_entries[i].name = mag3110_saul_info[i].name;
        saul_entries[i].driver = &mag3110_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
    return 0;
}

#ifdef MODULE_PREINIT
device_init(auto_init_mag3110);
#endif
