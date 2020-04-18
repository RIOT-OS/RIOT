/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_si70xx
 * @{
 *
 * @file
 * @brief       Implementation of Si7006/13/20/21 sensor driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <string.h>
#include "xtimer.h"

#include "si70xx_internals.h"
#include "si70xx.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SI70XX_I2C     (dev->params.i2c_dev)
#define SI70XX_ADDR    (dev->params.address)

/**
 * @brief   Internal helper function to perform and reconstruct a measurement.
 */
static uint16_t _do_measure(const si70xx_t *dev, uint8_t command)
{
    uint8_t result[2];

    i2c_acquire(SI70XX_I2C);

    if (i2c_write_byte(SI70XX_I2C, SI70XX_ADDR, command, 0) != 0) {
        DEBUG("[ERROR] Cannot write command '%d' to I2C.\n", command);
    }

    if (i2c_read_bytes(SI70XX_I2C, SI70XX_ADDR, result, 2, 0) != 0) {
        DEBUG("[ERROR] Cannot read command '%d' result from I2C.\n", command);
    }

    i2c_release(SI70XX_I2C);

    /* reconstruct raw result */
    return ((uint16_t)result[0] << 8) + (result[1] & 0xfc);
}

uint64_t si70xx_get_serial(const si70xx_t *dev)
{
    uint8_t out[2];
    uint8_t in_first[8] = { 0 };
    uint8_t in_second[8] = { 0 };

    /* read the lower bytes */
    out[0] = SI70XX_READ_ID_FIRST_A;
    out[1] = SI70XX_READ_ID_FIRST_B;

    if (i2c_write_bytes(SI70XX_I2C, SI70XX_ADDR, out, 2, 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'READ_ID_FIRST' to I2C.\n");
    }

    if (i2c_read_bytes(SI70XX_I2C, SI70XX_ADDR, in_first, 8, 0) != 0) {
        DEBUG("[ERROR] Cannot read device first ID from I2C.\n");
    }

    /* read the higher bytes */
    out[0] = SI70XX_READ_ID_SECOND_A;
    out[1] = SI70XX_READ_ID_SECOND_B;

    if (i2c_write_bytes(SI70XX_I2C, SI70XX_ADDR, out, 2, 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'READ_ID_SECOND' to I2C.\n");
    }

    if (i2c_read_bytes(SI70XX_I2C, SI70XX_ADDR, in_second, 8, 0) != 0) {
        DEBUG("[ERROR] Cannot read device second ID from I2C.\n");
    }

    /* calculate the ID */
    uint32_t id_first = ((uint32_t)in_first[0] << 24) + ((uint32_t)in_first[2] << 16) +
                        (in_first[4] << 8) + (in_first[6] << 0);
    uint32_t id_second = ((uint32_t)in_second[0] << 24) + ((uint32_t)in_second[2] << 16) +
                         (in_second[4] << 8) + (in_second[6] << 0);

    return (((uint64_t) id_first) << 32) + id_second;
}

uint8_t si70xx_get_id(const si70xx_t *dev)
{
    return (si70xx_get_serial(dev) >> 24) & 0xff;
}

uint8_t si70xx_get_revision(const si70xx_t *dev)
{
    uint8_t out[2];
    uint8_t in = 0;

    /* read the revision number */
    out[0] = SI70XX_READ_REVISION_A;
    out[1] = SI70XX_READ_REVISION_B;

    if (i2c_write_bytes(SI70XX_I2C, SI70XX_ADDR, out, 2, 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'READ_REVISION' to I2C.\n");
    }

    if (i2c_read_byte(SI70XX_I2C, SI70XX_ADDR, &in, 0) != 0) {
        DEBUG("[ERROR] Cannot read device revision from I2C.\n");
    }

    return in;
}

static int _test_device(const si70xx_t *dev)
{
    uint8_t revision = si70xx_get_revision(dev);

    if (revision != SI70XX_REVISION_1 && revision != SI70XX_REVISION_2) {
        DEBUG("[ERROR] Bad device revision (%d).\n", revision);
        return -SI70XX_ERR_NODEV;
    }

    uint8_t id = si70xx_get_id(dev);

    const bool valid_id = (id == SI70XX_ID);
    if (!valid_id) {
        DEBUG("[ERROR] Not a valid Si7006/13/20/21/5x device: %u\n",
              (unsigned)id);
        return -SI70XX_ERR_NODEV;;
    }

    return SI70XX_OK;
}

int si70xx_init(si70xx_t *dev, const si70xx_params_t *params)
{
    /* initialize the device descriptor */
    dev->params = *params;

    /* setup the i2c bus */
    i2c_acquire(SI70XX_I2C);

    if (_test_device(dev) != SI70XX_OK) {
        DEBUG("[ERROR] No valid device found.\n");
        i2c_release(SI70XX_I2C);
        return SI70XX_ERR_NODEV;
    }

    /* initialize the peripheral */
    if (i2c_write_byte(SI70XX_I2C, SI70XX_ADDR, SI70XX_RESET, 0) != 0) {
        DEBUG("[ERROR] Cannot reset device.\n");
        i2c_release(SI70XX_I2C);
        return SI70XX_ERR_I2C;
    }

    i2c_release(SI70XX_I2C);

    /* sensor is ready after at most 25 ms */
    xtimer_msleep(25);

    DEBUG("[DEBUG] Device initialized with success.\n");
    return SI70XX_OK;
}

uint16_t si70xx_get_relative_humidity(const si70xx_t *dev)
{
    uint16_t raw;
    int32_t humidity;

    /* perform measurement */
    raw = _do_measure(dev, SI70XX_MEASURE_RH_HOLD);

    humidity = ((12500 * raw) / 65536) - 600;

    /* according to datasheet, values may exceed bounds, but can be clipped */
    if (humidity < 0) {
        return 0;
    }
    else if (humidity > 10000) {
        return 10000;
    }
    else {
        return (uint16_t) humidity;
    }
}

int16_t si70xx_get_temperature(const si70xx_t *dev)
{
    uint16_t raw;

    /* perform measurement */
    raw = _do_measure(dev, SI70XX_MEASURE_TEMP_HOLD);

    return ((17572 * raw) / 65536) - 4685;
}

void si70xx_get_both(const si70xx_t *dev, uint16_t *humidity, int16_t *temperature)
{
    uint16_t raw;

    /* read the humidity the normal way */
    *humidity = si70xx_get_relative_humidity(dev);

    /* read the temperature using the data from the previous measurement */
    raw = _do_measure(dev, SI70XX_MEASURE_TEMP_PREV);

    *temperature = ((17572 * raw) / 65536) - 4685;
}
