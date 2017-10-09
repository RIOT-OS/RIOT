/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_winc1500
 *
 * @{
 * @file
 * @brief       Default configuration for the WINC1500 Wi-Fi driver
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 */

#ifndef WINC1500_PARAMS_H
#define WINC1500_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   WINC1500 SPI mode and clock configurations
 * @{
 */
#ifndef WINC1500_SPI
#define WINC1500_SPI            (SPI_DEV(1))
#endif
#ifndef WINC1500_INTN_PIN
#define WINC1500_INTN_PIN       (GPIO_PIN(PB, 9))
#endif
#ifndef WINC1500_SPI_CS_PIN
#define WINC1500_SPI_CS_PIN     (GPIO_PIN(PA, 14))
#endif

#ifndef WINC1500_RESET_PIN
#define WINC1500_RESET_PIN      (GPIO_PIN(PA, 27))
#endif
#ifndef WINC1500_CHIP_EN_PIN
#define WINC1500_CHIP_EN_PIN    (GPIO_PIN(PA, 28))
#endif
#ifndef WINC1500_WAKE_PIN
#define WINC1500_WAKE_PIN       (GPIO_PIN(PB, 8))
#endif

#ifndef WINC1500_SPI_MODE
#define WINC1500_SPI_MODE   SPI_MODE_0
#endif
#ifndef WINC1500_SPI_CLOCK
#define WINC1500_SPI_CLOCK  SPI_CLK_10MHZ
#endif
/**@}*/

/**
 * @brief   Set default configuration parameters for the WINC1500 driver
 */
#define WINC1500_PARAMS_DEFAULT    {.spi = WINC1500_SPI, \
                                     .cs_pin = WINC1500_SPI_CS_PIN, \
                                     .int_pin = WINC1500_INTN_PIN, \
                                     .reset_pin = WINC1500_RESET_PIN, \
                                     .en_pin = WINC1500_CHIP_EN_PIN, \
                                     .wake_pin = WINC1500_WAKE_PIN, \
                                     .spi_clk = WINC1500_SPI_CLOCK}

/**
 * @brief   WINC1500 SPI configuration
 */
static const winc1500_params_t winc1500_params[] =
{
#ifdef WINC1500_PARAMS_BOARD
    WINC1500_PARAMS_BOARD,
#else
    WINC1500_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* WINC1500_PARAMS_H */
/** @} */
