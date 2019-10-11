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

#include <stdbool.h>
#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "n25q128.h"
#include "include/n25q128_internal.h"

/* The size of a command without address (1Byte). */
#define CMD_LEN_1_BYTE  (1)

/* The size of a command (1Byte) + address (3Byte) in bytes. */
#define CMD_LEN_4_BYTE  (4)

/* Some instructions don't need an address. */
#define ADDR_UNDEF      (-1)

/* When the range of data bytes to send is not defined or 'infinite'. */
#define INFINITE        (-1)

/*
 * @brief   Calls the spi driver for sending bytes.
 */
static inline void _send(spi_t bus, uint8_t * out, uint8_t len)
{
    spi_transfer_bytes(bus, SPI_CS_UNDEF, false, out, NULL, len);
}

/*
 * @brief   Calls the spi driver for receiving bytes.
 */
static inline void _receive(spi_t bus,uint8_t * in, uint8_t len)
{
    spi_transfer_bytes(bus, SPI_CS_UNDEF, false, NULL, in, len);
}

/*
 * @brief   Sets the Write Enable Latch (WEL) bit.
 *
 * The WEL-bit must be set prior to every Program or Erase instruction.
 *
 * @param[in] *dev      A pointer to the configured device.
 */
static inline void _write_enable(n25q128_dev_t *dev)
{
    (void)*dev;
    // TODO
}

/*
 * @brief   Resets the Write Enable Latch (WEL) bit.
 *
 * @param[in] *dev      A pointer to the configured device.
 */
static inline void _write_disable(n25q128_dev_t *dev)
{
    (void)*dev;
    // TODO
}

/*
 * @brief   Read the status register.
 *
 * The Read Status Register (RDSR) instruction allows the Status Register to be
 * read. The Status Register may be read at any time, even while a Program,
 * Erase or Write Status Register cycle is in progress. When one of these cycles
 * is in progress, it is recommended to check the Write In Progress (WIP) bit
 * (or the Program/Erase controller bit of the Flag Status Register) before
 * sending a new instruction to the device. It is also possible to read the
 * Status Register continuously.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
static inline uint8_t _read_status_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    return 0;
}

/*
 * @brief   Write to the status register.
 *
 * The write status register (WRSR) instruction allows new values to be written
 * to the status register. Before it can be accepted, a write enable (WREN)
 * instruction must previously have been executed. After the write enable (WREN)
 * instruction has been decoded and executed, the device sets the write enable
 * latch (WEL).
 *
 * @return  A status code, depending on the state of the device/driver.
 */
static inline void _write_status_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    // TODO
}

/*
 * @brief
 *
 * @return  A status code, depending on the state of the device/driver.
 */
static inline uint8_t _read_lock_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    return 0; // TODO
}

/*
 * @brief
 *
 * The Write to Lock Register (WRLR) instruction allows bits to be changed in
 * the Lock Registers. Before it can be accepted, a Write Enable (WREN)
 * instruction must previously have been executed. After the Write Enable (WREN)
 * instruction has been decoded, the device sets the Write Enable Latch (WEL).
 *
 * @return  A status code, depending on the state of the device/driver.
 */
static inline void _write_lock_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    // TODO
}

/*
 * @brief TODO
 *
 * The Write Non Volatile Configuration register (WRNVCR) instruction
 * allows new values to be written to the Non Volatile Configuration register.
 * Before it can be accepted, a write enable (WREN) instruction must previously
 * have been executed. After the write enable (WREN) instruction has been
 * decoded and executed, the device sets the write enable latch (WEL).
 *
 * @param[in] *dev      A pointer to the configured device.
 *
 * @return  The value of the non volatile register.
 */
static inline uint8_t _read_nv_reg(n25q128_dev_t *dev)
{
    (void)*dev;
    return 0;
}

/*
 * @brief TODO
 *
 * The Write Non Volatile Configuration register (WRNVCR) instruction allows new
 * values to be written to the Non Volatile Configuration register. Before it
 * can be accepted, a write enable (WREN) instruction must previously have been
 * executed. After the write enable (WREN) instruction has been decoded and
 * executed, the device sets the write enable latch (WEL).
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[in] val       The value to write into the non volatile register.
 */
