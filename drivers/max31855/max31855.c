/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_max31855
 * @{
 *
 * @file
 * @brief       Device driver implementation for the drivers_sensors
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "byteorder.h"
#include "log.h"

#include "max31855.h"
#include "max31855_constants.h"
#include "max31855_params.h"

void _raw_data_to_thermocouple_temperature(uint32_t raw_data, int32_t *temperature)
{
    assert(temperature);

    int32_t is_negative = raw_data & MAX31855_THERMOCOUPLE_SIGN_MASK;

    if (is_negative) {
        raw_data = ~raw_data + 1;
    }

    *temperature = (raw_data & MAX31855_THERMOCOUPLE_INTEGER_MASK) >>
                        MAX31855_THERMOCOUPLE_INTEGER_SHIFT;

    /* convert to centi degC */
    *temperature = *temperature * 100;

    /* add fractional parts */
    *temperature += (raw_data & MAX31855_THERMOCOUPLE_FRACTIONAL_HALF_MASK) ? 50 : 0;
    *temperature += (raw_data & MAX31855_THERMOCOUPLE_FRACTIONAL_QUARTER_MASK) ? 25 : 0;

    if (is_negative) {
        *temperature *= -1;
    }
}

void _raw_data_to_internal_temperature(uint32_t raw_data, int32_t *temperature)
{
    assert(temperature);

    int32_t is_negative = raw_data & MAX31855_INTERNAL_SIGN_MASK;

    if (is_negative) {
        raw_data = ~raw_data + 1;
    }

    *temperature = (raw_data & MAX31855_INTERNAL_INTEGER_MASK) >> MAX31855_INTERNAL_INTEGER_SHIFT;

    /* convert to centi degC */
    *temperature = *temperature * 100;

    /* add fractional parts */
    *temperature += (raw_data & MAX31855_INTERNAL_FRACTIONAL_HALF_MASK) ? 50 : 0;
    *temperature += (raw_data & MAX31855_INTERNAL_FRACTIONAL_QUARTER_MASK) ? 25 : 0;
    *temperature += (raw_data & MAX31855_INTERNAL_FRACTIONAL_EIGHTH_MASK) ? 12 : 0;
    *temperature += (raw_data & MAX31855_INTERNAL_FRACTIONAL_SIXTEENTH_MASK) ? 6 : 0;

    if (is_negative) {
        *temperature *= -1;
    }
}

void _raw_data_to_fault(uint32_t raw_data, max31855_fault_t *fault)
{
    assert(fault);

    switch (raw_data & MAX31855_FAULT_MASK) {
        case MAX31855_FAULT_VCC_SHORT_MASK:
            *fault = MAX31855_FAULT_VCC_SHORT;
            break;
        case MAX31855_FAULT_GND_SHORT_MASK:
            *fault = MAX31855_FAULT_GND_SHORT;
            break;
        case MAX31855_FAULT_OPEN_CIRCUIT_MASK:
            *fault = MAX31855_FAULT_OPEN_CIRCUIT;
            break;
        default:
            *fault = MAX31855_FAULT_NO_FAULT;
            break;
    }
}

int max31855_init(max31855_t *dev, const max31855_params_t *params)
{
    assert(dev);
    assert(params);
    dev->params = params;

    int ret = spi_init_cs(dev->params->spi, dev->params->cs_pin);

    if (ret < 0) {
        LOG_ERROR("Failed to initialize MAX31855\n");
        return ret;
    }

    return 0;
}

void max31855_read_raw(max31855_t *dev, uint32_t *data)
{
    assert(dev);
    assert(data);

    uint8_t buffer[MAX31855_DATA_SIZE] = { 0 };
    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_0, SPI_CLK_5MHZ);
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, NULL, buffer, sizeof(buffer));
    spi_release(dev->params->spi);

    *data = byteorder_bebuftohl(buffer);
}

void max31855_raw_to_data(uint32_t raw_data, max31855_data_t *data)
{
    assert(data);

    _raw_data_to_thermocouple_temperature(raw_data, &data->thermocouple_temperature);
    _raw_data_to_internal_temperature(raw_data, &data->internal_temperature);
    _raw_data_to_fault(raw_data, &data->fault);
}

int max31855_read(max31855_t *dev, max31855_data_t *data)
{
    assert(dev && data);

    uint32_t raw_data;
    max31855_read_raw(dev, &raw_data);
    max31855_raw_to_data(raw_data, data);

    if (data->fault != MAX31855_FAULT_NO_FAULT) {
        LOG_ERROR("MAX31855 fault: %d\n", data->fault);
        return -EIO;
    }

    return 0;
}
