/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdint.h>
#include <stddef.h>
#include "nvram.h"
#include "nvram-spi.h"
#include "byteorder.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "ztimer.h"

/**
 * @ingroup     drivers_nvram
 * @{
 *
 * @file
 *
 * @brief       Device interface for various SPI connected NVRAM.
 *
 * Tested on:
 * - Cypress/Ramtron FM25L04B.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

typedef enum {
    /** WRITE command byte, 0b0000 0010 */
    NVRAM_SPI_CMD_WRITE = 0x02,
    /** READ command byte, 0b0000 0011 */
    NVRAM_SPI_CMD_READ = 0x03,
    /** WREN command byte, 0b0000 0110 */
    NVRAM_SPI_CMD_WREN = 0x06,
} nvram_spi_commands_t;

/** @brief Delay to wait between toggling CS pin, on most chips this can probably be
 * removed. */
#define NVRAM_SPI_CS_TOGGLE_US      1

/**
 * @brief Copy data from system memory to NVRAM.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  src   Pointer to the first byte in the system memory address space
 * @param[in]  dst   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return           Number of bytes written on success
 * @return           <0 on errors
 */
static int nvram_spi_write(nvram_t *dev, const uint8_t *src, uint32_t dst, size_t len);

/**
 * @brief Copy data from NVRAM to system memory.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[out] dst   Pointer to the first byte in the system memory address space
 * @param[in]  src   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return           Number of bytes read on success
 * @return           <0 on errors
 */
static int nvram_spi_read(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len);

/**
 * @brief Copy data from system memory to NVRAM.
 *
 * This is a special form of the WRITE command used by some Ramtron/Cypress
 * 4Kbit FRAM devices which puts the 9th address bit inside the command byte to
 * be able to use one byte for addressing instead of two.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  src   Pointer to the first byte in the system memory address space
 * @param[in]  dst   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return           Number of bytes written on success
 * @return           <0 on errors
 */
static int nvram_spi_write_9bit_addr(nvram_t *dev, const uint8_t *src, uint32_t dst, size_t len);

/**
 * @brief Copy data from NVRAM to system memory.
 *
 * This is a special form of the READ command used by some Ramtron/Cypress 4Kbit
 * FRAM devices which puts the 9th address bit inside the command byte to be
 * able to use one byte for addressing instead of two.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[out] dst   Pointer to the first byte in the system memory address space
 * @param[in]  src   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return           Number of bytes read on success
 * @return           <0 on errors
 */
static int nvram_spi_read_9bit_addr(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len);

int nvram_spi_init(nvram_t *dev, nvram_spi_params_t *spi_params, size_t size)
{
    dev->size = size;
    if (size > 0x100 && spi_params->address_count == 1) {
        dev->write = nvram_spi_write_9bit_addr;
        dev->read = nvram_spi_read_9bit_addr;
    } else {
        dev->write = nvram_spi_write;
        dev->read = nvram_spi_read;
    }
    dev->extra = spi_params;

    if (spi_init_cs(spi_params->spi, spi_params->cs) != SPI_OK) {
        return -1;
    }

    return 0;
}

static int nvram_spi_write(nvram_t *dev, const uint8_t *src, uint32_t dst, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    union {
        uint32_t u32;
        char c[4];
    } addr;

    /* Address is expected by the device as big-endian, i.e. network byte order,
     * we utilize the network byte order macros here. */
    addr.u32 = htonl(dst);

    /* Acquire exclusive bus access while configuring clock and mode */
    spi_acquire(spi_dev->spi, spi_dev->cs, SPI_MODE_0, spi_dev->clk);
    /* Enable writes */
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, false, NVRAM_SPI_CMD_WREN);
    /* Make sure we have a minimum gap between transfers */
    ztimer_spin(ZTIMER_USEC, NVRAM_SPI_CS_TOGGLE_US);
    /* Write command and address */
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, true, NVRAM_SPI_CMD_WRITE);
    spi_transfer_bytes(spi_dev->spi, spi_dev->cs, true,
                      &addr.c[sizeof(addr.c) - spi_dev->address_count], NULL,
                      spi_dev->address_count);
    /* Write data (we still hold the CS line low in between) */
    spi_transfer_bytes(spi_dev->spi, spi_dev->cs, false, src, NULL, len);
    /* Release exclusive bus access */
    spi_release(spi_dev->spi);

    return (int)len;
}

static int nvram_spi_read(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    union {
        uint32_t u32;
        char c[4];
    } addr;
    /* Address is expected by the device as big-endian, i.e. network byte order,
     * we utilize the network byte order macros here. */
    addr.u32 = htonl(src);

    /* Acquire exclusive bus access while configuring clock and mode */
    spi_acquire(spi_dev->spi, spi_dev->cs, SPI_MODE_0, spi_dev->clk);
    /* Write command and address */
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, true, NVRAM_SPI_CMD_READ);
    spi_transfer_bytes(spi_dev->spi, spi_dev->cs, true,
                               &addr.c[sizeof(addr.c) - spi_dev->address_count],
                               NULL, spi_dev->address_count);
    /* Read data (while still holding the CS line active) */
    spi_transfer_bytes(spi_dev->spi, spi_dev->cs, false, NULL, dst, len);
    /* Release exclusive bus access */
    spi_release(spi_dev->spi);

    /* status contains the number of bytes actually read from the SPI bus. */
    return (int)len;
}

static int nvram_spi_write_9bit_addr(nvram_t *dev, const uint8_t *src, uint32_t dst, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    uint8_t cmd;
    uint8_t addr;
    cmd = NVRAM_SPI_CMD_WRITE;

    /* The upper address bit is mixed into the command byte on certain devices,
     * probably just to save a byte in the SPI transfer protocol. */
    if (dst > 0xff) {
        cmd |= 0x08;
    }
    /* LSB of address */
    addr = (dst & 0xff);

    spi_acquire(spi_dev->spi, spi_dev->cs, SPI_MODE_0, spi_dev->clk);
    /* Enable writes */
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, false, NVRAM_SPI_CMD_WREN);
    /* Insert needed delay between transactions */
    ztimer_spin(ZTIMER_USEC, NVRAM_SPI_CS_TOGGLE_US);
    /* Write command and address */
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, true, cmd);
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, true, addr);
    /* Keep holding CS and write data */
    spi_transfer_bytes(spi_dev->spi, spi_dev->cs, false, src, NULL, len);
    spi_release(spi_dev->spi);

    /* status contains the number of bytes actually written to the SPI bus. */
    return (int)len;
}

static int nvram_spi_read_9bit_addr(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    uint8_t cmd;
    uint8_t addr;
    cmd = NVRAM_SPI_CMD_READ;

    /* The upper address bit is mixed into the command byte on certain devices,
     * probably just to save a byte in the SPI transfer protocol. */
    if (src > 0xff) {
        cmd |= 0x08;
    }
    /* LSB of address */
    addr = (src & 0xff);

    spi_acquire(spi_dev->spi, spi_dev->cs, SPI_MODE_0, spi_dev->clk);
    /* Write command and address */
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, true, cmd);
    spi_transfer_byte(spi_dev->spi, spi_dev->cs, true, addr);
    /* Keep holding CS and read data */
    spi_transfer_bytes(spi_dev->spi, spi_dev->cs, false, NULL, dst, len);
    spi_release(spi_dev->spi);

    /* status contains the number of bytes actually read from the SPI bus. */
    return (int)len;
}

/** @} */
