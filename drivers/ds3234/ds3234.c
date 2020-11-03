/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_ds3234
 * @{
 *
 * @file
 * @brief       Driver for the DS3234 Extremely Accurate SPI Bus RTC with
 *              Integrated Crystal and SRAM, from Maxim
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include "ds3234.h"
#include "ds3234_regs.h"

#define ENABLE_DEBUG            0
#include "debug.h"

/* SPI command byte parameters */
#define DS3234_CMD_READ         (0x00)
#define DS3234_CMD_WRITE        (0x80)


/**
 * @brief Read one or more registers from the sensor
 *
 * @param[in]  dev    device descriptor
 * @param[in]  addr   register address
 * @param[in]  len    number of bytes to read
 * @param[out] buf    destination buffer
 */
static void ds3234_read_reg(const ds3234_params_t *dev, uint8_t addr, size_t len, uint8_t *buf)
{
    uint8_t command = DS3234_CMD_READ | addr;
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE_3, dev->clk);
    /* Perform the transaction */
    spi_transfer_regs(dev->spi, dev->cs, command, NULL, buf, len);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
}

/**
 * @brief Write a register value to the sensor
 *
 * @param[in]  dev    device descriptor
 * @param[in]  addr   register address
 * @param[in]  len    register size
 * @param[in]  buf    source buffer
 */
static void ds3234_write_reg(const ds3234_params_t *dev, uint8_t addr, size_t len, const uint8_t *buf)
{
    uint8_t command = DS3234_CMD_WRITE | addr;
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE_3, dev->clk);
    /* Perform the transaction */
    spi_transfer_regs(dev->spi, dev->cs, command, buf, NULL, len);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
}

int ds3234_pps_init(const ds3234_params_t *dev)
{
    /* initialize CS pin */
    int res = spi_init_cs(dev->spi, dev->cs);
    if (res < 0) {
        return DS3234_NO_SPI;
    }
    DEBUG("ds3234: init on SPI_DEV(%u)\n", dev->spi);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        for (int k = 0; k <= 0x19; ++k) {
            uint8_t dbg_reg = 0;
            ds3234_read_reg(dev, k, 1, &dbg_reg);
            DEBUG("%2x: %2x\n", k, dbg_reg);
        }
    }
    uint8_t reg = 0;
    ds3234_read_reg(dev, DS323X_REG_CONTROL, 1, &reg);

    /* set reg to a non-zero known value to check if device is present */
    reg |= DS323X_REG_CONTROL_RS1_MASK;

    ds3234_write_reg(dev, DS323X_REG_CONTROL, 1, &reg);
    uint8_t readback = 0;
    ds3234_read_reg(dev, DS323X_REG_CONTROL, 1, &readback);
    if (reg != readback) {
        DEBUG("ds3234: readback mismatch: expected %u, actual %u\n", (unsigned)reg, (unsigned)readback);
        return DS3234_NO_DEV;
    }

    /* The control register is configured to:
     * - Enable the oscillator
     * - Enable an square wave output on the SQW pin
     * - Sets the square wave frequency to 1 Hz
     */
    reg &= ~(DS323X_REG_CONTROL_EOSC_MASK | DS323X_REG_CONTROL_INTCN_MASK |
        DS323X_REG_CONTROL_RS1_MASK | DS323X_REG_CONTROL_RS2_MASK);
    ds3234_write_reg(dev, DS323X_REG_CONTROL, 1, &reg);

    return DS3234_OK;
}
