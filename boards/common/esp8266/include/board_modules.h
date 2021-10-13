/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BOARD_MODULES_H
#define BOARD_MODULES_H

/**
 * @ingroup     boards_common_esp8266
 * @brief       Definitions for optional off-board hardware modules that can
 *              be used with all ESP8266 boards.
 *
 * All ESP8266 boards can be used with different off-board hardware modules.
 * This file contains the default configurations for those external hardware
 * modules that have been tested with the ESP8266 and are preconfigured here.
 * Most of these configurations can be overridden by an
 * \ref esp8266_app_spec_conf "application-specific configuration".
 *
 * The configurations of the respective hardware modules only take place if
 * the corresponding driver modules are used.
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_ENC28J60) || defined(DOXYGEN)
/**
 * @name    ENC28J60 configuration
 *
 * Configuration for the ENC28J60 interface when module ```enc28j60``` is used.
 *
 * SPI_DEV(0) is always used for the the module. The only configurations that
 * can be overridden by
 * \ref esp8266_app_spec_conf "Application Specific Board Configuration"
 * are the CS, INT and RESET signals.
 *
 * @note    The RESET signal can also be connected to the ESP8266 RST pin to
 *          keep an additional GPIO free.
 *
 * @{
 */
#define ENC28J60_PARAM_SPI      SPI_DEV(0)  /**< SPI_DEV(0) is used (fixed) */

#ifndef ENC28J60_PARAM_CS
#define ENC28J60_PARAM_CS       GPIO4       /**< ENC28J60 CS signal (can be overridden) */
#endif
#ifndef ENC28J60_PARAM_INT
#define ENC28J60_PARAM_INT      GPIO9       /**< ENC28J60 INT signal (can be overridden) */
#endif
#ifndef ENC28J60_PARAM_RESET
#define ENC28J60_PARAM_RESET    GPIO10      /**< ENC28J60 RESET signal (can be overridden) */
#endif
/** @} */
#endif /* defined(MODULE_ENC28J60) || defined(DOXYGEN) */

#if defined(MODULE_MRF24J40) || defined(DOXYGEN)
/**
 * @name    MRF24J40 configuration
 *
 * Configuration for the MRF24J40 interface when module ```mrf24j40``` is used.
 *
 * SPI_DEV(0) is always used for the the module. The only configurations that
 * can be overridden by
 * \ref esp8266_app_spec_conf "Application Specific Board Configuration"
 * are the CS, INT and RESET signals.
 *
 * @note    The RESET signal can also be connected to the ESP8266 RST pin to
 *          keep an additional GPIO free.
 * @{
 */
#define MRF24J40_PARAM_SPI      SPI_DEV(0)      /**< SPI_DEV(0) is used (fixed) */

#ifndef MRF24J40_PARAM_SPI_CLK
#define MRF24J40_PARAM_SPI_CLK  SPI_CLK_1MHZ    /**< SPI bus speed used (can be overridden) */
#endif
#ifndef MRF24J40_PARAM_CS
#define MRF24J40_PARAM_CS       GPIO16          /**< MRF24J40 CS signal (can be overridden) */
#endif
#ifndef MRF24J40_PARAM_INT
#define MRF24J40_PARAM_INT      GPIO0           /**< MRF24J40 INT signal (can be overridden) */
#endif
#ifndef MRF24J40_PARAM_RESET
#define MRF24J40_PARAM_RESET    GPIO2           /**< MRF24J40 RESET signal (can be overridden) */
#endif
/** @} */
#endif /* defined(MODULE_MRF24J40) || defined(DOXYGEN) */

#if defined(MODULE_SDCARD_SPI) || defined(DOXYGEN)
/**
 * @name    SD-Card configuration
 *
 * Configuration of the SD-Card interface when module ```sdcard_spi``` is used.
 *
 * The SPI interface with the corresponding pins used for the SD-Card
 * interface is fixed. SPI_DEV(0) is always used for the SD-Card. The only
 * configuration that can be overridden by \ref esp8266_app_spec_conf
 * "Application Specific Board Configuration" is the CS signal.
 * If not defined, the default CS signal of SPI_DEV(0) is used.
 * @{
 */
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(0)  /**< SPI_DEV(0) is used (fixed) */
#define SDCARD_SPI_PARAM_CLK    SPI0_SCK    /**< SPI_DEV(0) SCK  is used (fixed) */
#define SDCARD_SPI_PARAM_MOSI   SPI0_MOSI   /**< SPI_DEV(0) MOSI is used (fixed) */
#define SDCARD_SPI_PARAM_MISO   SPI0_MISO   /**< SPI_DEV(0) MISO is used (fixed) */
#define SDCARD_SPI_PARAM_POWER  GPIO_UNDEF  /**< power control is not used (fixed) */

#ifndef SDCARD_SPI_PARAM_CS
#define SDCARD_SPI_PARAM_CS     SPI0_CS0    /**< SD-Card CS signal (can be overridden) */
#endif
/** @} */
#endif /* defined(MODULE_SDCARD_SPI) || defined(DOXYGEN) */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */

#endif /* BOARD_MODULES_H */
