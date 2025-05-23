/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32_mh-et-live-minikit
 * @brief       Definitions for Wemos stackable hardware modules (shields)
 *
 * The board can be used with lots of optional stackable hardware modules.
 * This file contains the default configurations for those hardware modules
 * that have been tested. Most of these configurations can be overridden by an
 * \ref esp32_application_specific_configurations "application-specific configuration".
 *
 * The configurations of the respective hardware modules only take place if
 * the corresponding driver modules are used.
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_MRF24J40 || DOXYGEN
/**
 * @name    MRF24J40 shield configuration
 *
 * Configuration for the MRF24J40 shield when module `mrf24j40` is used.
 *
 * MRF24J40 module uses SPI_DEV(0) and according pins on this board to be
 * compatible with the Wemos D1 mini MRF24J40 shield.
 *
 * @note    To keep an additional GPIO free, the ENC28J60 RESET signal can
 *          be connected to ESP32 Reset pin.
 * @{
 */
#define MRF24J40_PARAM_SPI      SPI_DEV(0)      /**< SPI_DEV(0) is used (fixed) */

#ifndef MRF24J40_PARAM_SPI_CLK
#define MRF24J40_PARAM_SPI_CLK  SPI_CLK_1MHZ    /**< SPI bus speed used (can be overridden) */
#endif
#ifndef MRF24J40_PARAM_CS
#define MRF24J40_PARAM_CS       GPIO26           /**< MRF24J40 CS signal (can be overridden) */
#endif
#ifndef MRF24J40_PARAM_INT
#define MRF24J40_PARAM_INT      GPIO17           /**< MRF24J40 INT signal (can be overridden) */
#endif
#ifndef MRF24J40_PARAM_RESET
#define MRF24J40_PARAM_RESET    GPIO16          /**< MRF24J40 RESET signal (can be overridden) */
#endif
/** @} */
#endif /* MODULE_MRF24J40 || DOXYGEN */

#if MODULE_SDCARD_SPI || DOXYGEN
/**
 * @name    SD-Card shield configuration
 *
 * Configuration of the SD-Card interface when module `sdcard_spi` is used.
 *
 * SD card interface uses SPI_DEV(0) on this board to be compatible with the
 * Wemos D1 mini micro SD card shield. The D8 pin (GPIO5) is used as default
 * CS signal.
 *
 * @note Please override the definition of CS pin by an by \ref
 * esp32_application_specific_configurations "application-specific configurations" according to your
 * solder bride configuration.
 * @{
 */
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(0)   /**< SPI_DEV(0) is used (fixed) */
#define SDCARD_SPI_PARAM_CLK    SPI0_SCK     /**< SPI_DEV(0) SCK  is used (fixed) */
#define SDCARD_SPI_PARAM_MOSI   SPI0_MOSI    /**< SPI_DEV(0) MOSI is used (fixed) */
#define SDCARD_SPI_PARAM_MISO   SPI0_MISO    /**< SPI_DEV(0) MISO is used (fixed) */
#define SDCARD_SPI_PARAM_POWER  GPIO_UNDEF   /**< power control is not used (fixed) */

#ifndef SDCARD_SPI_PARAM_CS
#define SDCARD_SPI_PARAM_CS     SPI0_CS0     /**< SD-Card CS signal (override it) */
#endif
/** @} */
#endif /* MODULE_SDCARD_SPI || DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
