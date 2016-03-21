/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_tmp006
 * @{
 *
 * @file
 * @brief       tmp006 adaption to the RIOT actuator/sensor interface
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "init.h"
#include "saul.h"
#include "saul_reg.h"
#include "tmp006.h"
#include "tmp006_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TMP006_NUM    (sizeof(tmp006_params)/sizeof(tmp006_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static tmp006_t tmp006_devs[TMP006_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TMP006_NUM];

static int read(void *dev, phydat_t *res)
{
    int16_t rawtemp, rawvolt;
    float tamb, tobj;
    uint8_t drdy;

    tmp006_t *d = (tmp006_t *)dev;
    tmp006_read(d, &rawvolt, &rawtemp, &drdy);

    if (!drdy) {
        return -ECANCELED;
    }

    tmp006_convert(rawvolt, rawtemp,  &tamb, &tobj);
    res->val[0] = (int)(tobj*100);
    res->val[1] = (int)(tamb*100);
    DEBUG("Data Tabm: %d   Tobj: %d\n", (int)(tamb*100), (int)(tobj*100));

    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 2;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t tmp006_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_TEMP,
};

int auto_init_tmp006(void)
{
    for (int i = 0; i < TMP006_NUM; i++) {
        const tmp006_params_t *p = &tmp006_params[i];

        DEBUG("[auto_init_saul] initializing tmp006\n");
        int res = tmp006_init(&tmp006_devs[i], p->i2c, p->addr, p->ratio);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return -1;
        }
        if (tmp006_set_active(&tmp006_devs[i])) {
            DEBUG("[auto_init_saul] measurement start failed\n");
            return -1;
        }
        saul_entries[i].dev = (&tmp006_devs[i]);
        saul_entries[i].name = tmp006_saul_info[i].name;
        saul_entries[i].driver = &tmp006_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
    return 0;
}

#ifdef MODULE_PREINIT
device_init(auto_init_tmp006);
#endif