static inline void _write_nv_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    // TODO
}

/*
 * @brief TODO
 *
 * The Read Volatile Configuration Register (RDVCR) instruction allows the
 * Volatile Configuration Register to be read.
 *
 * @param[in] *dev      A pointer to the configured device.
 *
 * @return  The content of the volatile register.
 */
static inline uint8_t _read_vol_reg(n25q128_dev_t *dev)
{
    (void)*dev;
    return 0;
}

/*
 * @brief TODO
 *
 * The Write Volatile Configuration register (WRVCR) instruction allows new
 * values to be written to the Volatile Configuration register. Before it can be
 * accepted, a write enable (WREN) instruction must have been executed. After
 * the write enable (WREN) instruction has been decoded and executed, the device
 * sets the write enable latch (WEL).
 *
 * @param[in] *dev  A pointer to the configured device.
 * @param[in] val   The value to write into the volatile register.
 */
static inline void _write_vol_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    // TODO
}

/*
 * @brief TODO
 *
 * The Read Volatile Enhanced Configuration Register (RDVECR) instruction allows
 * the Volatile Configuration Register to be read.
 *
 * @param[in] *dev      A pointer to the configured device.
 *
 * @return  The content of the volatile enhanced register.
 */
static inline uint8_t _read_vol_ehn_reg(n25q128_dev_t *dev)
{
    (void)*dev;
    return 0; // TODO
}

/*
 * @brief TODO
 *
 * The Write Volatile Enhanced Configuration register (WRVECR) instruction
 * allows new values to be written to the Volatile Enhanced Configuration
 * register. Before it can be accepted, a write enable (WREN) instruction must
 * previously have been executed. After the write enable (WREN) instruction has
 * been decoded and executed, the device sets the write enable latch (WEL).
 *
 * @param[in] *dev  A pointer to the configured device.
 * @param[in] val   The value to write into the volatile enhanced register.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
static inline void _write_vol_enh_reg(n25q128_dev_t *dev, uint8_t val)
{
    (void)*dev;
    (void)val;
    // TODO
}

/*
 * @brief   Reads the content of the "Read Flag Status"-register.
 *
 * The Read Flag Status Register (RFSR) instruction allows the Flag Status
 * Register to be read. The Status Register may be read at any time, even while
 * a Program or Erase is in progress. When one of these cycles is in progress,
 * it is recommended to check the P/E Controller bit (Not WIP) bit before
 * sending a new instruction to the device. It is also possible to read the
 * Flag Register continuously.
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[out] *buf     Stores the status from the memory device.
 * @param[in] buf_len   The length of the *buf.
 *
 * @return  The content from the flag status register.
 */
static inline uint8_t _read_flag_status(n25q128_dev_t *dev)
{
    (void)*dev;
    return 0; // TODO
}

/*
 * @brief
 *
 * The Clear Flag Status Register (CLFSR) instruction reset the error Flag
 * Status Register bits (Erase Error bit, Program Error bit, VPP Error bit,
 * Protection Error bit). It is not necessary to set the WEL bit before the
 * Clear Flag Status Register instruction is executed. The WEL bit will be
 * unchanged after this command is executed.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
static inline void _clear_flag_status(n25q128_dev_t *dev)
{
    (void)*dev;
    // TODO
}

/*
 * @brief   Builds and send a packet, with an opcode included.
 */
static inline void _instruction_set(n25q128_dev_t *dev)
{
    /* The instruction 'set', including operation code and address. */
    uint8_t set[CMD_LEN_4_BYTE] = {0};

    set[0] = (dev->cmd.code & 0xFF);
    /* The address is made of 3 Bytes, which needs to be shifted correctly. */
    set[1] = ((dev->cmd.addr & 0x00FF0000) >> 16); // >> 0x000000FF
    set[2] = ((dev->cmd.addr & 0x0000FF00) >> 8);  // >> 0x000000FF
    set[3] = (dev->cmd.addr & 0x000000FF);         // >> 0x000000FF

    /* We need to send it here, to start the corresponding program in the device. */
    _send(dev->conf.bus, set, dev->cmd.len);
}

/*
 * @brief   Execute the configured device with the prepared instruction set.
 */
