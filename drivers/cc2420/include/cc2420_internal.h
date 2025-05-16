/*
 * Copyright (C) 2014 Milan Babel <babel@inf.fu-berlin.de> and INRIA
 *               2015-2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
  * @ingroup    drivers_cc2420
  * @{
  *
  * @file
  * @brief      Definitions and settings for the cc2420
  *
  * @author     Milan Babel <babel@inf.fu-berlin.de>
  * @author     Kévin Roussel <Kevin.Roussel@inria.fr>
  * @author     Thomas Eichinger <thomas.eichinger@fu-berlin.de>
  * @author     Hauke Petersen <hauke.petersen@fu-berlin.de>
  * @author     Francisco Acosta <francisco.acosta@inria.fr>
  *
  */

#include <stdint.h>

#include "cc2420.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Delays for resetting and turning on the device
 */
#define CC2420_RESET_DELAY      (500U)
#define CC2420_XOSCON_DELAY     (2000U)

uint8_t cc2420_strobe(const cc2420_t *dev, const uint8_t command);

/**
 * @brief   Read from a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to read from
 * @param[in] addr      address of the register to read
 *
 * @return              the value of the specified register
 */
uint16_t cc2420_reg_read(const cc2420_t *dev, const uint8_t addr);

/**
 * @brief   Write to a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to write to
 * @param[in] addr      address of the register to write
 * @param[in] value     value to write to the given register
 */
void cc2420_reg_write(const cc2420_t *dev, const uint8_t addr,
                      const uint16_t value);

/**
 * @brief   Read a chunk of data from the SRAM of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  addr     starting address to read from [valid 0x00-0x16B]
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from SRAM
 */
void cc2420_ram_read(const cc2420_t *dev, const uint16_t addr,
                     uint8_t *data, const size_t len);

/**
 * @brief   Write a chunk of data into the SRAM of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] addr      address in the SRAM to write to [valid 0x00-0x16B]
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void cc2420_ram_write(const cc2420_t *dev, const uint16_t addr,
                      const uint8_t *data, const size_t len);

/**
 * @brief   Reads FIFO buffer from RAM at address 0x080
 *
 * @param[in] dev       device to write to
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void cc2420_fifo_read(const cc2420_t *dev, uint8_t *data, const size_t len);

/**
 * @brief   Writes FIFO buffer to RAM at address 0x000
 *
 * @param[in] dev       device to write to
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void cc2420_fifo_write(const cc2420_t *dev, uint8_t *data, const size_t len);

/**
 * @brief   Get the device's status byte
 */
uint8_t cc2420_status(cc2420_t *dev);

/**
 * @brief   Get the device's current state
 */
uint8_t cc2420_state(cc2420_t *dev);

/**
 * @brief   Enable on-board oscillator
 */
void cc2420_en_xosc(cc2420_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
