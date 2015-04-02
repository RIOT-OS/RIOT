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
 * @file        kw2xrf_spi.h
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
 */
void kw2xrf_spi_init(void);

/**
 * @brief Writes a byte to the kw2xrf register.
 *
 * @param[in] addr Address of the register to write.
 * @param[in] value The value to write in the register.
 */
void kw2xrf_write_dreg(uint8_t addr, uint8_t value);

/**
 * @brief Reads a byte from the kw2xrf register.
 *
 * @param[in] addr Address of the register to read.
 * @return Value of the register.
 */
uint8_t kw2xrf_read_dreg(uint8_t addr);

/**
 * @brief Writes to  a byte from the kw2xrf indirect register.
 *
 * @param[in] addr Address of the register to write into.
 * @param[in] value Value that shall be written.
 */
void kw2xrf_write_ireg(uint8_t addr, uint8_t value);

/**
 * @brief Reads a byte from the kw2xrf indirect register.
 *
 * @param[in] addr Address of the register to read.
 *
 * @return value in the register
 */
uint8_t kw2xrf_read_ireg(uint8_t addr);

/**
 * @brief Writes to kw2xrf indirect registers.
 *
 * @param[in] addr Address of the register to write into.
 * @param[in] buf Value that shall be written.
 * @param[in] length Length of the register.
 */
void kw2xrf_write_iregs(uint8_t addr, uint8_t *buf, uint8_t length);

/**
 * @brief Reads a byte from the kw2xrf indirect register.
 *
 * @param[in] addr Address of the register to read.
 * @param[in] buf Buffer, where the content of the reg shall be written to.
 * @param[in] length Length of the register.
 */
void kw2xrf_read_iregs(uint8_t addr, uint8_t *buf, uint8_t length);

/**
 * @brief Writes multiple bytes to the kw2xrf fifo.
 *
 * @param[in] data A buffer with the value to write to the fifo.
 * @param[in] data_length The count of bytes which should be written.
 *
 * @return number of bytes written.
 */
radio_packet_length_t kw2xrf_write_fifo(uint8_t *data, radio_packet_length_t data_length);

/**
 * @brief Reads multiple bytes from the kw2xrf fifo.
 *
 * @param[out] data A buffer to store the value of the fifo.
 * @param[in] data_length The count of bytes which should be read.
 *
 * @return number of bytes read.
 */
radio_packet_length_t kw2xrf_read_fifo(uint8_t *data, radio_packet_length_t data_length);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
