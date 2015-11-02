/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_xmc1000
 * @{
 *
 * @file
 * @brief           Code shared by peripheral drivers using the USIC
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef USIC_DEFS_H_
#define USIC_DEFS_H_

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cpu_xmc1000_usic USIC configuration
 * @ingroup cpu_xmc1000
 * @{
 */

#define USIC_CTQIN_PDIV  (0) /**< f<sub>CTQIN</sub> is provided by f<sub>PDIV</sub> */
#define USIC_CTQIN_PPP   (1) /**< f<sub>CTQIN</sub> is provided by f<sub>PPP</sub>  */
#define USIC_CTQIN_SCLK  (2) /**< f<sub>CTQIN</sub> is provided by f<sub>SCLK</sub> */
#define USIC_CTQIN_MCLK  (3) /**< f<sub>CTQIN</sub> is provided by f<sub>MCLK</sub> */

#define USIC_MODE_ASC    (0) /**< select ASC/UART mode */
#define USIC_MODE_SSC    (1) /**< select SSC/SPI mode  */

/**
 * @brief Struct to hold the configuration values necessary to
 *        initialize a USIC channel 
 * @{
 */
typedef struct {
    uint32_t ccr;      /**< channel control register           */
    uint32_t sctr;     /**< shift control register             */
    uint32_t tcsr;     /**< transmit control/status register   */
    uint32_t pcr;      /**< protocol control register          */
    uint32_t inpr;     /**< interrupt node pointer register    */
    uint8_t dx2_dsel;  /**< data selection for input stage DX2 */
} usic_controls_t;
/** @} */

/**
 * @brief   USIC BRG (baud rate generator) register configuration
 * @{
 */
typedef struct {
    union {
        struct __attribute__((packed)) {
            unsigned clksel : 2;
            unsigned __reserved1 : 1;
            unsigned tmen : 1;
            unsigned pppen : 1;
            unsigned __reserved2 : 1;
            unsigned ctqsel : 2;
            unsigned pctq : 2;
            unsigned dctq : 5;
            unsigned __reserved3 : 1;
            unsigned pdiv : 10;
            unsigned __reserved4 : 2;
            unsigned sclkosel : 1;
            unsigned mclkcfg : 1;
            unsigned sclkcfg : 2;
        } field;
        uint32_t reg;
    };
} usic_brg_t;
/** @} */

/**
 * @brief   USIC FDR (fractional divider) register configuration
 * @{
 */
typedef struct {
    union {
        struct __attribute__((packed)) {
            unsigned step : 10;
            unsigned __reserved1 : 4;
            unsigned dm : 2;
            unsigned result : 10;
            unsigned __reserved2 : 6;
        } field;
        uint32_t reg;
    };
} usic_fdr_t;
/** @} */

/**
 * @brief   UART device configuration
 * @{
 */
typedef struct {
    USIC_CH_TypeDef *usic;          /**< pointer to USIC channel                             */
    const usic_controls_t *mode;    /**< pointer to the control mode for this USIC           */
    const gpio_alt_t tx_pin;        /**< the primary data output pin (MOSI/TX)               */
    const gpio_alt_t rx_pin;        /**< the primary data input pin (MISO/RX)                */
} uart_instance_t;
/** @} */

/**
 * @brief   SPI device configuration
 * @{
 */
typedef struct {
    USIC_CH_TypeDef *usic;          /**< pointer to USIC channel                             */
    const usic_controls_t *mode;    /**< pointer to the control mode for this USIC           */
    const gpio_alt_t mosi_pin;      /**< MOSI pin                                            */
    const gpio_alt_t miso_pin;      /**< MISO pin                                            */
    const gpio_alt_t sclk_pin;      /**< master shift clock pin (SCLK)                       */
    const gpio_alt_t msls_pin;      /**< master-slave-select pin (slave select)              */
} spi_instance_t;
/** @} */

/**
 * @brief   USIC channel configuration
 * @{
 */
typedef struct {
    USIC_CH_TypeDef *usic;          /**< pointer to USIC channel                             */
    const usic_controls_t *mode;    /**< pointer to the control mode for this USIC           */
    const gpio_alt_t tx_pin;        /**< the primary data output pin (MOSI/TX)               */
    const gpio_alt_t rx_pin;        /**< the primary data input pin (MISO/RX)                */
    const gpio_alt_t sclk_pin;      /**< supplementary pin for master/slave SSC/SPI (clock)  */
    const gpio_alt_t msls_pin;      /**< supplementary pin for master SSC/SPI (slave select) */
} usic_channel_t;
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* USIC_DEFS_H_ */
