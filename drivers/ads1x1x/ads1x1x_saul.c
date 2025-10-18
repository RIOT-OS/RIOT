/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ads1x1x
 * @{
 *
 * @file
 * @brief       ADS101x/111x adaption to the RIOT actuator/sensor interface
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "saul.h"
#include "ads1x1x.h"
#include "ads1x1x_internal.h"
#include "ads1x1x_params.h"

static int read_adc(const void *dev, phydat_t *res)
{
    const ads1x1x_t *mydev = dev;

    /* Read raw value */
    int16_t raw;

    if (ads1x1x_read_raw(mydev, &raw) < 0) {
        return ECANCELED;
    }

    *res->val = (int16_t)(ads1x1x_convert_to_mv(mydev, raw));
    res->unit = UNIT_V;
    /* 1V = 1*10^3 mV */
    res->scale = -3;

    return 1;
}

const saul_driver_t ads1x1x_saul_driver = {
    .read = read_adc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ANALOG,
};
