/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_gp8xxx
 * @{
 *
 * @file
 * @brief       SAUL adoption for the Guestgood GP8xxx I2C DACs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <math.h>

#include "saul.h"

#include "gp8xxx.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uint16_t convert(uint16_t value, int scale)
{
    if (scale > 0) {
        return value / pow(10, scale);
    }
    else if (scale < 0) {
        return value * pow(10, -scale);
    }

    return value;
}

#if GP8XXX_HAS_VDAC
static int set_voltage(const void *dev, const phydat_t *data)
{
    gp8xxx_saul_t *saul_dev = (gp8xxx_saul_t *)dev;

    DEBUG("[gp8xxx_saul] set_voltage value=%d scale=%d\n", data->val[0], data->scale);

    /* convert value to mV */
    uint16_t value = convert(data->val[0], -3 - data->scale);

    /* write voltage*/
    int res = gp8xxx_set_voltage(saul_dev->dev, saul_dev->channel, value);

    if (res != 0) {
        return -ECANCELED;
    }

    return 1;
}
#endif

#if GP8XXX_HAS_IDAC
static int set_current(const void *dev, const phydat_t *data)
{
    gp8xxx_saul_t *saul_dev = (gp8xxx_saul_t *)dev;

    DEBUG("[gp8xxx_saul] set_current value=%d scale=%d\n", data->val[0], data->scale);

    /* convert value to uA */
    uint16_t value = convert(data->val[0], -6 - data->scale);

    /* write current */
    int res = gp8xxx_set_current(saul_dev->dev, saul_dev->channel, value);

    if (res != 0) {
        return -ECANCELED;
    }

    return 1;
}
#endif

#if GP8XXX_HAS_VDAC
const saul_driver_t gp8xxx_voltage_saul_driver = {
    .read = saul_read_notsup,
    .write = set_voltage,
    .type = SAUL_ACT_VOLTAGE,
};
#endif

#if GP8XXX_HAS_IDAC
const saul_driver_t gp8xxx_current_saul_driver = {
    .read = saul_read_notsup,
    .write = set_current,
    .type = SAUL_ACT_CURRENT,
};
#endif
