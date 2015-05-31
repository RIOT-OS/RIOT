/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_hdc1000
 * @{
 *
 * @file
 * @brief       Driver for the TI HDC1000 Humidity and Temperature Sensor.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"
#include "hdc1000.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Register Map
 */
#define HDC1000_TEMPERATURE        0x00
#define HDC1000_HUMIDITY           0x01
#define HDC1000_CONFG              0x02
#define HDCD1000_SID1              0xFB
#define HDCD1000_SID2              0xFC
#define HDCD1000_SID3              0xFD
#define HDC1000_MANUFACTURER_ID    0xFE
#define HDC1000_DEVICE_ID          0xFF

/**
 * @brief Configuration Register Description
 */
#define HDC1000_CONFG_RST          (1 << 15)
#define HDC1000_CONFG_HEAT         (1 << 13)
#define HDC1000_CONFG_SEQ_MOD      (1 << 12)
#define HDC1000_CONFG_BTST_LOW     (1 << 11)
#define HDC1000_CONFG_TRES_MSK     (1 << 10)
#define HDC1000_CONFG_TRES_11      (1 << 10)
#define HDC1000_CONFG_TRES_14      0
#define HDC1000_CONFG_HRES_MSK     (1 << 9 | 1 << 8)
#define HDC1000_CONFG_HRES_14      0
#define HDC1000_CONFG_HRES_11      (1 << 8)
#define HDC1000_CONFG_HRES_8       (1 << 9)

/**
 * @brief Manufacturer and Device ID
 */
#define HDC1000_MID_VALUE          0x5449
#define HDC1000_DID_VALUE          0x1000

#define I2C_SPEED                  I2C_SPEED_FAST

int hdc1000_test(hdc1000_t *dev)
{
    char reg[2];
    uint16_t tmp;

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, HDC1000_MANUFACTURER_ID, reg, 2) != 2) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    tmp = ((uint16_t)reg[0] << 8) | reg[1];
    if (tmp != HDC1000_MID_VALUE) {
        return -1;
    }

    return 0;
}

int hdc1000_init(hdc1000_t *dev, i2c_t i2c, uint8_t address)
{
    char reg[2];

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->initialized = false;

    i2c_acquire(dev->i2c);
    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    if (hdc1000_test(dev)) {
        return -2;
    }

    /* set 14 bit resolution for both sensors and sequence mode */
    uint16_t tmp = HDC1000_CONFG_SEQ_MOD;
    reg[0] = (uint8_t)(tmp >> 8);
    reg[1] = (uint8_t)tmp;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, HDC1000_CONFG, reg, 2) != 2) {
        i2c_release(dev->i2c);
        return -3;
    }
    dev->initialized = true;

    i2c_release(dev->i2c);
    return 0;
}

int hdc1000_reset(hdc1000_t *dev)
{
    char reg[2];
    uint16_t tmp = HDC1000_CONFG_RST;
    reg[0] = (uint8_t)(tmp >> 8);
    reg[1] = (uint8_t)tmp;
    dev->initialized = false;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, HDC1000_CONFG, reg, 2) != 2) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    return 0;
}

int hdc1000_startmeasure(hdc1000_t *dev)
{
    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    /* Trigger the measurements by executing a write access
     * to the address 0x00 (HDC1000_TEMPERATURE).
     * Conversion Time is 6.50ms by 14 bit resolution.
     */
    if (i2c_write_byte(dev->i2c, dev->addr, HDC1000_TEMPERATURE) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    return 0;
}

int hdc1000_read(hdc1000_t *dev, uint16_t *rawtemp, uint16_t *rawhum)
{
    char buf[4];

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_bytes(dev->i2c, dev->addr, buf, 4) != 4) {
        i2c_release(dev->i2c);
        return -1;
    }
    /* Register bytes are sent MSB first. */
    *rawtemp = ((uint16_t)buf[0] << 8) | buf[1];
    *rawhum = ((uint16_t)buf[2] << 8) | buf[3];

    i2c_release(dev->i2c);
    return 0;
}

void hdc1000_convert(uint16_t rawtemp, uint16_t rawhum,  int *temp, int *hum)
{
    /* calculate temperature*100 [°C] */
    *temp = (int)((((int32_t)rawtemp * 16500) >> 16) - 4000);
    DEBUG("hdc1000 : T: %d\n", *temp);

    /* calculate relative humidity*100 [%RH] */
    *hum = (int)(((int32_t)rawhum * 10000) >> 16);
    DEBUG("hdc1000 : RH: %d\n", *hum);
}
