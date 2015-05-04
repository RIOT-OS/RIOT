/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include <stddef.h>
#include "nvram.h"
#include "nvram-spi.h"
#include "byteorder.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "hwtimer.h"

/**
 * @ingroup     nvram
 * @{
 *
 * @file
 *
 * @brief       Device interface for various SPI connected NVRAM.
 *
 * Tested on:
 * - Cypress/Ramtron FM25L04B.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
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
#define NVRAM_SPI_CS_TOGGLE_TICKS 1

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
static int nvram_spi_write(nvram_t *dev, uint8_t *src, uint32_t dst, size_t len);

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
static int nvram_spi_write_9bit_addr(nvram_t *dev, uint8_t *src, uint32_t dst, size_t len);

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

    gpio_init_out(spi_params->cs, GPIO_NOPULL);
    gpio_set(spi_params->cs);

    return 0;
}

static int nvram_spi_write(nvram_t *dev, uint8_t *src, uint32_t dst, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    int status;
    union {
        uint32_t u32;
        char c[4];
    } addr;
    /* Address is expected by the device as big-endian, i.e. network byte order,
     * we utilize the network byte order macros here. */
    addr.u32 = HTONL(dst);
    /* Acquire exclusive bus access */
    spi_acquire(spi_dev->spi);
    /* Assert CS */
    gpio_clear(spi_dev->cs);
    /* Enable writes */
    status = spi_transfer_byte(spi_dev->spi, NVRAM_SPI_CMD_WREN, NULL);
    if (status < 0)
    {
        return status;
    }
    /* Release CS */
    gpio_set(spi_dev->cs);
    hwtimer_spin(NVRAM_SPI_CS_TOGGLE_TICKS);
    /* Re-assert CS */
    gpio_clear(spi_dev->cs);
    /* Write command and address */
    status = spi_transfer_regs(spi_dev->spi, NVRAM_SPI_CMD_WRITE,
                      &addr.c[sizeof(addr.c) - spi_dev->address_count], NULL,
                      spi_dev->address_count);
    if (status < 0)
    {
        return status;
    }
    /* Keep holding CS and write data */
    status = spi_transfer_bytes(spi_dev->spi, (char *)src, NULL, len);
    if (status < 0)
    {
        return status;
    }
    /* Release CS */
    gpio_set(spi_dev->cs);
    /* Release exclusive bus access */
    spi_release(spi_dev->spi);
    return status;
}

static int nvram_spi_read(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    int status;
    union {
        uint32_t u32;
        char c[4];
    } addr;
    /* Address is expected by the device as big-endian, i.e. network byte order,
     * we utilize the network byte order macros here. */
    addr.u32 = HTONL(src);
    /* Acquire exclusive bus access */
    spi_acquire(spi_dev->spi);
    /* Assert CS */
    gpio_clear(spi_dev->cs);
    /* Write command and address */
    status = spi_transfer_regs(spi_dev->spi, NVRAM_SPI_CMD_READ,
                               &addr.c[sizeof(addr.c) - spi_dev->address_count],
                               NULL, spi_dev->address_count);
    if (status < 0)
    {
        return status;
    }
    /* Keep holding CS and read data */
    status = spi_transfer_bytes(spi_dev->spi, NULL, (char *)dst, len);
    if (status < 0)
    {
        return status;
    }
    /* Release CS */
    gpio_set(spi_dev->cs);
    /* Release exclusive bus access */
    spi_release(spi_dev->spi);
    /* status contains the number of bytes actually read from the SPI bus. */
    return status;
}


static int nvram_spi_write_9bit_addr(nvram_t *dev, uint8_t *src, uint32_t dst, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    int status;
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
    spi_acquire(spi_dev->spi);
    gpio_clear(spi_dev->cs);
    /* Enable writes */
    status = spi_transfer_byte(spi_dev->spi, NVRAM_SPI_CMD_WREN, NULL);
    if (status < 0)
    {
        return status;
    }
    gpio_set(spi_dev->cs);
    hwtimer_spin(NVRAM_SPI_CS_TOGGLE_TICKS);
    gpio_clear(spi_dev->cs);
    /* Write command and address */
    status = spi_transfer_reg(spi_dev->spi, cmd, addr, NULL);
    if (status < 0)
    {
        return status;
    }
    /* Keep holding CS and write data */
    status = spi_transfer_bytes(spi_dev->spi, (char *)src, NULL, len);
    if (status < 0)
    {
        return status;
    }
    gpio_set(spi_dev->cs);
    spi_release(spi_dev->spi);
    /* status contains the number of bytes actually written to the SPI bus. */
    return status;
}

static int nvram_spi_read_9bit_addr(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len)
{
    nvram_spi_params_t *spi_dev = (nvram_spi_params_t *) dev->extra;
    int status;
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
    spi_acquire(spi_dev->spi);
    gpio_clear(spi_dev->cs);
    /* Write command and address */
    status = spi_transfer_reg(spi_dev->spi, (char)cmd, addr, NULL);
    if (status < 0)
    {
        return status;
    }
    /* Keep holding CS and read data */
    status = spi_transfer_bytes(spi_dev->spi, NULL, (char *)dst, len);
    if (status < 0)
    {
        return status;
    }
    gpio_set(spi_dev->cs);
    spi_release(spi_dev->spi);
    /* status contains the number of bytes actually read from the SPI bus. */
    return status;
}
