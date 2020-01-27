/*
 * Copyright (C) 2020 Pieter du Preez <pdupreez@gmail.com>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/comms_bus.h"

/* flag to enable address auto incrementation on read or write */
#define COMMS_BUS_FLAG_AINC (0x40)

#ifdef MODULE_PERIPH_SPI

#include "periph/gpio.h"

static int _spi_init(const comms_bus_params_t *bus);
static int _spi_acquire(const comms_bus_params_t *bus);
static void _spi_release(const comms_bus_params_t *bus);
static int _spi_read_reg(const comms_bus_params_t *bus,
                         uint16_t reg, uint8_t *out);
static int _spi_read_regs(const comms_bus_params_t *bus,
                          uint16_t reg, void *data, size_t len);
static int _spi_write_reg(const comms_bus_params_t *bus,
                          uint8_t reg, uint8_t data);

/* flag to set when reading from the device */
#define COMMS_BUS_FLAG_READ (0x80)

static int _spi_init(const comms_bus_params_t *bus)
{
    spi_init(bus->spi.dev);
    if (bus->spi.cs != GPIO_UNDEF && spi_init_cs(bus->spi.dev, bus->spi.cs) != SPI_OK) {
        return SPI_NOCS;
    }
    return SPI_OK;
}

static int _spi_acquire(const comms_bus_params_t *bus)
{
    return spi_acquire(bus->spi.dev, bus->spi.cs, bus->spi.mode, bus->spi.clk);
}

static void _spi_release(const comms_bus_params_t *bus)
{
    spi_release(bus->spi.dev);
}

static int _spi_read_reg(const comms_bus_params_t *bus, uint16_t reg, uint8_t *out)
{
    spi_transfer_regs(bus->spi.dev, bus->spi.cs,
                      (COMMS_BUS_FLAG_READ | (uint8_t)reg), NULL, out, 1);
    DEBUG("[_spi_read_reg] reg 0x%02x, val 0x%02x\n", reg, *out);
    return SPI_OK;
}

static int _spi_read_regs(const comms_bus_params_t *bus, uint16_t reg,
                           void *data, size_t len)
{
    spi_transfer_regs(bus->spi.dev, bus->spi.cs,
                      (COMMS_BUS_FLAG_READ | COMMS_BUS_FLAG_AINC | (uint8_t)reg), NULL, data, len);
    return SPI_OK;
}

static int _spi_write_reg(const comms_bus_params_t *bus, uint8_t reg, uint8_t data)
{
    DEBUG("[_spi_write_reg] write: reg 0x%02x, val 0x%02x\n", (int)reg, (int)data);
    spi_transfer_reg(bus->spi.dev, bus->spi.cs, reg, data);
    return SPI_OK;
}

#endif

#ifdef MODULE_PERIPH_I2C

static int _i2c_init(const comms_bus_params_t *bus);
static int _i2c_acquire(const comms_bus_params_t *bus);
static void _i2c_release(const comms_bus_params_t *bus);
static int _i2c_read_reg(const comms_bus_params_t *bus,
                         uint16_t reg, uint8_t *out);
static int _i2c_read_regs(const comms_bus_params_t *bus,
                          uint16_t reg, void *data, size_t len);
static int _i2c_write_reg(const comms_bus_params_t *bus,
                          uint8_t reg, uint8_t data);

static int _i2c_init(const comms_bus_params_t *bus)
{
    (void)(bus->i2c.dev);
    return 0;
}

static int _i2c_acquire(const comms_bus_params_t *bus)
{
    return i2c_acquire(bus->i2c.dev);
}

static void _i2c_release(const comms_bus_params_t *bus)
{
    i2c_release(bus->i2c.dev);
}

static int _i2c_read_reg(const comms_bus_params_t *bus, uint16_t reg, uint8_t *out)
{
    return i2c_read_reg(bus->i2c.dev, bus->i2c.addr, reg, out, 0);
}

static int _i2c_read_regs(const comms_bus_params_t *bus, uint16_t reg,
                          void *data, size_t len)
{
    return i2c_read_regs(bus->i2c.dev, bus->i2c.addr, (COMMS_BUS_FLAG_AINC | reg), data, len, 0);
}

static int _i2c_write_reg(const comms_bus_params_t *bus, uint8_t reg, uint8_t data)
{
    DEBUG("[_i2c_write_reg] write: reg 0x%02x, val 0x%02x\n", (int)reg, (int)data);
    return i2c_write_reg(bus->i2c.dev, bus->i2c.addr, reg, data, 0);
}

#endif

void comms_bus_setup(comms_transport_t* transport)
{
    switch (transport->type) {
#ifdef MODULE_PERIPH_SPI
    case PERIPH_COMMS_BUS_SPI:
        transport->f.comms_bus_init = _spi_init;
        transport->f.comms_bus_acquire = _spi_acquire;
        transport->f.comms_bus_release = _spi_release;
        transport->f.comms_bus_read_reg = _spi_read_reg;
        transport->f.comms_bus_read_regs = _spi_read_regs;
        transport->f.comms_bus_write_reg = _spi_write_reg;
        break;
#endif
#ifdef MODULE_PERIPH_I2C
    case PERIPH_COMMS_BUS_I2C:
        transport->f.comms_bus_init = _i2c_init;
        transport->f.comms_bus_acquire = _i2c_acquire;
        transport->f.comms_bus_release = _i2c_release;
        transport->f.comms_bus_read_reg = _i2c_read_reg;
        transport->f.comms_bus_read_regs = _i2c_read_regs;
        transport->f.comms_bus_write_reg = _i2c_write_reg;
        break;
#endif
    default:
        transport->f.comms_bus_init = NULL;
        transport->f.comms_bus_acquire = NULL;
        transport->f.comms_bus_release = NULL;
        transport->f.comms_bus_read_reg = NULL;
        transport->f.comms_bus_read_regs = NULL;
        transport->f.comms_bus_write_reg = NULL;
    }
}
