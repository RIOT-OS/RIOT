/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           CPU specific definitions for SDIO/SDMMC for the STM32 family
 *
 * @author          Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

#include "periph/cpu_dma.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the types for SDIO/SDMMC only if the STM32 has SDIO/SDMMC peripheral */
#if defined(SDMMC_POWER_PWRCTRL) || defined(SDIO_POWER_PWRCTRL)

/* For F1, F2, F4 and L1 the SDMMC interface is called SDIO, define used
 * symbols for source code compatibility */
#if !defined(SDMMC1) && !DOXYGEN
#define SDMMC_TypeDef   SDIO_TypeDef
#endif

/**
 * @brief SDIO/SDMMC buffers alignment because of STM32 DMA/FIFO restrictions
 */
#define SDMMC_CPU_DMA_ALIGNMENT     4

/**
 * @brief SDIO/SDMMC buffer instantiation requirement for STM32
 */
#define SDMMC_CPU_DMA_REQUIREMENTS  __attribute__((aligned(SDMMC_CPU_DMA_ALIGNMENT)))

/**
 * @brief SDIO/SDMMC pin structure for STM32
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t af;           /**< GPIO alternate function */
#endif
} sdmmc_pin_t;

/**
 * @brief SDIO/SDMCC peripheral configuration for STM32
 *
 * To use 1-bit bus width, define `dat1` to `dat3` as `GPIO_UNDEF`. 8-bit bus
 * width with `dat4` to `dat7` is only available if the board provides
 * feature `periph_sdmmc_8bit`.
 */
typedef struct {
    SDMMC_TypeDef *dev;         /**< SDIO/SDMMC device */
    uint8_t bus;                /**< APB/AHB bus used for SDIO/SDMMC peripheral */
    uint32_t rcc_mask;          /**< Bit mask in clock enable register */
    gpio_t cd;                  /**< Card Detect pin (GPIO_UNDEF if not used) */
    int cd_active;              /**< Card Detect pin active level */
    gpio_mode_t cd_mode;        /**< Card Detect pin mode */
    sdmmc_pin_t clk;            /**< Clock pin */
    sdmmc_pin_t cmd;            /**< Command/Response pin */
    sdmmc_pin_t dat0;           /**< Data Line Bit 0 pin */
    sdmmc_pin_t dat1;           /**< Data Line Bit 1 pin */
    sdmmc_pin_t dat2;           /**< Data Line Bit 2 pin */
    sdmmc_pin_t dat3;           /**< Data Line Bit 3 pin */
#if IS_USED(MODULE_PERIPH_SDMMC_8BIT)
    sdmmc_pin_t dat4;           /**< Data Line Bit 4 pin */
    sdmmc_pin_t dat5;           /**< Data Line Bit 5 pin */
    sdmmc_pin_t dat6;           /**< Data Line Bit 6 pin */
    sdmmc_pin_t dat7;           /**< Data Line Bit 7 pin */
#endif
#if IS_USED(MODULE_PERIPH_DMA)
    dma_t dma;                  /**< Logical DMA stream used for SDIO/SDMMC */
    uint8_t dma_chan;           /**< DMA channel used for SDIO/SDMMC */
#endif
    uint8_t irqn;               /**< SDIO/SDMMC interrupt number */
} sdmmc_conf_t;

#endif /* defined(SDMMC_POWER_PWRCTRL) || defined(SDIO_POWER_PWRCTRL) */

#ifdef __cplusplus
}
#endif

/** @} */
