/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32f310c-eval
 * @{
 *
 * @file
 * @brief       Common peripheral configuration for GD32F310 boards
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef CFG_SPI_DEFAULT_H
#define CFG_SPI_DEFAULT_H

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
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_B, 6),
        // .mosi_af  = GPIO_AF_1,
        // .miso_af  = GPIO_AF_1,
        // .sclk_af  = GPIO_AF_1,
        // .cs_af    = GPIO_AF_1,
        .rcumask  = RCU_APB2EN_SPI0EN_Msk, //SPI0_CTL0_SPIEN_Msk
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = 0,
        .rx_dma   = 0,
        .rx_dma_chan = 0,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_SPI_DEFAULT_H */
/** @} */
