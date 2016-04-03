/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_si70xx
 * @{
 *
 * @file
 * @brief       Implementation of Si7006/13/20/21 sensor driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */


#include "si70xx.h"

#include "xtimer.h"

/**
 * @brief       Utility method to perform and reconstruct a measurement.
 */
static uint32_t si70xx_measure(si70xx_t *dev, uint8_t command)
{
    uint8_t result[2];

    i2c_acquire(dev->i2c_dev);
    i2c_write_byte(dev->i2c_dev, dev->address, command);
    i2c_read_bytes(dev->i2c_dev, dev->address, (char *) result, 2);
    i2c_release(dev->i2c_dev);

    /* reconstruct raw result */
    return ((uint32_t)result[0] << 8) + (result[1] & 0xfc);
}

int si70xx_test(si70xx_t *dev)
{
    uint8_t revision = si70xx_get_revision(dev);

    if (revision != SI70XX_REVISION_1 && revision != SI70XX_REVISION_2) {
        return -1;
    }

    uint8_t id = si70xx_get_id(dev);

    if (id != SI70XX_ID_SI7006 && id != SI70XX_ID_SI7013 &&
        id != SI70XX_ID_SI7020 && id != SI70XX_ID_SI7021) {
        return -2;
    }

    return 0;
}

int si70xx_init(si70xx_t *dev, i2c_t i2c_dev, uint8_t address)
{
    dev->i2c_dev = i2c_dev;
    dev->address = address;

    /* setup the i2c bus */
    i2c_acquire(dev->i2c_dev);
    int result = i2c_init_master(dev->i2c_dev, I2C_SPEED_NORMAL);

    if (result != 0) {
        i2c_release(dev->i2c_dev);
        return result;
    }

    /* initialize the peripheral */
    i2c_write_byte(dev->i2c_dev, dev->address, SI70XX_RESET);

    /* sensor is ready after at most 25 ms */
    xtimer_usleep(25 * MS_IN_USEC);
    i2c_release(dev->i2c_dev);

    return 0;
}

uint16_t si70xx_get_relative_humidity(si70xx_t *dev)
{
    uint32_t raw;
    int32_t humidity;

    /* perform measurement */
    raw = si70xx_measure(dev, SI70XX_MEASURE_RH_HOLD);

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

int16_t si70xx_get_temperature(si70xx_t *dev)
{
    uint32_t raw;

    /* perform measurement */
    raw = si70xx_measure(dev, SI70XX_MEASURE_TEMP_HOLD);

    return ((17572 * raw) / 65536) - 4685;
}

void si70xx_get_both(si70xx_t *dev, uint16_t *humidity, int16_t *temperature)
{
    uint32_t raw;

    /* read the humidity the normal way */
    *humidity = si70xx_get_relative_humidity(dev);

    /* read the temperature using the data from the previous measurement */
    raw = si70xx_measure(dev, SI70XX_MEASURE_TEMP_PREV);

    *temperature = ((17572 * raw) / 65536) - 4685;
}

uint64_t si70xx_get_serial(si70xx_t *dev)
{
    uint8_t out[2];
    uint8_t in_first[8] = { 0 };
    uint8_t in_second[8] = { 0 };

    /* read the lower bytes */
    out[0] = SI70XX_READ_ID_FIRST_A;
    out[1] = SI70XX_READ_ID_FIRST_B;

    i2c_acquire(dev->i2c_dev);
    i2c_write_bytes(dev->i2c_dev, dev->address, (char *) out, 2);
    i2c_read_bytes(dev->i2c_dev, dev->address, (char *) in_first, 8);

    /* read the higher bytes */
    out[0] = SI70XX_READ_ID_SECOND_A;
    out[1] = SI70XX_READ_ID_SECOND_B;

    i2c_write_bytes(dev->i2c_dev, dev->address, (char *) out, 2);
    i2c_read_bytes(dev->i2c_dev, dev->address, (char *) in_second, 8);
    i2c_release(dev->i2c_dev);

    /* calculate the ID */
    uint32_t id_first = ((uint32_t)in_first[0] << 24) + ((uint32_t)in_first[2] << 16) +
                        (in_first[4] << 8) + (in_first[6] << 0);
    uint32_t id_second = ((uint32_t)in_second[0] << 24) + ((uint32_t)in_second[2] << 16) +
                         (in_second[4] << 8) + (in_second[6] << 0);

    return (((uint64_t) id_first) << 32) + id_second;
}

uint8_t si70xx_get_id(si70xx_t *dev)
{
    return (si70xx_get_serial(dev) >> 24) & 0xff;
}

uint8_t si70xx_get_revision(si70xx_t *dev)
{
    uint8_t out[2];
    uint8_t in = 0;

    /* read the revision number */
    out[0] = SI70XX_READ_REVISION_A;
    out[1] = SI70XX_READ_REVISION_B;

    i2c_acquire(dev->i2c_dev);
    i2c_write_bytes(dev->i2c_dev, dev->address, (char *) out, 2);
    i2c_read_byte(dev->i2c_dev, dev->address, (char *) &in);
    i2c_release(dev->i2c_dev);

    return in;
}
