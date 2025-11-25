/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_apds99xx
 * @brief       APDS99XX adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "apds99xx.h"

static int read_prx(const void *dev, phydat_t *res)
{
    if (apds99xx_read_prx_raw((const apds99xx_t *)dev, (uint16_t*)res) == 0) {
        res->unit = UNIT_CTS;
        res->scale = 0;
        return 1;
    }
    return -ECANCELED;
}

static int read_als(const void *dev, phydat_t *res)
{
    if (apds99xx_read_als_raw((const apds99xx_t *)dev, (uint16_t*)res) == 0) {
        res->unit = UNIT_CTS;
        res->scale = 0;
        return 1;
    }
    return -ECANCELED;
}

#if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930
static int read_lux(const void *dev, phydat_t *res)
{
    if (apds99xx_read_illuminance((const apds99xx_t *)dev, (uint16_t*)res) == 0) {
        res->unit = UNIT_LUX;
        res->scale = 0;
        return 1;
    }
    return -ECANCELED;
}
#endif

#if MODULE_APDS9950 || MODULE_APDS9960
static int read_rgb(const void *dev, phydat_t *res)
{
    apds99xx_rgb_t rgb;
    if (apds99xx_read_rgb_raw((const apds99xx_t *)dev, &rgb) == 0) {
        res->val[0] = (int16_t)rgb.red;
        res->val[1] = (int16_t)rgb.green;
        res->val[2] = (int16_t)rgb.blue;
        res->unit = UNIT_CTS;
        res->scale = 0;
        return 3;
    }
    return -ECANCELED;
}
#endif

const saul_driver_t apds99xx_saul_prx_driver = {
    .read = read_prx,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PROXIMITY,
};

const saul_driver_t apds99xx_saul_als_driver = {
    .read = read_als,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT,
};

#if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930
const saul_driver_t apds99xx_saul_lux_driver = {
    .read = read_lux,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT,
};
#endif

#if MODULE_APDS9950 || MODULE_APDS9960
const saul_driver_t apds99xx_saul_rgb_driver = {
    .read = read_rgb,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT,
};
#endif
