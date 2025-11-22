/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ads101x
 * @{
 *
 * @file
 * @brief       ADS101x/111x adaption to the RIOT actuator/sensor interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "ads101x.h"
#include "ads101x_regs.h"

/* LSB conversion to power of 10
 * (5/8) * 2^16 = 40960 */
#define CONV_TO_B10    (40960L)

static int read_adc(const void *dev, phydat_t *res)
{
    const ads101x_t *mydev = dev;

    /* Change the mux channel */
    ads101x_set_mux_gain(mydev, mydev->params.mux_gain);

    /* Read raw value */
    if (ads101x_read_raw(mydev, res->val) < 0) {
        return ECANCELED;
    }

    /* Special case for 2.048V */
    /* (this is the fixed FSR of ADS1013 and ADS1113) */
    if ((mydev->params.mux_gain & ADS101X_PGA_MASK)
        == ADS101X_PGA_FSR_2V048) {

        /* LSB == 62.5uV to LSB == 100uV */
        *(res->val) = (int16_t)((CONV_TO_B10 * (int32_t)*(res->val)) >> 16);

        /* 100uV == 2^-4 V */
        res->unit = UNIT_V;
        res->scale = -4;
    }
    else {
        /* Otherwise let the user deal with it */
        res->unit = UNIT_NONE;
        res->scale = 0;
    }

    return 1;
}

const saul_driver_t ads101x_saul_driver = {
    .read = read_adc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ANALOG,
};
