/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp8266_sparkfun-thing
 * @brief       Board specific definitions for
 *              SparkFun ESP8266 Thing boards.
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    On-board LED configuration and control definitions
 * @{
 */
#define LED0_PIN    GPIO5   /**< GPIO5 is used as LED Pin */
#define LED0_ACTIVE (1)     /**< LED is high active */

/** @} */

/**
 * @name    SPI configuration
 * @{
 */
#if defined(MODULE_PERIPH_SPI) || defined(DOXYGEN)
/**
 * GPIO15 is not available on SparkFun Thing Dev. Therefore another GPIO is
 * define as default CS signal for HSPI interface SPI_DEV(0).
 */
#ifndef SPI0_CS0_GPIO
#define SPI0_CS0_GPIO    GPIO16  /**< HSPI / SPI_DEV(0) CS default pin, only used when cs
                                      parameter in spi_acquire is GPIO_UNDEF */
#endif
#endif /* defined(MODULE_PERIPH_SPI) || defined(DOXYGEN) */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

/** @} */
