/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_amg88xx
 * @{
 *
 * @file
 * @brief       SAUL adoption for the AMG88xx infrared array sensor
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "saul.h"

#include "amg88xx.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    uint16_t temp;

    if (amg88xx_get_temperature((const amg88xx_t *)dev, (int16_t *)&temp) != 0) {
        return -ECANCELED;
    }

    res->val[0] = (int32_t)temp;
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

const saul_driver_t amg88xx_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
