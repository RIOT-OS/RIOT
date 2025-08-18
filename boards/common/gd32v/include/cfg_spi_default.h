/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht <gunar@schorcht.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_gd32v
 * @{
 *
 * @file
 * @brief       Common peripheral configuration for GD32VF103 boards
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   SPI configuration
 * @{
 */

/**
 * @brief   Disable the second SPI device `SPI_DEV(1)` by default
 *
 * The second SPI device `SPI_DEV(1)` is only defined if `SPI_DEV_1_USED`
 * is set to 1 by the board.
 * This allows to use the default configuration with one or two SPI devices
 * depending on whether other peripherals are enabled that would collide with
 * the SPI devices.
 */
#ifndef SPI_DEV_1_USED
#define SPI_DEV_1_USED  0
#endif

/**
 * @brief   Defines PB12 as the default CS signal for `SPI_DEV(0)`
 *
 * Overriding this default allows the use of another pin as CS signal if
 * the default CS signal is connected to an unused hardware.
 */
#ifndef SPI_DEV_0_CS
#define SPI_DEV_0_CS    GPIO_PIN(PORT_B, 12)
#endif

/**
 * @brief   Defines PA4 as the default CS signal for `SPI_DEV(1)`
 *
 * Overriding this default allows the use of another pin as CS signal if
 * the default CS signal is connected to an unused hardware.
 */
#ifndef SPI_DEV_1_CS
#define SPI_DEV_1_CS    GPIO_PIN(PORT_B, 5)
#endif

/**
 * @brief   Default SPI device configuration
 *
 * The default SPI device configuration allows to define up to two SPI devices
 * `SPI_DEV(0)` and `SPI_DEV(1)`. `SPI_DEV(0)` is always defined if the SPI
 * peripheral is enabled by the module `periph_spi`. The second SPI device
 * `SPI_DEV(1)` is only defined if `SPI_DEV_1_USED` is set to 1 by the board.
 * This allows to use the default configuration with one or two SPI devices
 * depending on whether other peripherals are enabled that would collide with
 * the SPI devices.
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = SPI_DEV_0_CS,
        .rcumask  = RCU_APB1EN_SPI1EN_Msk,
        .apbbus   = APB1,
    },
#if SPI_DEV_1_USED
    {
        .dev      = SPI0,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = SPI_DEV_1_CS,
        .rcumask  = RCU_APB2EN_SPI0EN_Msk,
        .apbbus   = APB2,
    },
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
