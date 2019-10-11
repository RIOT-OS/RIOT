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

/**
 * @brief   Status codes used by the N25Q128 driver interface.
 */
enum {
    N25Q128_OK      =  0,   /**< Everything went as planned. */
    N25Q128_OOR     = -1,   /**< Buffer-size: Out-Of-Range. */
    N25Q128_NOSPI   = -2,   /**< No SPI device available. */
    N25Q128_NOGPIO  = -3,   /**< NO GPIO available */
};

/*
 * @brief   Default type definiton of a instruction for a N25Q128.
 */
typedef struct {
    uint8_t code;       /*< The code of the instruction. */
    int32_t addr;       /*< The address to read or write data from. */
    uint8_t len;        /*< The length of code + addr in Bytes. */
} n25q128_cmd_t;

/*
 * @brief   Default type definition for the data of a N25Q128.
 */
typedef struct {
    uint8_t *buf;       /*< The buffer for sending or receiving data. */
    uint8_t len;        /*< The length of the buffer in Bytes. */
} n25q128_data_t;

/*
 * @brief   Default type definition for the configuration of an N25Q128.
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
 * @brief   Default type definition for a N25Q128 device.
 */
typedef struct {
    n25q128_conf_t conf;    /*< The configuration (SPI/GPIO) of the device. */
    n25q128_cmd_t cmd;      /*< The current instruction set for the device. */
    n25q128_data_t data;    /*< The in/out data for the current instruction. */
} n25q128_dev_t;

/*
 * @brief   Initialize the N25Q128 with all needed peripherals.
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
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_read_id(n25q128_dev_t *dev, uint8_t *buf, uint8_t buf_len);

/*
 * @brief   Read a specific amount of data bytes from an address.
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[in] address   The address to read bytes from.
 * @param[out] *buf     Stores the bytes from the address of the device.
 * @param[in] buf_len   The length of the *buf.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_read_data_bytes(n25q128_dev_t *dev, int32_t addr, uint8_t *buf, uint8_t buf_len);

/*
 * @brief   Read data bytes at higher speed.
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[in] address   The address to read bytes from.
 * @param[out] *buf     Stores the bytes from the address of the device.
 * @param[in] buf_len   The length of the *buf.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_read_fast_data_bytes(n25q128_dev_t *dev, int32_t addr, uint8_t *buf, uint8_t buf_len);

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
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_page_program(n25q128_dev_t *dev, int32_t addr, uint8_t *buf, uint8_t buf_len);

/*
 * @brief   TODO: Description.
 *
 * The Program OTP instruction (POTP) is used to program at most 64 bytes to the
 * OTP memory area (by changing bits from 1 to 0, only). Before it can be
 * accepted, a Write Enable (WREN) instruction must previously have been
 * executed. After the Write Enable (WREN) instruction has been decoded, the
 * device sets the Write Enable Latch (WEL) bit.
 *
 * @param[in] *dev      A pointer to the configured device.
 * @param[out] *buf     Stores the bytes from the address of the device.
 * @param[in] buf_len   The length of the *buf.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_read_otp(n25q128_dev_t *dev, uint8_t *buf, uint8_t buf_len);

/*
 * @brief   Erases all bits (set to 1 (FFh)) inside the chosen sector.
 *
 * The Sector Erase (SE) instruction sets to '1' (FFh) all bits inside the
 * chosen sector. Before itcan be accepted, a Write Enable (WREN) instruction
 * must previously have been executed.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_sector_erase(n25q128_dev_t *dev, int32_t addr);

/*
 * @brief   Erases the whole device.
 *
 * The Bulk Erase (BE) instruction sets all bits to '1' (FFh). Before it can be
 * accepted, a Write Enable (WREN) instruction must previously have been
 * executed. After the Write Enable (WREN) instruction has been decoded, the
 * device sets the Write Enable Latch (WEL).
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_bulk_erase(n25q128_dev_t *dev);

/*
 * @brief
 *
 * The Program/Erase Suspend instruction allows the controller to interrupt a
 * Program or an Erase instruction, in particular: Sector Erase, Subsector
 * Erase, Page Program, Dual Input Page Program, Dual Input Extended Page
 * program, Quad Input Page Program and Quad Input Extended Page program can be
 * suspended and resumed.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_program_erase_suspend(n25q128_dev_t *dev);

/*
 * @brief
 *
 * After a Program/Erase suspend instruction, a Program/Erase Resume instruction
 * is required to continue performing the suspended Program or Erase sequence.
 * Program/Erase Resume instruction is ignored if the device is not in a
 * Program/Erase Suspended status. The WIP bit of the Status Register and
 * Program/Erase controller bit (NotWIP) of the Flag Status Register both switch
 * to the busy state (1 and 0 respectively) after Program/Erase Resume
 * instruction until the Program or Erase sequence is completed.
 *
 * @return  A status code, depending on the state of the device/driver.
 */
int n25q128_program_erase_resume(n25q128_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* N25Q128_H */
/** @} */
