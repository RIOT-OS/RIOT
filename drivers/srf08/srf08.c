/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_srf08
 * @{
 *
 * @file
 * @brief       Driver for the SRF08 ultrasonic ranger.
 *              The connection between the MCU and the SRF08 is based on the
 *              i2c-interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include "xtimer.h"
#include "srf08.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


int srf08_init(srf08_t *dev, i2c_t i2c, uint8_t addr, i2c_speed_t speed)
{
    int status = 0;
    (void)speed;
    (void)status;

    dev->i2c = i2c;
    dev->addr = addr;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    /* initialize i2c interface */
    i2c_init(dev->i2c);
    /* Release the bus for other threads. */
    i2c_release(dev->i2c);

    if(status < 0) {
        return -1;
    }

    /* set the maximum range */
    if (srf08_set_max_range(dev, SRF08_MAX_RANGE_6M) < 0) {
        return -3;
    }

    /* set the maximum gain */
    if (srf08_set_max_gain(dev, SRF08_MAX_GAIN) < 0) {
        return -4;
    }

    return 0;
}


int srf08_set_max_range(const srf08_t *dev, uint8_t max_range)
{
    int status;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    status = i2c_write_reg(dev->i2c, dev->addr, SRF08_RANGE_REG, max_range, 0);
    /* Release the bus for other threads. */
    i2c_release(dev->i2c);

    return status;
}


int srf08_set_max_gain(const srf08_t *dev, uint8_t gain)
{
    int status;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    status = i2c_write_reg(dev->i2c, dev->addr, SRF08_GAIN_REG, gain, 0);
    /* Release the bus for other threads. */
    i2c_release(dev->i2c);

    return status;
}


int srf08_get_distances(const srf08_t *dev, uint16_t *range_array, int num_echos, srf08_mode_t ranging_mode)
{
    int status;
    int echo_number = 0;
    uint8_t range_bytes[sizeof(uint16_t)];
    uint8_t register_location;
    char max_reg_no_read = (num_echos * sizeof(range_bytes)) +1;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    /* set ranging mode */
    status = i2c_write_reg(dev->i2c, dev->addr, SRF08_COMMAND_REG, ranging_mode, 0);
    /* Release the bus for other threads. */
    i2c_release(dev->i2c);

    if (!status) {
        DEBUG("Write the ranging command to the i2c-interface is failed\n");
        return -1;
    }

    if(max_reg_no_read > SRF08_MAX_REGISTER_NUMBER) {
        DEBUG("Too many echos requested. Max. is 17\n");
        return -2;
    }

    xtimer_usleep(70000);

    /* read all echo buffers */
    for (register_location = 2; register_location < max_reg_no_read;
         register_location += sizeof(range_bytes)) {

        /* Acquire exclusive access to the bus. */
        i2c_acquire(dev->i2c);
        /* read the echo bytes */
        status = i2c_read_regs(dev->i2c, dev->addr, register_location, range_bytes, sizeof(range_bytes), 0);
        /* Release the bus for other threads. */
        i2c_release(dev->i2c);

        if (!status) {
            DEBUG("Read the echo bytes from the i2c-interface is failed\n");
            return -3;
        }

        else {
            uint16_t distance = (range_bytes[0] << 8) | range_bytes[1];
            range_array[(register_location - 2) / 2] = distance;
            echo_number++;
        }
    }

    return echo_number;
}
