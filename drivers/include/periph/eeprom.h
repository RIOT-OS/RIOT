/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_eeprom EEPROM driver
 * @ingroup     drivers_periph
 * @brief       Low-level EEPROM interface
 *
 * @{
 * @file
 * @brief       Low-level eeprom driver interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef PERIPH_EEPROM_H
#define PERIPH_EEPROM_H

#include <stdint.h>

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EEPROM_SIZE
#error "periph/eeprom: EEPROM_SIZE is not defined"
#endif

/**
 * @brief   Default value of the EEPROM clear byte
 */
#ifndef EEPROM_CLEAR_BYTE
#define EEPROM_CLEAR_BYTE 0x00
#endif

/**
 * @brief   Read a byte at the given position in eeprom
 *
 * @param[in]  pos      position to read
 *
 * @return the byte read
 */
uint8_t eeprom_read_byte(uint32_t pos);

/**
 * @brief   Read @p len bytes from the given position
 *
 * This function must be implemented by each CPU that provides an internal
 * EEPROM.
 *
 * @param[in]  pos      start position in eeprom
 * @param[out] data     output memory location to write to
 * @param[in]  len      the number of bytes to read
 *
 * @return  the number of bytes read
 */
size_t eeprom_read(uint32_t pos, void *data, size_t len);

/**
 * @brief   Write a byte at the given position
 *
 * @param[in] pos       position to write
 * @param[in] data      byte address to write to
 */
void eeprom_write_byte(uint32_t pos, uint8_t data);

/**
 * @brief   Write @p len bytes at the given position
 *
 * This function must be implemented by each CPU that provides an internal
 * EEPROM.
 *
 * @param[in] pos       start position in eeprom
 * @param[in] data      input memory location to read into
 * @param[in] len       the number of bytes to read
 *
 * @return the number of bytes written
 */
size_t eeprom_write(uint32_t pos, const void *data, size_t len);

/**
 * @brief   Set @p len bytes from the given position @p pos with value @p val
 *
 * @param[in] pos       start position in eeprom
 * @param[in] val       the value to set
 * @param[in] len       the number of bytes to set
 *
 * @return the number of bytes set
 */
size_t eeprom_set(uint32_t pos, uint8_t val, size_t len);

/**
 * @brief   Clear @p len bytes from the given position @p pos
 *
 * Clearing a byte in EEPROM simply consists in setting it to 0
 *
 * @param[in] pos       start position in eeprom
 * @param[in] len       the number of bytes to clear
 *
 * @return the number of bytes cleared
 */
size_t eeprom_clear(uint32_t pos, size_t len);

/**
 * @brief   Erase the whole EEPROM content
 *
 * @return the EEPROM_SIZE
 */
size_t eeprom_erase(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_EEPROM_H */
