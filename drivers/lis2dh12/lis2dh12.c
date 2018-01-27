/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_lis2dh12
 * @{
 *
 * @file
 * @brief       LIS2DH12 accelerometer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "assert.h"

#include "lis2dh12.h"
#include "lis2dh12_internal.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/* SPI bus speed and mode */
#define BUS_CLK             SPI_CLK_5MHZ
#define MODE                SPI_MODE_0

#define BUS_OK              SPI_OK

/* shortcuts for SPI bus parameters */
#define BUS                 (dev->p->spi)
#define CS                  (dev->p->cs)

/* flag to set when reading from the device */
#define FLAG_READ           (0x80)

/* flag to enable address auto incrementation on read or write */
#define FLAG_AINC           (0x40)

static int _acquire(const lis2dh12_t *dev)
{
    return spi_acquire(BUS, CS, MODE, BUS_CLK);
}

static void _release(const lis2dh12_t *dev)
{
    spi_release(BUS);
}

static uint8_t _read(const lis2dh12_t *dev, uint8_t reg)
{
    return spi_transfer_reg(BUS, CS, (FLAG_READ | reg), 0);
}

static void _read_burst(const lis2dh12_t *dev, uint8_t reg,
                              void *data, size_t len)
{
    spi_transfer_regs(BUS, CS, (FLAG_READ | FLAG_AINC | reg), NULL, data, len);
}

static void _write(const lis2dh12_t *dev, uint8_t reg, uint8_t data)
{
    DEBUG("[lis2dh12] write: reg 0x%02x, val 0x%02x\n", (int)reg, (int)data);
    spi_transfer_reg(BUS, CS, reg, data);
}

int lis2dh12_init(lis2dh12_t *dev, const lis2dh12_params_t *params)
{
    assert(dev && params);

    dev->p = params;
    dev->comp = (1000UL * (0x02 << (dev->p->scale >> 4)));

    /* start by setting up the chip select pin */
    if (spi_init_cs(BUS, CS) != SPI_OK) {
        DEBUG("[lis2dh12] error: unable to initialize CS pin\n");
        return LIS2DH12_NOBUS;
    }

    /* acquire the SPI bus and verify that our parameters are valid */
    if (_acquire(dev) != BUS_OK) {
        DEBUG("[lis2dh12] error: unable to acquire SPI bus\n");
        return LIS2DH12_NOBUS;
    }

    /* read the WHO_IM_I register to verify the connections to the device */
    if (_read(dev, REG_WHO_AM_I) != WHO_AM_I_VAL) {
        DEBUG("[lis2dh12] error: invalid value read from WHO_AM_I register\n");
        _release(dev);
        return LIS2DH12_NODEV;
    }

    /* set sampling rate and scale. This also enables the device and starts
     * sampling of data */
    _write(dev, REG_CTRL_REG4, dev->p->scale);
    _write(dev, REG_CTRL_REG1, dev->p->rate);

    _release(dev);
    return LIS2DH12_OK;
}

int lis2dh12_read(const lis2dh12_t *dev, int16_t *data)
{
    assert(dev && data);

    /* allocate 6 byte to save the 6 RAW data registers */
    uint8_t raw[6];

    /* read sampled data from the device */
    _acquire(dev);
    _read_burst(dev, REG_OUT_X_L, raw, 6);
    _release(dev);

    /* calculate the actual g-values for the x, y, and z dimension */
    for (int i = 0; i < 3; i++) {
        int32_t tmp = ((raw[i * 2] >> 6) | (raw[(i * 2) + 1] << 2));
        if (tmp & 0x00000200) {
            tmp |= 0xfffffc00;
        }
        data[i] = (int16_t)((tmp * dev->comp) / 512);
    }

    return LIS2DH12_OK;
}

int lis2dh12_poweron(const lis2dh12_t *dev)
{
    assert(dev);

    _acquire(dev);
    _write(dev, REG_CTRL_REG1, dev->p->rate);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_poweroff(const lis2dh12_t *dev)
{
    assert(dev);

    _acquire(dev);
    _write(dev, REG_CTRL_REG1, 0);
    _release(dev);

    return LIS2DH12_OK;
}
