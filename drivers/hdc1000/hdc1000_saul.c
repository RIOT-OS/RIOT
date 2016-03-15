/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_hdc1000
 * @{
 *
 * @file
 * @brief       hdc1000 adaption to the RIOT actuator/sensor interface
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "init.h"
#include "saul.h"
#include "saul_reg.h"
#include "hdc1000.h"
#include "hdc1000_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HDC1000_NUM    (sizeof(hdc1000_params)/sizeof(hdc1000_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static hdc1000_t hdc1000_devs[HDC1000_NUM];

/**
 * @brief   Memory for the SAUL registry entries, humidity
 */
static saul_reg_t saul_entries_h[HDC1000_NUM];

/**
 * @brief   Memory for the SAUL registry entries, temperature
 */
static saul_reg_t saul_entries_t[HDC1000_NUM];

static uint16_t rawtemp, rawhum;
static int temp, hum;

static int read_h(void *dev, phydat_t *res)
{
    hdc1000_t *d = (hdc1000_t *)dev;

    hdc1000_read(d, &rawtemp, &rawhum);
    hdc1000_convert(rawtemp, rawhum,  &temp, &hum);
    DEBUG("Data T: %d   RH: %d\n", temp, hum);

    res->val[0] = (uint16_t)(hum);
    res->unit = UNIT_PERCENT;
    res->scale = -2;
    hdc1000_startmeasure(d);
    return 1;
}

static int read_t(void *dev, phydat_t *res)
{
    (void)dev;

    res->val[0] = (uint16_t)(temp);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t hdc1000h_saul_driver = {
    .read = read_h,
    .write = write,
    .type = SAUL_SENSE_HUM,
};

const saul_driver_t hdc1000t_saul_driver = {
    .read = read_t,
    .write = write,
    .type = SAUL_SENSE_TEMP,
};

int auto_init_hdc1000(void)
{
    for (int i = 0; i < HDC1000_NUM; i++) {
        const hdc1000_params_t *p = &hdc1000_params[i];

        DEBUG("[auto_init_saul] initializing hdc1000\n");
        int res = hdc1000_init(&hdc1000_devs[i], p->i2c, p->addr);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return -1;
        }
        if (hdc1000_startmeasure(&hdc1000_devs[i])) {
            DEBUG("[auto_init_saul] measurement start failed\n");
            return -1;
        }
        saul_entries_h[i].dev = &hdc1000_devs[i];
        saul_entries_h[i].name = hdc1000_saul_info[i].name;
        saul_entries_h[i].driver = &hdc1000h_saul_driver;
        saul_reg_add(&(saul_entries_h[i]));

        saul_entries_t[i].dev = &hdc1000_devs[i];
        saul_entries_t[i].name = hdc1000_saul_info[i].name;
        saul_entries_t[i].driver = &hdc1000t_saul_driver;
        saul_reg_add(&(saul_entries_t[i]));
    }
    return 0;
}

#ifdef MODULE_PREINIT
device_init(auto_init_hdc1000);
#endif
