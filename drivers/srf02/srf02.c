/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_srf02
 * @{
 *
 * @file
 * @brief       Driver for the SRF02 ultrasonic range sensor
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"
#include "srf02.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   Per default use normal speed on the I2C bus
 */
#define BUS_SPEED           (I2C_SPEED_NORMAL)

/**
 * @brief   SRF02 register addresses
 * @{
 */
#define REG_CMD             (0x00)
#define REG_HIGH            (0x02)
#define REG_LOW             (0x03)
#define REG_AUTO_HIGH       (0x04)
#define REG_AUTO_LOW        (0x05)
/** @} */

/**
 * @brief   Some additional SRF02 commands
 * @{
 */
#define CMD_ADDR_SEQ1       (0xa0)
#define CMD_ADDR_SEQ2       (0xaa)
#define CMD_ADDR_SEQ3       (0xa5)
/** @} */


int srf02_init(srf02_t *dev, i2c_t i2c, uint8_t addr)
{
    dev->i2c = i2c;
    dev->addr = (addr >> 1);    /* internally we right align the 7-bit addr */
    char rev;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    /* initialize i2c interface */
    if (i2c_init_master(dev->i2c, BUS_SPEED) < 0) {
        DEBUG("[srf02] error initializing I2C bus\n");
        return -1;
    }
    /* try to read the software revision (read the CMD reg) from the device */
    i2c_read_reg(i2c, dev->addr, REG_CMD, &rev);
    if (rev == 0 || rev == 255) {
        i2c_release(dev->i2c);
        DEBUG("[srf02] error reading the devices software revision\n");
        return -1;
    } else {
        DEBUG("[srf02] software revision: 0x%02x\n", rev);
    }
    /* Release the bus for other threads. */
    i2c_release(dev->i2c);

    DEBUG("[srf02] initialization successful\n");
    return 0;
}

void srf02_trigger(srf02_t *dev, srf02_mode_t mode)
{
    /* trigger a new measurement by writing the mode to the CMD register */
    DEBUG("[srf02] trigger new reading\n");
    i2c_acquire(dev->i2c);
    i2c_write_reg(dev->i2c, dev->addr, REG_CMD, mode);
    i2c_release(dev->i2c);
}

uint16_t srf02_read(srf02_t *dev)
{
    char res[2];

    /* read the results */
    i2c_acquire(dev->i2c);
    i2c_read_regs(dev->i2c, dev->addr, REG_HIGH, res, 2);
    i2c_release(dev->i2c);
    DEBUG("[srf02] result - high: 0x%02x low: 0x%02x\n", res[0], res[1]);

    /* compile result - TODO: fix for different host byte order other than LE */
    return ((((uint16_t)res[0]) << 8) | (res[1] & 0xff));
}

uint16_t srf02_get_distance(srf02_t *dev, srf02_mode_t mode)
{
    /* trigger a new reading */
    srf02_trigger(dev, mode);
    /* give the sensor the required time for sampling */
    xtimer_usleep(SRF02_RANGE_DELAY);
    /* get the results */
    return srf02_read(dev);
}

void srf02_set_addr(srf02_t *dev, uint8_t new_addr)
{
    /* get access to the bus */
    i2c_acquire(dev->i2c);

    DEBUG("[srf02] reprogramming device address to 0x%02x\n", (int)new_addr);

    /* write the new address, for this we need to follow a certain sequence */
    i2c_write_reg(dev->i2c, dev->addr, REG_CMD, CMD_ADDR_SEQ1);
    i2c_write_reg(dev->i2c, dev->addr, REG_CMD, CMD_ADDR_SEQ2);
    i2c_write_reg(dev->i2c, dev->addr, REG_CMD, CMD_ADDR_SEQ3);
    i2c_write_reg(dev->i2c, dev->addr, REG_CMD, new_addr);
    dev->addr = (new_addr >> 1);

    /* release the bus */
    i2c_release(dev->i2c);
}
