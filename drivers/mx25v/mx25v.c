/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mx25v
 * @{
 *
 * @file
 * @brief       Device driver implementation for the mx25v flash memory
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 *
 * @}
 */
#include <stddef.h>

#include "board.h"
#include "xtimer.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "timex.h"
#include "mx25v.h"

/**
 * @brief MX25V instruction code
 * @{
 */
#define MX25V_RDID          (uint8_t)(0x9F) /**< Read Identification */
#define MX25V_RES           (uint8_t)(0xAB) /**< Read Electronic ID */
#define MX25V_REMS          (uint8_t)(0x90) /**< Read Electronic & Device ID */
#define MX25V_WRSR          (uint8_t)(0x01) /**< Write Status Register */
#define MX25V_RDSR          (uint8_t)(0x05) /**< Read Status Register */
#define MX25V_READ          (uint8_t)(0x03) /**< 1 x I/O */
#define MX25V_FASTREAD      (uint8_t)(0x0B) /**< Fast read data */
#define MX25V_DREAD         (uint8_t)(0x3B) /**< 1In/2 Out fast read */
#define MX25V_RDSFDP        (uint8_t)(0x5A) /**< Read SFDP */
#define MX25V_WREN          (uint8_t)(0x06) /**< Write Enable */
#define MX25V_WRDI          (uint8_t)(0x04) /**< Write Disable */
#define MX25V_PP            (uint8_t)(0x02) /**< page program */
#define MX25V_SE            (uint8_t)(0x20) /**< Sector Erase */
#define MX25V_BE            (uint8_t)(0xD8) /**< Block Erase */
#define MX25V_CE            (uint8_t)(0x60) /**< Chip Erase) hex code: 60 or C7 */
#define MX25V_DP            (uint8_t)(0xB9) /**< Deep Power Down */
#define MX25V_RDP           (uint8_t)(0xAB) /**< Release form Deep Power Down */
/** @} */

/** Timeout value for waiting operation done */
#define MX25V_WRITE_STATUS_TIME (uint32_t)(5 * MS_IN_USEC)    /* write status time: TYP. 5  ms */
#define MX25V_SECTOR_ERASE_TIME (uint32_t)(60 * MS_IN_USEC)   /* sector erase time: TYP. 60 ms */
#define MX25V_BLOCK_ERASE_TIME  (uint32_t)(1 * SEC_IN_USEC)   /* block erase time:  TYP. 1  s */
#define MX25V_CHIP_ERASE_TIME   (uint32_t)(15 * SEC_IN_USEC)  /* chip erase time:   TYP. 7  s */
#define MX25V_PAGE_PROG_TIME    (uint32_t)(1400)              /* page program time: TYP. 1.4ms */

/**
 * @brief Send command to set or reset WEL bit.
 *
 * The WREN instruction is for setting Write Enable Latch (WEL) bit.
 * The WRDI instruction is to reset Write Enable Latch (WEL) bit.
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 * @param[in] cmd           WREN or WRDI command
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 */
static int mx25v_cmd_write_latch(mx25v_t *mx25v_dev, char cmd)
{
    int status;

    status = spi_transfer_byte(mx25v_dev->spi_dev, cmd, NULL);
    if (status != 1) {
        return -1;
    }

    return 0;
}

/**
 * @brief Check if mx25v is busy.
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 *
 * @return                  1 flash is busy (WIP bit = 1)
 * @return                  0 flash is ready (WIP bit = 0)
 */
