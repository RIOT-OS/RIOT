/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       Implementation of the temperature peripheral internal driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "saul.h"
#include "saul_reg.h"
#include "phydat.h"

void temperature_read(int16_t *temp)
{
    /* Start temperature measurement task */
    NRF_TEMP->TASKS_START = 1;

    /* Wait for temperature measurement to be ready */
    while (!NRF_TEMP->EVENTS_DATARDY); /* takes 36us according to manual */

    /* temperature is in 0.25°C step, so just divide by 4 */
    *temp = (int16_t)NRF_TEMP->TEMP >> 2;

    /* Clear data ready bit and stop temperature measurement task */
    NRF_TEMP->EVENTS_DATARDY = 0;
    NRF_TEMP->TASKS_STOP = 1;
}

static int _read_temperature(const void *dev, phydat_t *res)
{
    (void) dev;
    temperature_read(&res->val[0]);
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_TEMP_C;
    res->scale = 0;
    return 1;
}

const saul_reg_info_t nrf_temperature_saul_info = {
    .name = "NRF_TEMP"
};

const saul_driver_t nrf_temperature_saul_driver = {
    .read = _read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
