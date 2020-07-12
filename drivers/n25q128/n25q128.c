/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Driver for the N25Q128 serial flash memory
 *
 * @author      René Herthel <rene-herthel@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "n25q128.h"
#include "include/n25q128_internal.h"

/*
 * @brief   Clamps a specifc value between minimum and maximum.
 */
#define CLAMP(x, min, max)    ((x > max) ? max : ((x < min) ? min : x))

/*
 * @brief   Calls the spi driver for receiving bytes.
 */
static inline void _receive(spi_t bus, uint8_t * in, size_t len)
{
    spi_transfer_bytes(bus, SPI_CS_UNDEF, false, NULL, in, len);
}

/*
 * @brief   Calls the spi driver for sending bytes.
 */
static inline void _send(spi_t bus, uint8_t * out, size_t len)
{
    spi_transfer_bytes(bus, SPI_CS_UNDEF, false, out, NULL, len);
}

/*
 * @brief   Builds and send a packet, with an opcode included.
 */
static inline void _send_instruction(n25q128_dev_t *dev)
{
    /* The instruction 'set', including operation code and address. */
    uint8_t set[4] = {0};

    /* Possible length is 4 Byte (with address) or 1 Byte (only instruction). */
    size_t len = (dev->opt.enabled.addr) ? 4 : 1;

    /* The first byte will allways be the operation code. */
    set[0] = (dev->cmd.code & 0xFF);

    /* The address is made of 3 Bytes, which needs to be shifted correctly. */
    set[1] = ((dev->cmd.addr & 0x00FF0000) >> 16); // = 0x000000FF
    set[2] = ((dev->cmd.addr & 0x0000FF00) >> 8);  // = 0x000000FF
    set[3] = (dev->cmd.addr & 0x000000FF);         // = 0x000000FF

    /* We need to send it here, to start the corresponding program in the device. */
    _send(dev->conf.bus, set, len);
}

/*
 * @brief   Execute the assembled command of the configured device with the
 *          prepared instruction set.
 */
static inline void _execute(n25q128_dev_t *dev)
{
    /* Select the configured memory device, via the Chip-Select-GPIO. */
    gpio_clear(dev->conf.cs);

    /* Try to acquire the required spi resource. */
    if (spi_acquire(dev->conf.bus, dev->conf.cs, dev->conf.mode, dev->conf.clk) == SPI_OK) {

        /* Send the instruction, containing operation code and (optional) an address. */
        _send_instruction(dev);

        /* Memory is receiving, so transfer from driver to memory. */
        if (dev->opt.enabled.send) {
            _send(dev->conf.bus, dev->data.buf, dev->data.len);
        }

        /* Memory is sending, so transfer from memory to driver. */
        if (dev->opt.enabled.recv) {
            _receive(dev->conf.bus, dev->data.buf, dev->data.len);
        }

        /* Release the previous acquired spi resource. */
        spi_release(dev->conf.bus);
    }

    /* Deselect the configured memory device, via the Chip-Select-GPIO. */
    gpio_set(dev->conf.cs);
}

/*
 * @brief   Sets the Write Enable Latch (WEL) bit in the Status Register.
 */
static inline void _write_enable(n25q128_dev_t *dev)
{
    dev->cmd.code = N25Q128_OPCODE_WREN;
    dev->cmd.addr = 0;
    dev->data.buf = NULL;
    dev->data.len = 0;
    dev->opt.mask = 0;

    _execute(dev);
}

/*
 * @brief   Resets the Write Enable Latch (WEL) bit in the Status Register.
 */
static inline void _write_disable(n25q128_dev_t *dev)
{
    dev->cmd.code = N25Q128_OPCODE_WRDI;
    dev->cmd.addr = 0;
    dev->data.buf = NULL;
    dev->data.len = 0;
    dev->opt.mask = 0;

    _execute(dev);
}

/*
 * @brief   Read and return the content of the Status Register.
 */
static inline uint8_t _read_status_reg(n25q128_dev_t *dev)
{
    static uint8_t buf[1] = {0};

    dev->cmd.code = N25Q128_OPCODE_RDSR;
    dev->cmd.addr = 0;
    dev->data.buf = buf;
    dev->data.len = 1;
    dev->opt.mask = N25Q128_OPT_RECV_EN;

    _execute(dev);

    return buf[0];
}

static inline bool _write_enable_latch(n25q128_dev_t *dev)
{
    uint8_t spi_status_reg = _read_status_reg(dev);
    return (spi_status_reg & N25Q128_STAT_REG_WEL);
}

