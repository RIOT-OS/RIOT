/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_srf02
 * @{
 *
 * @file        srf02.c
 * @brief       Driver for the SRF02 ultrasonic ranger.
 *              The connection between the MCU and the SRF02 is based on the
 *              i2c-interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "msg.h"
#include "vtimer.h"
#include "timex.h"
#include "thread.h"
#include "hwtimer.h"
#include "srf02.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


int srf02_init(srf02_t *dev, i2c_t i2c, uint8_t addr, i2c_speed_t speed)
{
    dev->i2c = i2c;
    dev->addr = addr;

    /* initialize i2c interface */
    return i2c_init_master(i2c, speed);
}

uint16_t srf02_get_distance(srf02_t *dev, srf02_mode_t mode)
{
    int status;
    char range_high_byte = 0;
    char range_low_byte = 0;
    uint16_t distance = 0;

    /* initialize measure mode*/
    status = i2c_write_reg(dev->i2c, dev->addr, SRF02_COMMAND_REG, mode);

    if (status < 0) {
        DEBUG("Write the ranging command to the i2c-interface is failed");
        uint16_t distance = UINT16_MAX;
        return distance;
    }

    hwtimer_wait(70000);

    status = i2c_read_reg(dev->i2c, dev->addr,
                      SRF02_RANGE_HIGH_BYTE, &range_high_byte);

    if (status < 0) {
        DEBUG("Read the high echo byte from the i2c-interface is failed");
        distance = UINT16_MAX;
        return distance;
    }

    status = i2c_read_reg(dev->i2c, dev->addr,
                      SRF02_RANGE_LOW_BYTE, &range_low_byte);

    if (status < 0) {
        DEBUG("Read the low echo byte from the i2c-interface is failed");
        distance = UINT16_MAX;
        return distance;
    }

    distance = (range_high_byte << 8) | (range_low_byte);

    return distance;
}
