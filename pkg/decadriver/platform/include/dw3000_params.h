/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_decadriver
 * @{
 *
 * @file
 * @brief       Default configuration for DW3000
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include "board.h"
/* Note: Since the dw3000.h is already used by decadriver, the name _conf.h is
 * used here for the struct definitions */
#include "dw3000_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 *
 * These default values correspond to the Qorvo DWM3000EVB Evaluation Shield
 * on top of an nrf52840dk
 * @{
 */
/** SPI device used */
#ifndef DW3000_PARAM_SPI
#  define DW3000_PARAM_SPI              SPI_DEV(0)
#endif

/** Chip Select pin for SPI */
#ifndef DW3000_PARAM_SPI_CS
#  define DW3000_PARAM_SPI_CS           GPIO_PIN(1, 12)
#endif

/** Mode that the SPI connection uses */
#ifndef DW3000_PARAM_SPI_MODE
#  define DW3000_PARAM_SPI_MODE         SPI_MODE_0
#endif

/** IRQ pin to receive interrupts from the DW3000 */
#ifndef DW3000_PARAM_IRQ
#  define DW3000_PARAM_IRQ              GPIO_PIN(1, 10)
#endif

/** Pin to reset the DW3000 */
#ifndef DW3000_PARAM_RESET
#  define DW3000_PARAM_RESET            GPIO_PIN(1, 8)
#endif

/**
 * @brief Pin to wakeup the DW3000. Optional, set to GPIO_UNDEF if unused.
 *
 * If undefined, the DW3000 will be woken using SPI CS.
 */
#ifndef DW3000_PARAM_WAKEUP
#  define DW3000_PARAM_WAKEUP           GPIO_PIN(1, 11)
#endif

/** The fast SPI speed. Can be activated after IDLE_RC */
#ifndef DW3000_PARAM_SPI_SPEED_FAST
#  define DW3000_PARAM_SPI_SPEED_FAST   SPI_CLK_10MHZ
#endif

/** The slow (and default) SPI speed */
#ifndef DW3000_PARAM_SPI_SPEED_SLOW
#  define DW3000_PARAM_SPI_SPEED_SLOW   SPI_CLK_1MHZ
#endif

/** Collection of all params. Only one device is allowed. */
#ifndef DW3000_PARAMS
#  define DW3000_PARAMS                 { .spi = DW3000_PARAM_SPI, \
                                          .spi_cs = DW3000_PARAM_SPI_CS, \
                                          .spi_mode = DW3000_PARAM_SPI_MODE, \
                                          .irq = DW3000_PARAM_IRQ, \
                                          .reset = DW3000_PARAM_RESET, \
                                          .wakeup = DW3000_PARAM_WAKEUP, \
                                          .spi_speed_fast = DW3000_PARAM_SPI_SPEED_FAST, \
                                          .spi_speed_slow = DW3000_PARAM_SPI_SPEED_SLOW }
#endif
/**@}*/

/**
 * @brief   Configuration struct.
 *
 * The patched decadriver by br101 only allows a single device, with the benefit
 * of reducing the code size and the reasoning that typical embedded devices
 * will only ever use one DW3000 radio. Thus only a single params struct is
 * allowed here.
 */
static const dw3000_params_t dw3000_params = DW3000_PARAMS;

#ifdef __cplusplus
}
#endif

/** @} */
