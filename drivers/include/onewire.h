/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_onewire 1-Wire Bus Driver
 * @ingroup     drivers_misc
 * @brief       Generic driver for 1-Wire bus
 *
 * # About
 * This is generic soft driver to interface devices connected via the 1-Wire
 * bus specified by Dallas Semiconductor Corp, today Maxim Integrated.
 *
 * @see         https://www.maximintegrated.com/en/design/technical-documents/app-notes/7/74.html
 * @see         https://pdfserv.maximintegrated.com/en/an/AN937.pdf
 * @see         https://www.ti.com/lit/an/spma057c/spma057c.pdf?ts=1598599201996
 *
 * @{
 * @file
 * @brief       1-Wire driver interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of 1-Wire ROM code in bytes
 */
#define ONEWIRE_ROM_LEN         (8U)

/**
 * @brief   String length of a hex encoded 1-Wire ROM code (including \0)
 */
#define ONEWIRE_ROM_STR_LEN     (17U)

/**
 * @brief   Custom value used to start a device search
 */
#define ONEWIRE_SEARCH_FIRST    (0)

/**
 * @brief   Return codes used by the 1-Wire driver
 */
enum {
    ONEWIRE_OK          =  0,   /**< everything fine */
    ONEWIRE_NODEV       = -1,   /**< no device connected to the bus */
    ONEWIRE_ERR_PINCFG  = -2,   /**< unable to initialize pin as open-drain */
    ONEWIRE_ERR_ROMSTR  = -3    /**< invalid ROM string given */
};

/**
 * @brief   1-Wire ROM commands
 */
enum {
    ONEWIRE_ROM_SEARCH  = 0xf0, /**< search for devices */
    ONEWIRE_ROM_READ    = 0x33, /**< read ROM code in single device config */
    ONEWIRE_ROM_MATCH   = 0x55, /**< address a specific slave */
    ONEWIRE_ROM_SKIP    = 0xcc, /**< broadcast commands to all connected devs */
    ONEWIRE_ROM_ALARM   = 0xec  /**< search for devices with active alarm */
};

/**
 * @brief   1-Wire configuration parameters
 */
typedef struct {
    gpio_t pin;                 /**< GPIO pin the bus is connected to */
    gpio_mode_t pin_mode;       /**< GPIO pin output mode */
} onewire_params_t;

/**
 * @brief   1-Wire bus device descriptor
 */
typedef struct {
    gpio_t pin;                 /**< GPIO pin */
    gpio_mode_t omode;          /**< GPIO pin output mode */
    gpio_mode_t imode;          /**< GPIO pin input mode, deducted from omode */
} onewire_t;

/**
 * @brief   1-Wire ROM code (device address) representation
 */
typedef struct {
    uint8_t u8[ONEWIRE_ROM_LEN];    /**< byte-wise access to address bytes */
} onewire_rom_t;


/**
 * @brief   Initialize a 1-Wire bus on the given GPIO pin
 *
 * This tries to configure the pin as open-drain output and will fail, if the
 * platform does not support this GPIO mode.
 *
 * @param[out] owi      1-Wire bus device descriptor
 * @param[in] params    configuration parameters
 *
 * @return  ONEWIRE_OK on success
 * @return  ONEWIRE_ERR_PINCFG if open-drain mode unsupported
 */
int onewire_init(onewire_t *owi, const onewire_params_t *params);

/**
 * @brief   Generate a bus reset sequence and look for connected devices
 *
 * Every bus transaction must be started with this reset sequence. During this
 * sequence it is detected, if there are any slaves connected to the bus.
 *
 * @param[in] owi       1-Wire bus device descriptor
 * @param[in] rom       1-Wire ROM code of target device
 *
 * @return  ONEWIRE_OK if slave(s) were found
 * @return  ONEWIRE_NODEV if no slave answered the reset sequence
 */
int onewire_reset(const onewire_t *owi, const onewire_rom_t *rom);

/**
 * @brief   Read data from the bus
 *
 * @param[in] owi       1-Wire bus device descriptor
 * @param[out] data     buffer to write received bytes into
 * @param[in] len       number of bytes to read from the bus
 */
void onewire_read(const onewire_t *owi, void *data, size_t len);

/**
 * @brief   Write data to the bus
 *
 * @param[in] owi       1-Wire bus device descriptor
 * @param[in] data      buffer holding the data that is written to the bus
 * @param[in] len       number of bytes to write to the bus
 */
void onewire_write(const onewire_t *owi, const void *data, size_t len);

/**
 * @brief   Convenience function for writing a single byte to the bus
 *
 * @param[in] owi       1-Wire bus device descriptor
 * @param[in] data      data byte to write to the bus
 */
static inline void onewire_write_byte(const onewire_t *owi, uint8_t data)
{
    onewire_write(owi, &data, 1);
}

/**
 * @brief   Calculate a 8-bit CRC using the 1-Wire specific polynomial
 *
 * @param[in] data      input data
 * @param[in] len       len of @p data in bytes
 *
 * @return  calculate 8-bit CRC
 */
uint8_t onewire_crc8(const uint8_t *data, size_t len);

/**
 * @brief   Search for devices on the given 1-Wire bus
 *
 * @see     https://pdfserv.maximintegrated.com/en/an/AN937.pdf page 51+52
 *
 * Use this function to discover devices connected to the given 1-Wire bus. To
 * carry out a full device discovery, this function has to be called multiple
 * times. For each iteration, the return value of the previous call as well as
 * the previous discovered ROM code have to be given as @p rom and @p ld
 * parameters to the subsequent call. One the function returns 0, the last
 * device has been found and there are no further devices to be discovered.
 *
 * @param[in] owi       1-Wire bus
 * @param[in,out] rom   next discovery ROM code, needs to hold previous value
 *                      when function is called multiple times
 * @param[in] ld        position of last discrepancy or ONEWIRE_SEARCH_FIRST for
 *                      initial call
 *
 * @return  bit position of last discrepancy
 * @return  0 if discovered device was the last
 * @return ONEWIRE_NODEV of no device was found
 */
int onewire_search(const onewire_t *owi, onewire_rom_t *rom, int ld);

/**
 * @brief   Read 1-Wire ROM code from string
 *
 * @param[out] rom      ROM code is parsed into this location
 * @param[in] str       String representation of a ROM code
 *
 * @return  ONEWIRE_OK on success
 * @return  ONEWIRE_ERR_ROMSTR on parsing error
 */
int onewire_rom_from_str(onewire_rom_t *rom, const char *str);

/**
 * @brief   Write a 1-Wire ROM code to a string
 *
 * @param[out] str      Output string, must be able to hold ONEWIRE_ROM_STR_LEN
 *                      characters
 * @param[in] rom       1-Wire ROM code to read
 */
void onewire_rom_to_str(char *str, const onewire_rom_t *rom);

/**
 * @brief   Print a 1-Wire ROM code to STDIO
 *
 * @param[in] rom       1-Wire ROM code
 */
void onewire_rom_print(const onewire_rom_t *rom);

#ifdef __cplusplus
}
#endif

#endif /* ONEWIRE_H */
/** @} */