int n25q128_init(n25q128_dev_t *dev)
{
    int status = N25Q128_OK;

    /* Initialize spi bus and ChipSelect pin. */
    status = spi_init_cs(dev->conf.bus, dev->conf.cs);

    /* Something went wrong, but we can't continue without SPI. */
    if (status != SPI_OK) {
#if ENABLE_DEBUG
        if (status == SPI_NODEV) {
            DEBUG("N25Q128 -> _init -> NODEV (Invalid SPI bus specified)\n");
        }
        else if (status == SPI_NOCS) {
            DEBUG("N25Q128 -> _init -> NOCS (Invalid chip select line specified)\n");
        }
        else if (status == SPI_NOMODE) {
            DEBUG("N25Q128 -> _init -> NOMODE (Selected mode is not supported)\n");
        }
        else if (status == SPI_NOCLK) {
            DEBUG("N25Q128 -> _init -> NOCLK (Selected clock value is not supported)\n");
        }
#endif /* ENABLE_DEBUG */
        return N25Q128_NOSPI;
    }

    /* Initialize the write pin. The write pin will always be an output pin. */
    if (gpio_init(dev->conf.write, GPIO_OUT) < 0) {
        DEBUG("N25Q128 -> init 'Write'-GPIO -> FAILED!\n");
        return N25Q128_NOGPIO;
    }

    /* Initialize the hold pin. The hold pin will always be an output pin. */
    if (gpio_init(dev->conf.hold, GPIO_OUT) < 0) {
        DEBUG("N25Q128 -> init 'Hold'-GPIO -> FAILED!\n");
        return N25Q128_NOGPIO;
    }

    /* DE-Select the write pin. */
    gpio_set(dev->conf.write);

    /* DE-Select the hold pin, or the device will be paused! */
    gpio_set(dev->conf.hold);

    return N25Q128_OK;
}

void n25q128_read_id(n25q128_dev_t *dev, uint8_t *buf, size_t len)
{
    dev->cmd.code = N25Q128_OPCODE_RDID;
    dev->cmd.addr = 0;
    dev->data.buf = buf;
    dev->data.len = CLAMP(len, 1, 20);
    dev->opt.mask = N25Q128_OPT_RECV_EN;

    _execute(dev);
}

void n25q128_read_data_bytes(n25q128_dev_t *dev, uint32_t addr, uint8_t *buf, size_t len)
{
    dev->cmd.code = N25Q128_OPCODE_READ;
    dev->cmd.addr = addr;
    dev->data.buf = buf;
    dev->data.len = CLAMP(len, 1, len); /* 'infinite' bytes possible */
    dev->opt.mask = N25Q128_OPT_INF_EN | N25Q128_OPT_RECV_EN | N25Q128_OPT_ADDR_EN;

    _execute(dev);
}

void n25q128_page_program(n25q128_dev_t *dev, uint32_t addr, uint8_t *buf, size_t len)
{
    _write_enable(dev);

    dev->cmd.code = N25Q128_OPCODE_PP;
    dev->cmd.addr = addr;
    dev->data.buf = buf;
    dev->data.len = CLAMP(len, 1, 256);
    dev->opt.mask = N25Q128_OPT_SEND_EN | N25Q128_OPT_ADDR_EN;

    _execute(dev);
}

void n25q128_sector_erase(n25q128_dev_t *dev, uint32_t addr)
{
    _write_enable(dev);

    dev->cmd.code = N25Q128_OPCODE_SE;
    dev->cmd.addr = addr;
    dev->data.buf = NULL;
    dev->data.len = 0;
    dev->opt.mask = N25Q128_OPT_ADDR_EN;

    _execute(dev);
}

void n25q128_bulk_erase(n25q128_dev_t *dev)
{
    _write_enable(dev);

    dev->cmd.code = N25Q128_OPCODE_BE;
    dev->cmd.addr = 0;
    dev->data.buf = NULL;
    dev->data.len = 0;
    dev->opt.mask = 0;

    _execute(dev);
}

void n25q128_program_erase_suspend(n25q128_dev_t *dev)
{
    dev->cmd.code = N25Q128_OPCODE_PES;
    dev->cmd.addr = 0;
    dev->data.buf = NULL;
    dev->data.len = 0;
    dev->opt.mask = 0;

    _execute(dev);
}

void n25q128_program_erase_resume(n25q128_dev_t *dev)
{
    dev->cmd.code = N25Q128_OPCODE_PER;
    dev->cmd.addr = 0;
    dev->data.buf = NULL;
    dev->data.len = 0;
    dev->opt.mask = 0;

    _execute(dev);
}

bool n25q128_write_in_progress(n25q128_dev_t *dev)
{
    return (_read_status_reg(dev) & N25Q128_STAT_REG_WIP);
}