static inline int _execute(n25q128_dev_t *dev)
{
    /* Select the configured memory device, via the Chip-Select-GPIO. */
    gpio_clear(dev->conf.cs);

    /* Acquire the configured spi resource. */
    if (spi_acquire(dev->conf.bus, dev->conf.cs, dev->conf.mode, dev->conf.clk) < 0) {
        return N25Q128_NOSPI;
    }

    /* Prepare and send the instruction set, to start a program on the device. */
    _instruction_set(dev);

    /* Switch between operation codes and send or receive, depending on the code. */
    switch (dev->cmd.code) {

        case N25Q128_OPCODE_RDID:
            _receive(dev->conf.bus, dev->data.buf, dev->data.len);
            break;

        case N25Q128_OPCODE_RFSR:
            _receive(dev->conf.bus, dev->data.buf, dev->data.len);
            break;

        case N25Q128_OPCODE_READ:
            _receive(dev->conf.bus, dev->data.buf, dev->data.len);
            break;

        case N25Q128_OPCODE_PP:
            _send(dev->conf.bus, dev->data.buf, dev->data.len);
            break;

        // TODO: Add all other opcodes.
    }

    /* Release the configured spi resource. */
    spi_release(dev->conf.bus);

    /* Deselect the configured memory device, via the Chip-Select-GPIO. */
    gpio_set(dev->conf.cs);

    return N25Q128_OK;
}

/*
 * @brief   Process routine for all instruction sets.
 */
static inline int _process(n25q128_dev_t *dev, uint8_t code, int32_t addr,
        uint8_t * buf, uint8_t buf_len, int min_bytes, int max_bytes)
{
    /* Only buffer with a specific size are allowed. */
    if (!(buf_len >= min_bytes && (buf_len <= max_bytes || max_bytes == INFINITE))) {
        return N25Q128_OOR;
    }

    dev->cmd.code = code;
    dev->cmd.addr = addr;
    /* Only 1 Byte instruction set instead of 4, when no address is defined. */
    dev->cmd.len = (addr == ADDR_UNDEF) ? CMD_LEN_1_BYTE : CMD_LEN_4_BYTE;
    dev->data.buf = buf;
    dev->data.len = buf_len;

    return _execute(dev);
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

int n25q128_read_id(n25q128_dev_t *dev, uint8_t *buf, uint8_t buf_len)
{
    return _process(dev, N25Q128_OPCODE_RDID, ADDR_UNDEF, buf, buf_len, 1, 20);
}

int n25q128_read_data_bytes(n25q128_dev_t *dev, int32_t addr, uint8_t *buf, uint8_t buf_len)
{
    return _process(dev, N25Q128_OPCODE_READ, addr, buf, buf_len, 1, INFINITE);
}

int n25q128_read_fast_data_bytes(n25q128_dev_t *dev, int32_t addr, uint8_t *buf, uint8_t buf_len)
{
    return _process(dev, N25Q128_OPCODE_FAST_READ, addr, buf, buf_len, 1, INFINITE);
}

int n25q128_page_program(n25q128_dev_t *dev, int32_t addr, uint8_t *buf, uint8_t buf_len)
{
    return _process(dev, N25Q128_OPCODE_PP, addr, buf, buf_len, 1, 256);
}

int n25q128_read_otp(n25q128_dev_t *dev, uint8_t *buf, uint8_t buf_len)
{
    return _process(dev, N25Q128_OPCODE_ROTP, ADDR_UNDEF, buf, buf_len, 1, 65);
}

int n25q128_sector_erase(n25q128_dev_t *dev, int32_t addr)
{
    return _process(dev, N25Q128_OPCODE_SE, addr, NULL, 0, 0, 0);
}

int n25q128_bulk_erase(n25q128_dev_t *dev)
{
    return _process(dev, N25Q128_OPCODE_BE, ADDR_UNDEF, NULL, 0, 0, 0);
}

int n25q128_program_erase_suspend(n25q128_dev_t *dev)
{
    return _process(dev, N25Q128_OPCODE_PES, ADDR_UNDEF, NULL, 0, 0, 0);
}

int n25q128_program_erase_resume(n25q128_dev_t *dev)
{
    return _process(dev, N25Q128_OPCODE_PER, ADDR_UNDEF, NULL, 0, 0, 0);
}
