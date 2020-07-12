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
 * @brief       Header for the Driver for the N25Q128 serial flash memory
 *
 * @author      René Herthel <rene-herthel@haw-hamburg.de>
 *
 * @}
 */

#ifndef N25Q128_H
#define N25Q128_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "periph/spi.h"
#include "periph/gpio.h"

/*
 * @brief   Status codes used by the N25Q128 driver interface.
 */
enum {
    N25Q128_OK      =  0,   /**< Everything went as planned. */
    N25Q128_NOSPI   = -1,   /**< No SPI device available. */
    N25Q128_NOGPIO  = -2,   /**< NO GPIO available */
};

/*
 * @brief   Helper for accessing/setting the bits of the 'n25q128_opt_t' type.
 * @{
 */
#define N25Q128_OPT_ADDR_EN     (0x01)
#define N25Q128_OPT_INF_EN      (0x02)
#define N25Q128_OPT_SEND_EN     (0x04)
#define N25Q128_OPT_RECV_EN     (0x08)
/** }@ */

/*
 *@brief    Additional options, for specific instructions.
 */
typedef union {
    uint8_t mask;
    struct {
        uint8_t addr    : 1;   /*< Set, when an address is available. */
        uint8_t inf     : 1;   /*< Set, when the memory is able to continously send */
        uint8_t send    : 1;   /*< Set, when the memory is receiving. */
        uint8_t recv    : 1;   /*< Set, when the memory is sending. */
        uint8_t unused  : 4;   /*< Not used bits. */
    } enabled;
} n25q128_opt_t;

/*
 * @brief   Type definiton of a instruction for a N25Q128.
 */
typedef struct {
    uint8_t code;       /*< The code of the instruction. */
    int32_t addr;       /*< The address to read or write data from. */
} n25q128_cmd_t;

/*
 * @brief   Type definition for the data of a N25Q128.
 */
typedef struct {
    uint8_t *buf;       /*< The buffer for sending or receiving data. */
    size_t len;         /*< The length of the buffer in Bytes. */
} n25q128_data_t;

/*
 * @brief   Type definition for the configuration of an N25Q128.
 */
typedef struct {
    spi_t bus;          /*< The used SPI bus */
    spi_mode_t mode;    /*< The mode of the SPI */
    spi_clk_t clk;      /*< The clock of the SPI */
    gpio_t cs;          /*< The Chip-Select GPIO */
    gpio_t write;       /*< The Write GPIO */
    gpio_t hold;        /*< The Hold GPIO */
} n25q128_conf_t;

/*
 * @brief   Type definition for a N25Q128 device.
 *
 */
typedef struct {
    n25q128_conf_t conf;    /*< The configuration (SPI/GPIO) of the device. */
    n25q128_cmd_t cmd;      /*< The current instruction set for the device. */
    n25q128_data_t data;    /*< The in/out data for the current instruction. */
    n25q128_opt_t opt;      /*< bitwise-options, for enabling additional functions. */
} n25q128_dev_t;

/*
 * @brief   Initialize the N25Q128 with all needed peripherals.
 *
 * NOTE:    To initialize a device, it is only required to assign the peripheral
 *          parameters to the configuration type (n25q128_conf_t). The other
 *          structures must not be modified.
 *
 * @param[in] *dev  A pointer to the configured device.
 */
int n25q128_init(n25q128_dev_t *dev);

/*
 * @brief   Reads the content of the "Read Identification"-register.
 *
 * The Read Identification (RDID) instruction allows to read the device
 * identification data:
 * > Manufacturer identification (1 byte)
 * > Device identification (2 bytes)
 * > A Unique ID code (UID) (17 bytes, of which 14 factory programmed upon
 * customer request).
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[out] *buf     Stores the id from the memory device.
 * @param[in] buf_len   The length of the *buf.
 */
void n25q128_read_id(n25q128_dev_t *dev, uint8_t *buf, size_t len);

/*
 * @brief   Read a specific amount of data bytes from an address.
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[in] address   The address to read bytes from.
 * @param[out] *buf     Stores the bytes from the address of the device.
 * @param[in] buf_len   The length of the *buf.
 */
void n25q128_read_data_bytes(n25q128_dev_t *dev, uint32_t addr, uint8_t *buf, size_t len);

/*
 * @brief   Programs bytes into the memory.
 *
 * The Page Program (PP) instruction allows bytes to be programmed in the memory
 * (changing bits from 1 to 0). Before it can be accepted, a Write Enable (WREN)
 * instruction must previously have been executed. After the Write Enable (WREN)
 * instruction has been decoded, the device sets the Write Enable Latch (WEL).
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[in] address   The address to read bytes from.
 * @param[out] *buf     Stores the bytes from the address of the device.
 * @param[in] buf_len   The length of the *buf.
 */
void n25q128_page_program(n25q128_dev_t *dev, uint32_t addr, uint8_t *buf, size_t len);

/*
 * @brief   Erases all bits (set to 1 (FFh)) inside the chosen sector.
 *
 * NOTE: This needs to be done, before programming a sector with new values.
 *       Because the memory can only set bits back to 1 during erasing. So, once
 *       a bit is programmed from 1 to 0, it cannot be 'overwritten'.
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[in] addr      The address of the sector to erase.
 */
void n25q128_sector_erase(n25q128_dev_t *dev, uint32_t addr);

/*
 * @brief   Erases the whole device.
 *
 * The Bulk Erase (BE) instruction sets all bits to '1' (FFh). Before it can be
 * accepted, a Write Enable (WREN) instruction must previously have been
 * executed. After the Write Enable (WREN) instruction has been decoded, the
 * device sets the Write Enable Latch (WEL).
 */
void n25q128_bulk_erase(n25q128_dev_t *dev);

/*
 * @brief   Pause the program/erase program on the memory.
 *
 */
void n25q128_program_erase_suspend(n25q128_dev_t *dev);

/*
 * @brief   Resume the program/erase program on the memory.
 *
 * @return  Status code used by the N25Q128 driver interface
 */
void n25q128_program_erase_resume(n25q128_dev_t *dev);

/*
 * @brief   Check if there is a write in progress.
 *
 * @return  The Write In Progress (WIP) bit set to 1 indicates that the memory
 *          is busy with a Write Status Register, Program or Erase cycle.
 *          0 indicates no cycles is in progress.
 */
bool n25q128_write_in_progress(n25q128_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* N25Q128_H */
/** @} */
