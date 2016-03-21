/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_tcs37727
 * @{
 *
 * @file
 * @brief       tcs37727 adaption to the RIOT actuator/sensor interface
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "init.h"
#include "saul.h"
#include "saul_reg.h"
#include "tcs37727.h"
#include "tcs37727_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TCS37727_NUM    (sizeof(tcs37727_params)/sizeof(tcs37727_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static tcs37727_t tcs37727_devs[TCS37727_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TCS37727_NUM];

static int read(void *dev, phydat_t *res)
{
    static tcs37727_data_t data;

    tcs37727_t *d = (tcs37727_t *)dev;
    tcs37727_read(d, &data);
    DEBUG("tcs37727 R: %5"PRIu32" G: %5"PRIu32" B: %5"PRIu32" C: %5"PRIu32"\n",
           data.red, data.green, data.blue, data.clear);

    res->val[0] =  data.red;
    res->val[1] =  data.green;
    res->val[2] =  data.blue;
    res->unit = UNIT_UNDEF;
    res->scale = 0;
    return 3;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t tcs37727_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_COLOR,
};

int auto_init_tcs37727(void)
{
    for (int i = 0; i < TCS37727_NUM; i++) {
        const tcs37727_params_t *p = &tcs37727_params[i];

        DEBUG("[auto_init_saul] initializing tcs37727\n");
        int res = tcs37727_init(&tcs37727_devs[i], p->i2c, p->addr, p->atime);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return -1;
        }
        if (tcs37727_set_rgbc_active(&tcs37727_devs[i])) {
            DEBUG("[auto_init_saul] measurement start failed\n");
            return -1;
        }
        saul_entries[i].dev = &tcs37727_devs[i];
        saul_entries[i].name = tcs37727_saul_info[i].name;
        saul_entries[i].driver = &tcs37727_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
    return 0;
}

#ifdef MODULE_PREINIT
device_init(auto_init_tcs37727);
#endif
