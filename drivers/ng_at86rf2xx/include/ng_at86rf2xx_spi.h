/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
 * @{
 *
 * @file
 * @brief       Register access function definitions for AT86RF2xx device driver
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef NG_AT86RF2XX_SPI_H_
#define NG_AT86RF2XX_SPI_H_

#include <stdint.h>

#include "board.h"
#include "ng_at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read from a register at address `addr` from device `dev`.
 *
 * @param[in] dev   Device descriptor of the radio device to read from
 * @param[in] addr  Address of the radio device's register
 *
 * @return the value of the specified register
 */
uint8_t ng_at86rf2xx_reg_read(const ng_at86rf2xx_t *dev, const uint8_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`.
 *
 * @param[in] dev       Device descriptor of the radio device to write to
 * @param[in] addr      Address of the radio device's register
 * @param[in] value     The value to write to the device's register
 */
void ng_at86rf2xx_reg_write(const ng_at86rf2xx_t *dev, const uint8_t addr, const uint8_t value);

/**
 * @brief Read first `length` number of bytes into `data` from device `dev`'s packet FIFO.
 * @details Sequencial calls to this function always return the same FIFO data.
 *          The FIFO only gets updated after one full packet read.
 *
 * @param[in]   dev     Device descriptor of the radio device to read from
 * @param[out]  data    Buffer to copy FIFO data into
 * @param[in]   length  Number of bytes to read from FIFO
 */
void ng_at86rf2xx_bulk_read_fifo(const ng_at86rf2xx_t *dev,
                                 uint8_t *data,
                                 const radio_packet_length_t length);

/**
 * @brief Write `length` number of bytes from `data` to device `dev`'s packet FIFO.
 *
 * @param[in] dev       Device descriptor of the radio device to write to
 * @param[in] data      Data to copy into FIFO
 * @param[in] length    Number of bytes to write to FIFO
 */
void ng_at86rf2xx_bulk_write_fifo(const ng_at86rf2xx_t *dev,
                                  const uint8_t *data,
                                  const radio_packet_length_t length);

/**
 * @brief Read `length` number of bytes into `data` from device `dev`'s packet FIFO
 *        starting from `addr`.
 * @details This function provides access to dedicated bytes within the FIFO.
 *
 * @param[in]   dev     Device descriptor of the radio device to read from
 * @param[in]   addr    Offset to start reading from. Valid values [0x00 - 0x7f]
 * @param[out]  data    Buffer to copy FIFO data into
 * @param[in]   length  Number of bytes to read from FIFO
 */
void ng_at86rf2xx_addr_read_fifo(const ng_at86rf2xx_t *dev,
                                 const uint8_t addr,
                                 uint8_t *data,
                                 const radio_packet_length_t length);

/**
 * @brief Write `length` number of bytes from `data` to device `dev`'s packet FIFO
 *        starting from `addr` in the FIFO.
 * @details This function provides access to dedicated bytes within the FIFO.
 *
 * @param[in]   dev     Device descriptor of the radio device to write to
 * @param[in]   addr    Offset to start writing to. Valid values [0x00 - 0x7f]
 * @param[out]  data    Data to copy into FIFO
 * @param[in]   length  Number of bytes to write to FIFO
 */
void ng_at86rf2xx_addr_write_fifo(const ng_at86rf2xx_t *dev,
                                  const uint8_t addr,
                                  const uint8_t *data,
                                  const radio_packet_length_t length);

/**
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */
uint8_t ng_at86rf2xx_get_status(const ng_at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NG_AT86RF2XX_SPI_H_ */
/** @} */
