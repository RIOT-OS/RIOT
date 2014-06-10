/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the LGPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level SPI peripheral driver
 * @{
 *
 * @file        spi.h
 * @brief       Low-level SPI peripheral driver interface definitions
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef __SPI_H
#define __SPI_H

extern unsigned char spi_busy;

void spi_init(void);

/**
 * @brief Write one character to SPI
 *
 * @param[in] data character to write on SPI
 */
void spi_write(char data);

/**
 * @brief Write one character to SPI - will not wait for end
 *        useful for multiple writes with wait after final
 *
 * @param[in] data character to write on SPI
 */
void spi_write_fast(char data);

/**
 * @brief Read one character from SPI
 *
 * @return the read character
 */
char spi_read(void);

/**
 * @brief Flush the SPI read register
 */
void spi_flush(void);

#endif /* __SPI_H */
/** @} */
