/*
 * Copyright (C) 2019 Mesotic SAS
 *               2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bme680
 * @{
 * @file
 * @brief       Bosch BME680 sensor driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>

#include "bme680.h"
#include "bme680_hal.h"
#include "bme680_params.h"

#include "log.h"

#ifdef MODULE_BME680_I2C
#include "periph/i2c.h"
#endif

#ifdef MODULE_BME680_SPI
#include "periph/spi.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

unsigned int bme680_devs_numof = 0;

bme680_t *bme680_devs[BME680_NUMOF] = { };

int bme680_init(bme680_t *dev, const bme680_params_t *params)
{
    int8_t ret;

    assert(bme680_devs_numof < BME680_NUMOF);
    assert(dev);

    bme680_devs[bme680_devs_numof] = dev;
    BME680_SENSOR(dev).dev_id = bme680_devs_numof++;

    /* store interface parameters in the device for the HAL functions */
    dev->intf = params->intf;

    /* Select device interface and apply needed params */
    if (params->ifsel == BME680_I2C_INTF) {
#ifdef MODULE_BME680_I2C
        BME680_SENSOR(dev).intf = BME680_I2C_INTF;
        BME680_SENSOR(dev).read = bme680_i2c_read_hal;
        BME680_SENSOR(dev).write = bme680_i2c_write_hal;
#else
        LOG_ERROR("[bme680]: module bme680_i2c not enabled\n");
        return BME680_NO_DEV;
#endif
    }
    else {
#ifdef MODULE_BME680_SPI
        BME680_SENSOR(dev).intf = BME680_SPI_INTF;
        BME680_SENSOR(dev).read = bme680_spi_read_hal;
        BME680_SENSOR(dev).write = bme680_spi_write_hal;
        spi_init_cs(SPI_DEV(0), params->intf.spi.nss_pin);
#else
        LOG_ERROR("[bme680]: module bme680_spi not enabled\n");
        return BME680_NO_DEV;
#endif
    }

    BME680_SENSOR(dev).delay_ms = bme680_ms_sleep;

    /* call internal bme680_init from Bosch Sensortech driver */
    ret = bme680_init_internal(&BME680_SENSOR(dev));
    if (ret != 0) {
        DEBUG("[bme680]: Failed to get ID\n");
        return ret;
    }

    /*  retrieve params and set them in bme680_t */
    BME680_SENSOR(dev).tph_sett.os_temp = params->temp_os;
    BME680_SENSOR(dev).tph_sett.os_hum = params->hum_os;
    BME680_SENSOR(dev).tph_sett.os_pres = params->pres_os;

    BME680_SENSOR(dev).tph_sett.filter = params->filter;

    /* Enable gas measurement if needed */
    BME680_SENSOR(dev).gas_sett.run_gas = params->gas_measure;
    /* Create a ramp heat waveform in 3 steps */
    BME680_SENSOR(dev).gas_sett.heatr_temp = params->heater_temp;
    BME680_SENSOR(dev).gas_sett.heatr_dur = params->heater_dur;

    /* Select the intended power mode */
    /* Must be set before writing the sensor configuration */
    BME680_SENSOR(dev).power_mode = BME680_FORCED_MODE;

    /* Set the desired sensor configuration */
    ret = bme680_set_sensor_settings(params->settings, &BME680_SENSOR(dev));
    if (ret != 0) {
        DEBUG("[bme680]: failed to set settings\n");
    }

    return ret;
}

int bme680_force_measurement(bme680_t *dev)
{
    assert(dev);
    BME680_SENSOR(dev).power_mode = BME680_FORCED_MODE;
    return bme680_set_sensor_mode(&BME680_SENSOR(dev));
}

int bme680_get_duration(bme680_t* dev)
{
    assert(dev);

    uint16_t duration;
    bme680_get_profile_dur(&duration, &BME680_SENSOR(dev));
    return duration;
}

int bme680_get_data(bme680_t* dev, bme680_field_data_t *data)
{
    assert(dev);

    int8_t res;
    if ((res = bme680_get_sensor_data(data, &BME680_SENSOR(dev))) == 0) {
        return BME680_OK;
    }

    DEBUG("[bme680]: reading data failed with reason %d\n", res);

    if (res == BME680_W_NO_NEW_DATA) {
        return BME680_NO_NEW_DATA;
    }
    return res;
}

int bme680_set_ambient_temp(bme680_t* dev, int8_t temp)
{
    assert(dev);

    BME680_SENSOR(dev).amb_temp = temp;
    return bme680_set_sensor_settings(BME680_GAS_MEAS_SEL, &BME680_SENSOR(dev));
}
