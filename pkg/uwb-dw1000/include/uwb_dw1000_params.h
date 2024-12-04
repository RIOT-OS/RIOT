/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef UWB_DW1000_PARAMS_H
#define UWB_DW1000_PARAMS_H

#include "board.h"
#include "uwb_dw1000.h"
#include "dpl/dpl_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef DW1000_SPI_SEM
static struct dpl_sem sem_spi;
#define DW1000_SPI_SEM              &sem_spi
#endif
#ifndef DW1000_PARAM_SPI
#define DW1000_PARAM_SPI            (SPI_DEV(1))
#endif
#ifndef DW1000_PARAM_SPI_CLK_LOW
#define DW1000_PARAM_SPI_CLK_LOW    (SPI_CLK_1MHZ)
#endif
#ifndef DW1000_PARAM_SPI_CLK_HIGH
#define DW1000_PARAM_SPI_CLK_HIGH   (SPI_CLK_10MHZ)
#endif
#ifndef DW1000_SPI_MODE
#define DW1000_SPI_MODE             (SPI_MODE_0)
#endif
#ifndef DW1000_PARAM_CS_PIN
#define DW1000_PARAM_CS_PIN         (GPIO_PIN(0, 17))
#endif
#ifndef DW1000_PARAM_IRQ_PIN
#define DW1000_PARAM_IRQ_PIN        (GPIO_PIN(0, 19))
#endif
#ifndef DW1000_PARAM_RESET_PIN
#define DW1000_PARAM_RESET_PIN      (GPIO_PIN(0, 24))
#endif
#ifndef DW1000_RX_ANTENNA_DELAY
#define DW1000_RX_ANTENNA_DELAY     (0x4042)
#endif
#ifndef DW1000_TX_ANTENNA_DELAY
#define DW1000_TX_ANTENNA_DELAY     (0x4042)
#endif
#ifndef DW1000_EXT_CLOCK_DELAY
#define DW1000_EXT_CLOCK_DELAY      (0)
#endif

#ifndef DW1000_PARAMS
#define DW1000_PARAMS           { .spi_sem = DW1000_SPI_SEM, \
                                  .spi_baudrate = DW1000_PARAM_SPI_CLK_HIGH, \
                                  .spi_baudrate_low = DW1000_PARAM_SPI_CLK_LOW, \
                                  .spi_num = DW1000_PARAM_SPI, \
                                  .rst_pin = DW1000_PARAM_RESET_PIN, \
                                  .irq_pin = DW1000_PARAM_IRQ_PIN, \
                                  .ss_pin = DW1000_PARAM_CS_PIN, \
                                  .rx_antenna_delay = DW1000_RX_ANTENNA_DELAY, \
                                  .tx_antenna_delay = DW1000_TX_ANTENNA_DELAY, \
                                  .ext_clock_delay = DW1000_EXT_CLOCK_DELAY }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const dw1000_params_t dw1000_params[] =
{
    DW1000_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* UWB_DW1000_PARAMS_H */
