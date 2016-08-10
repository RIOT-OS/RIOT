/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ADXL362 driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifdef MODULE_ADXL362

#include "log.h"
#include "saul_reg.h"

#include "adxl362_params.h"
#include "adxl362.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ADXL362_NUMOF    (sizeof(adxl362_params) / sizeof(adxl362_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static adxl362_t adxl362_devs[ADXL362_NUMOF];

/**
 * @brief   Memory for the SAUL registry temperature sensor entries
 */
static saul_reg_t saul_entries_temp[ADXL362_NUMOF];

/**
 * @brief   Memory for the SAUL registry acceleration sensor entries
 */
static saul_reg_t saul_entries_acc[ADXL362_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t adxl362_temperature_saul_driver;
extern const saul_driver_t adxl362_acceleration_saul_driver;
/** @} */

void auto_init_adxl362(void)
{
    for (unsigned int i = 0; i < ADXL362_NUMOF; i++) {
        int res;
        adxl362_t *dev = &adxl362_devs[i];
        const adxl362_params_t *params = &adxl362_params[i];
        res = spi_init_master(params->spi, SPI_CONF_FIRST_RISING, SPI_SPEED_1MHZ);
        if (res < 0) {
            LOG_ERROR("Unable to initialize SPI bus #%u\n", dev->spi);
            continue;
        }
        res = adxl362_init(dev, params->spi, params->cs);
        if (res < 0) {
            LOG_ERROR("Unable to initialize ADXL362 sensor #%u\n", i);
            continue;
        }
        res = adxl362_set_filter(dev, params->range, params->odr, 0);
        if (res < 0) {
            LOG_ERROR("Unable to set filter parameters on ADXL362 sensor #%u\n", i);
            continue;
        }
        /* acceleration */
        saul_entries_acc[i].dev = &adxl362_devs[i];
        saul_entries_acc[i].name = adxl362_saul_reg_info[i].name;
        saul_entries_acc[i].driver = &adxl362_acceleration_saul_driver;
        saul_reg_add(&saul_entries_acc[i]);

        /* temperature ADC */
        saul_entries_temp[i].dev = &adxl362_devs[i];
        saul_entries_temp[i].name = adxl362_saul_reg_info[i + 1].name;
        saul_entries_temp[i].driver = &adxl362_temperature_saul_driver;
        saul_reg_add(&saul_entries_temp[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ADXL362 */
