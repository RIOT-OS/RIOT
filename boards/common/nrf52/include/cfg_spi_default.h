/*
 * SPDX-FileCopyrightText: 2016-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Default SPI config for nRF52 based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 15),
        .mosi = GPIO_PIN(0, 13),
        .miso = GPIO_PIN(0, 14),
#ifdef ERRATA_SPI_SINGLE_BYTE_WORKAROUND
        .ppi = 0,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
