/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           SPI CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#include <stdint.h>

#include "cpu.h"
#include "macros/units.h"
#include "periph/cpu_dma.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
/* resolve circular dependency by declaring spi_t here */
#define HAVE_SPI_T
typedef uint_fast8_t spi_t;
#endif

/**
 * @brief   Define a magic number that tells us to use hardware chip select
 *
 * We use a random value here, that does clearly differentiate from any possible
 * GPIO_PIN(x) value.
 */
#define SPI_HWCS_MASK       (0xffffff00)

/**
 * @brief   Override the default SPI hardware chip select access macro
 *
 * Since the CPU does only support one single hardware chip select line, we can
 * detect the usage of non-valid lines by comparing to SPI_HWCS_VALID.
 */
#define SPI_HWCS(x)         (SPI_HWCS_MASK | x)

/**
 * @brief   Define value for unused CS line
 */
#define SPI_CS_UNDEF        (GPIO_UNDEF)

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default spi_cs_t type definition
 * @{
 */
#define HAVE_SPI_CS_T
typedef uint32_t spi_cs_t;
/** @} */
#endif

/**
 * @brief   Use the shared SPI functions
 * @{
 */
/** Use transfer byte function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
/** Use transfer reg function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
/** Use transfer regs function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/* unify names across STM32 families */
#ifdef SPI_CR1_CPHA_Msk
#  define STM32_SPI_CPHA_Msk            SPI_CR1_CPHA_Msk
#endif
#ifdef SPI_CFG2_CPHA_Msk
#  define STM32_SPI_CPHA_Msk            SPI_CFG2_CPHA_Msk
#endif
#ifdef SPI_CR1_CPOL_Msk
#  define STM32_SPI_CPOL_Msk            SPI_CR1_CPOL_Msk
#endif
#ifdef SPI_CFG2_CPOL_Msk
#  define STM32_SPI_CPOL_Msk            SPI_CFG2_CPOL_Msk
#endif

/**
 * @name   Override the SPI mode values
 *
 * As the mode is set in bit 3 and 2 of the configuration register, we put the
 * correct configuration there
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                         /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = STM32_SPI_CPHA_Msk,                        /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = STM32_SPI_CPOL_Msk,                        /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = STM32_SPI_CPOL_Msk | STM32_SPI_CPHA_Msk,   /**< CPOL=1, CPHA=0 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
enum {
    SPI_CLK_100KHZ = KHZ(100), /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = KHZ(400), /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = MHZ(1),   /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = MHZ(5),   /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = MHZ(10),  /**< drive the SPI bus with 10MHz */
};

/**
 * @brief   SPI clock type
 */
typedef uint32_t spi_clk_t;
/** @} */

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    SPI_TypeDef *dev;       /**< SPI device base register address */
    gpio_t mosi_pin;        /**< MOSI pin */
    gpio_t miso_pin;        /**< MISO pin */
    gpio_t sclk_pin;        /**< SCLK pin */
    spi_cs_t cs_pin;        /**< HWCS pin, set to SPI_CS_UNDEF if not mapped */
#ifndef CPU_FAM_STM32F1
    gpio_af_t mosi_af;      /**< MOSI pin alternate function */
    gpio_af_t miso_af;      /**< MISO pin alternate function */
    gpio_af_t sclk_af;      /**< SCLK pin alternate function */
    gpio_af_t cs_af;        /**< HWCS pin alternate function */
#endif
    uint32_t rccmask;       /**< bit in the RCC peripheral enable register */
    uint8_t apbbus;         /**< APBx bus the device is connected to */
#ifdef MODULE_PERIPH_DMA
    dma_t tx_dma;           /**< Logical DMA stream used for TX */
    uint8_t tx_dma_chan;    /**< DMA channel used for TX */
    dma_t rx_dma;           /**< Logical DMA stream used for RX */
    uint8_t rx_dma_chan;    /**< DMA channel used for RX */
#endif
} spi_conf_t;

#ifndef DOXYGEN
gpio_t spi_pin_miso(spi_t bus);
gpio_t spi_pin_mosi(spi_t bus);
gpio_t spi_pin_clk(spi_t bus);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
