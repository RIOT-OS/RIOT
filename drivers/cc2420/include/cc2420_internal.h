/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Internal interfaces for CC2420 drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef CC2420_INTERNAL_H_
#define CC2420_INTERNAL_H_

#include <stdint.h>

#include "cc2420.h"


#ifdef __cplusplus
extern "C" {
#endif

/* startup timeout (2 ms) in 16MHz-clock cycles */
#define CC2420_STARTUP_TIMEOUT  32000U
/* Various configuration settings for the CC2420 drivers  */
#define CC2420_SYNC_WORD_TX_TIME 900000
#define CC2420_RX_BUF_SIZE      3
#define CC2420_WAIT_TIME        500

/* Shortcut macro */
#define CC2420_STATUS_BYTE() cc2420_strobe(dev, NOBYTE)

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
 * @param[in]  offset   starting address to read from
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from SRAM
 */
void cc2420_ram_read(const cc2420_t *dev,
                     const uint16_t offset,
                     uint8_t *data,
                     const size_t len);

/**
 * @brief   Write a chunk of data into the SRAM of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the SRAM to write to [valid 0x00-0x7f]
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void cc2420_ram_write(const cc2420_t *dev,
                      const uint16_t offset,
                      const uint8_t *data,
                      const size_t len);

/**
 * @brief   Read the internal frame buffer of the given device
 *
 * Reading the frame buffer returns some extra bytes that are not accessible
 * through reading the RAM directly.
 *
 * @param[in]  dev      device to read from
 * @param[out] data     buffer to copy the data to
 * @param[in]  len      number of bytes to read from the frame buffer
 */
void cc2420_fb_read(const cc2420_t *dev,
                    uint8_t *data, const size_t len);

/**
 * @brief   Write the internal frame buffer of the given device
 *
 * Reading the frame buffer returns some extra bytes that are not accessible
 * through reading the RAM directly.
 *
 * @param[in]  dev      device to read from
 * @param[out] data     buffer to copy the data to
 * @param[in]  len      number of bytes to read from the frame buffer
 */
void cc2420_fb_write(const cc2420_t *dev,
                     uint8_t *data, const size_t len);

/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
// uint8_t cc2420_get_status(const cc2420_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NG_AT86RF2XX_INTERNAL_H_ */
/** @} */