static uint8_t mx25v_is_flash_busy(mx25v_t *mx25v_dev)
{
    char mx25v_status;

    mx25v_read_status(mx25v_dev, &mx25v_status);
    if (mx25v_status & MX25V_WIP) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Wait mx25v ready with timeout
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 * @param[in] time_to_wait  typical time to wait for the operation
 *
 * @return                  1 flash is ready
 * @return                  0 flash is time-out
 */
static uint8_t mx25v_wait_ready(mx25v_t *mx25v_dev, uint32_t time_to_wait)
{
    uint32_t start_time = xtimer_now();
    /* the timeout is reached at 2 times of the typical operation time */
    uint32_t timeout = start_time + (2 * time_to_wait);
    uint32_t time = start_time;

    /* sleep during typical time operation */
    xtimer_usleep(time_to_wait);

    /* wait for device ready or timeout reached */
    while (mx25v_is_flash_busy(mx25v_dev) == 1 && time < timeout) {
        xtimer_usleep(time_to_wait / 2);
        time = xtimer_now();
    }

    /* failed if timeout is reached */
    if (time >= timeout) {
        return 0;
    }

    return 1;
}

int mx25v_init(mx25v_t *mx25v_dev)
{
    int status;

    /* init spi */
    status = spi_init_master(mx25v_dev->spi_dev, mx25v_dev->conf, mx25v_dev->speed);
    if (status != 0) {
        return status;
    }

    /* fail if no chip select pin */
    if (mx25v_dev->cs_pin == 0) {
        return -3;
    }

    /* configure chip select pin */
    gpio_init(mx25v_dev->cs_pin, GPIO_OUT);
    gpio_set(mx25v_dev->cs_pin);

    /* configure hold pin if defined */
    if (mx25v_dev->hold_pin != 0) {
        gpio_init(mx25v_dev->hold_pin, GPIO_OUT);
        gpio_set(mx25v_dev->hold_pin);
    }

    /* configure wp pin if defined */
    if (mx25v_dev->wp_pin != 0) {
        gpio_init(mx25v_dev->wp_pin, GPIO_OUT);
        gpio_set(mx25v_dev->wp_pin);
    }

    return 0;
}

int mx25v_read_id(mx25v_t *mx25v_dev, mx25v_id_t *id)
{
    int status;
    char rx_buf[3];

    spi_acquire(mx25v_dev->spi_dev);
    gpio_clear(mx25v_dev->cs_pin);

    status = spi_transfer_regs(mx25v_dev->spi_dev, MX25V_RDID, NULL, rx_buf, 3);

    gpio_set(mx25v_dev->cs_pin);
    spi_release(mx25v_dev->spi_dev);

    if (status != 3) {
        return -1;
    }
    id->manufacturer_id = rx_buf[0];
    id->memory_id = rx_buf[1];
    id->device_id = rx_buf[2];

    return 0;
}

int mx25v_read_status(mx25v_t *mx25v_dev, char *value)
{
    int status;
    char data;

    spi_acquire(mx25v_dev->spi_dev);
    gpio_clear(mx25v_dev->cs_pin);

    status = spi_transfer_reg(mx25v_dev->spi_dev, MX25V_RDSR, 0, &data);

    gpio_set(mx25v_dev->cs_pin);
    spi_release(mx25v_dev->spi_dev);

    if (status != 1) {
        return -1;
    }

    value[0] = data;

    return 0;
}

int mx25v_write_status(mx25v_t *mx25v_dev, char value)
{
    int status;

    if (mx25v_is_flash_busy(mx25v_dev) == 1) {
        return -3;
    }

    spi_acquire(mx25v_dev->spi_dev);

    gpio_clear(mx25v_dev->cs_pin);
    mx25v_cmd_write_latch(mx25v_dev, MX25V_WREN);
    gpio_set(mx25v_dev->cs_pin);

    gpio_clear(mx25v_dev->cs_pin);
    status = spi_transfer_reg(mx25v_dev->spi_dev, MX25V_WRSR, value, NULL);
    gpio_set(mx25v_dev->cs_pin);

    spi_release(mx25v_dev->spi_dev);

    if (status != 1) {
        return -1;
    }

    if (mx25v_wait_ready(mx25v_dev, MX25V_WRITE_STATUS_TIME) == 0) {
        return -4;
    }

    return 0;
}

int mx25v_read_buffer(mx25v_t *mx25v_dev, uint32_t address, char *data,
                      uint32_t length)
{
    int status;
    char tx_buf[3] = {
        (char)(address >> 16),
        (char)(address >> 8),
        (char)(address)
    };

    if ((address + length) > MX25V_FLASH_SIZE) {
        return -2;
    }

    spi_acquire(mx25v_dev->spi_dev);
    gpio_clear(mx25v_dev->cs_pin);

    status = spi_transfer_regs(mx25v_dev->spi_dev, MX25V_READ, tx_buf, NULL, 3);

    if (status != 3) {
        gpio_set(mx25v_dev->cs_pin);
        spi_release(mx25v_dev->spi_dev);
        return -1;
    }

    status = spi_transfer_bytes(mx25v_dev->spi_dev, NULL, data, length);

    gpio_set(mx25v_dev->cs_pin);
    spi_release(mx25v_dev->spi_dev);

    if (status != length) {
        return -1;
    }

    return 0;
}

int mx25v_write_buffer(mx25v_t *mx25v_dev, uint32_t address, const unsigned char *data,
                       uint32_t length)
{
    int status;
    uint32_t write_address;
    uint32_t write_length;
    char *data_wr = (char *)data;
    char tx_buf[3];

    if ((address + length) > MX25V_FLASH_SIZE) {
        return -2;
    }

    if (mx25v_is_flash_busy(mx25v_dev) == 1) {
        return -3;
    }

    write_address = address;
    write_length = MX25V_PAGE_SIZE - (address % MX25V_PAGE_SIZE);
    if (write_length) {
        spi_acquire(mx25v_dev->spi_dev);

        /* send WREN bit to initiate write instruction */
        gpio_clear(mx25v_dev->cs_pin);
        mx25v_cmd_write_latch(mx25v_dev, MX25V_WREN);
        gpio_set(mx25v_dev->cs_pin);

        /* write page program command */
        tx_buf[0] = (uint8_t)(write_address >> 16);
        tx_buf[1] = (uint8_t)(write_address >> 8);
        tx_buf[2] = (uint8_t)(write_address);

        gpio_clear(mx25v_dev->cs_pin);
        status = spi_transfer_regs(mx25v_dev->spi_dev, MX25V_PP, tx_buf, NULL, 3);
        if (status != 3) {
            gpio_set(mx25v_dev->cs_pin);
            spi_release(mx25v_dev->spi_dev);
            return -1;
        }

        /* write page data into flash */
        status = spi_transfer_bytes(mx25v_dev->spi_dev, data_wr, NULL, write_length);

        gpio_set(mx25v_dev->cs_pin);
        spi_release(mx25v_dev->spi_dev);

        if (status != write_length) {
            return -1;
        }

        if (mx25v_wait_ready(mx25v_dev, MX25V_PAGE_PROG_TIME) == 0) {
            return -4;
        }

        write_address += write_length;
        data_wr += write_length;
        length -= write_length;
    }

    while (length) {
        spi_acquire(mx25v_dev->spi_dev);

        /* write_length takes the minimum value between */
        /* MX25V_PAGE_SIZE and length */
        if (MX25V_PAGE_SIZE > length) {
            write_length = length;
        }
        else {
            write_length = MX25V_PAGE_SIZE;
        }

        /* send WREN bit to initiate write instruction */
        gpio_clear(mx25v_dev->cs_pin);
        mx25v_cmd_write_latch(mx25v_dev, MX25V_WREN);
        gpio_set(mx25v_dev->cs_pin);

        /* write page program command */
        tx_buf[0] = (uint8_t)(write_address >> 16);
        tx_buf[1] = (uint8_t)(write_address >> 8);
        tx_buf[2] = (uint8_t)(write_address);

        gpio_clear(mx25v_dev->cs_pin);
        status = spi_transfer_regs(mx25v_dev->spi_dev, MX25V_PP, tx_buf, NULL, 3);
        if (status != 3) {
            gpio_set(mx25v_dev->cs_pin);
            spi_release(mx25v_dev->spi_dev);
            return -1;
        }

        /* write page data into flash */
        status = spi_transfer_bytes(mx25v_dev->spi_dev, data_wr, NULL, write_length);

        gpio_set(mx25v_dev->cs_pin);
        spi_release(mx25v_dev->spi_dev);

        if (status != write_length) {
            return -1;
        }

        if (mx25v_wait_ready(mx25v_dev, MX25V_PAGE_PROG_TIME) == 0) {
            return -4;
        }

        write_address += write_length;
        data_wr += write_length;
        length -= write_length;
    }

    return 0;
}

int mx25v_erase_sector(mx25v_t *mx25v_dev, uint32_t address)
{
    int status;
    char tx_buf[3] = {
        (char)(address >> 16),
        (char)(address >> 8),
        (char)(address)
    };

   if ((address) >= MX25V_FLASH_SIZE) {
        return -2;
    }

    if (mx25v_is_flash_busy(mx25v_dev) == 1) {
        return -3;
    }

    spi_acquire(mx25v_dev->spi_dev);

    gpio_clear(mx25v_dev->cs_pin);
    mx25v_cmd_write_latch(mx25v_dev, MX25V_WREN);
    gpio_set(mx25v_dev->cs_pin);

    gpio_clear(mx25v_dev->cs_pin);
    status = spi_transfer_regs(mx25v_dev->spi_dev, MX25V_SE, tx_buf, NULL, 3);
    gpio_set(mx25v_dev->cs_pin);

    spi_release(mx25v_dev->spi_dev);

    if (status != 3) {
        return -1;
    }

    if (mx25v_wait_ready(mx25v_dev, MX25V_SECTOR_ERASE_TIME) == 0) {
        return -4;
    }
    return 0;
}

int mx25v_erase_block(mx25v_t *mx25v_dev, uint32_t address)
{
    int status;
    char tx_buf[3] = {
        (char)(address >> 16),
        (char)(address >> 8),
        (char)(address)
    };

    if ((address) >= MX25V_FLASH_SIZE) {
        return -2;
    }

    if (mx25v_is_flash_busy(mx25v_dev) == 1) {
        return -3;
    }

    spi_acquire(mx25v_dev->spi_dev);

    gpio_clear(mx25v_dev->cs_pin);
    mx25v_cmd_write_latch(mx25v_dev, MX25V_WREN);
    gpio_set(mx25v_dev->cs_pin);

    gpio_clear(mx25v_dev->cs_pin);
    status = spi_transfer_regs(mx25v_dev->spi_dev, MX25V_BE, tx_buf, NULL, 3);
    gpio_set(mx25v_dev->cs_pin);

    spi_release(mx25v_dev->spi_dev);

    if (status != 3) {
        return -1;
    }

    if (mx25v_wait_ready(mx25v_dev, MX25V_BLOCK_ERASE_TIME) == 0) {
        return -4;
    }

    return 0;
}

int mx25v_erase_chip(mx25v_t *mx25v_dev)
{
    int status;

    if (mx25v_is_flash_busy(mx25v_dev) == 1) {
        return -3;
    }

    spi_acquire(mx25v_dev->spi_dev);

    gpio_clear(mx25v_dev->cs_pin);
    mx25v_cmd_write_latch(mx25v_dev, MX25V_WREN);
    gpio_set(mx25v_dev->cs_pin);

    gpio_clear(mx25v_dev->cs_pin);
    status = spi_transfer_byte(mx25v_dev->spi_dev, MX25V_CE, NULL);
    gpio_set(mx25v_dev->cs_pin);

    spi_release(mx25v_dev->spi_dev);

    if (status != 1) {
        return -1;
    }

    if (mx25v_wait_ready(mx25v_dev, MX25V_CHIP_ERASE_TIME) == 0) {
        return -4;
    }

    return 0;
}


int mx25v_enter_deep_powerdown(mx25v_t *mx25v_dev)
{
    int status;

    spi_acquire(mx25v_dev->spi_dev);

    gpio_clear(mx25v_dev->cs_pin);
    status = spi_transfer_byte(mx25v_dev->spi_dev, MX25V_DP, NULL);
    gpio_set(mx25v_dev->cs_pin);

    spi_release(mx25v_dev->spi_dev);

    if (status != 1) {
        return -1;
    }

    return 0;
}

int mx25v_exit_deep_powerdown(mx25v_t *mx25v_dev)
{
    int status;

    spi_acquire(mx25v_dev->spi_dev);

    gpio_clear(mx25v_dev->cs_pin);
    status = spi_transfer_byte(mx25v_dev->spi_dev, MX25V_RDP, NULL);
    gpio_set(mx25v_dev->cs_pin);

    spi_release(mx25v_dev->spi_dev);

    if (status != 1) {
        return -1;
    }

    return 0;
}
