/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef NVRAM_SPI_H
#define NVRAM_SPI_H

/**
 * @ingroup     drivers_nvram
 * @{
 *
 * @file
 *
 * @brief       Device interface for various SPI connected NVRAM.
 *
 * Tested on:
 * - Cypress/Ramtron FM25L04B.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include "nvram.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Bus parameters for SPI NVRAM.
 */
typedef struct nvram_spi_params {
    /** @brief   RIOT SPI device */
    spi_t spi;
    /** @brief   SPI clock speed */
    spi_clk_t clk;
    /** @brief   Chip select pin */
    gpio_t cs;
    /** @brief   Number of address bytes following each read/write command. */
    uint8_t address_count;
} nvram_spi_params_t;

/**
 * @brief   Initialize an nvram_t structure with SPI settings.
 *
 * This will also initialize the CS pin as a GPIO output, without pull resistors.
 *
 * @param[out] dev          Pointer to NVRAM device descriptor
 * @param[out] spi_params   Pointer to SPI settings
 * @param[in]  size         Device capacity
 *
 * @return                  0 on success
 * @return                  <0 on errors
 */
int nvram_spi_init(nvram_t *dev, nvram_spi_params_t *spi_params, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NVRAM_SPI_H */
