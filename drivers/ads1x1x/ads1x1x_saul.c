/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "saul.h"
#include "ads1x1x.h"
#include "ads1x1x_internal.h"
#include "ads1x1x_params.h"

static int read_adc(const void *dev, phydat_t *res)
{
    const ads1x1x_t *mydev = dev;

    /* Read raw value */
    if (ads1x1x_read_raw(mydev, res->val) < 0) {
        return ECANCELED;
    }

    /* Convert raw ADC value to voltage according to PGA */
    int32_t fsr_mv = _ads1x1x_get_pga_voltage(mydev->params.mux & ADS1X1X_PGA_MASK);
    /* ADS101X_BITS_RES is 12 for ADS101x, 16 for ADS111x */
    *res->val = (int16_t)(((int32_t)*(res->val) * fsr_mv) / (1 << (ADS1X1X_BITS_RES - 1)));

    res->unit  = UNIT_V;
    /* 1V = 1*10^3 mV */
    res->scale = -3;

    return 1;
}


const saul_driver_t ads1x1x_saul_driver = {
    .read = read_adc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ANALOG,
};
