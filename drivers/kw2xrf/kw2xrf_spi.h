/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file
 * @brief       Definition of KW2XRF SPI functions
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef KW2XRF_SPI_H
#define KW2XRF_SPI_H

#include <stdio.h>
#include "board.h"
#include "cpu.h"
#include "periph/spi.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI interface initialization
 * @param[in] dev device descriptor
 *
 * @return 0 on success
 * @return -1 on error
 */
int kw2xrf_spi_init(kw2xrf_t *dev);

/**
 * @brief Writes a byte to the kw2xrf register.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to write.
 * @param[in] value The value to write in the register.
 */
void kw2xrf_write_dreg(kw2xrf_t *dev, uint8_t addr, uint8_t value);

/**
 * @brief Reads a byte from the kw2xrf register.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to read.
 * @return Value of the register.
 */
uint8_t kw2xrf_read_dreg(kw2xrf_t *dev, uint8_t addr);

/**
 * @brief Writes to kw2xrf direct registers.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to write into.
 * @param[in] buf Value that shall be written.
 * @param[in] length Length of the register.
 */
void kw2xrf_write_dregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length);

/**
 * @brief Reads a byte from the kw2xrf indirect register.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to read.
 * @param[in] buf Buffer, where the content of the reg shall be written to.
 * @param[in] length Length of the register.
 */
void kw2xrf_read_dregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length);

/**
 * @brief Writes to  a byte from the kw2xrf indirect register.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to write into.
 * @param[in] value Value that shall be written.
 */
void kw2xrf_write_ireg(kw2xrf_t *dev, uint8_t addr, uint8_t value);

/**
 * @brief Reads a byte from the kw2xrf indirect register.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to read.
 *
 * @return value in the register
 */
uint8_t kw2xrf_read_ireg(kw2xrf_t *dev, uint8_t addr);

/**
 * @brief Writes to kw2xrf indirect registers.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to write into.
 * @param[in] buf Value that shall be written.
 * @param[in] length Length of the register.
 */
void kw2xrf_write_iregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length);

/**
 * @brief Reads a byte from the kw2xrf indirect register.
 *
 * @param[in] dev device descriptor
 * @param[in] addr Address of the register to read.
 * @param[in] buf Buffer, where the content of the reg shall be written to.
 * @param[in] length Length of the register.
 */
void kw2xrf_read_iregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length);

/**
 * @brief Writes multiple bytes to the kw2xrf fifo.
 *
 * @param[in] dev device descriptor
 * @param[in] data A buffer with the value to write to the fifo.
 * @param[in] data_length The count of bytes which should be written.
 *
 * @return number of bytes written.
 */
void kw2xrf_write_fifo(kw2xrf_t *dev, uint8_t *data, uint8_t data_length);

/**
 * @brief Reads multiple bytes from the kw2xrf fifo.
 *
 * @param[in] dev device descriptor
 * @param[out] data A buffer to store the value of the fifo.
 * @param[in] data_length The count of bytes which should be read.
 *
 * @return number of bytes read.
 */
void kw2xrf_read_fifo(kw2xrf_t *dev, uint8_t *data, uint8_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* KW2XRF_SPI_H */
/** @} */
