/*
 * Copyright (C) 2023 Freie Universität Berlin
 *               2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       I2S CPU specific definitions for the STM32 family
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_CPU_I2S_H
#define PERIPH_CPU_I2S_H

#include <stdint.h>

#include "cpu.h"
#include "macros/units.h"
#include "periph/cpu_dma.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Align by full words for the DMA
 */
#define I2S_CPU_DMA_ALIGNMENT 4

/**
 * @brief   Structure for I2S configuration data
 */
typedef struct {
    SPI_TypeDef *dev;       /**< SPI/I2S device base register address */
    gpio_t data_pin;        /**< Data In/Out pin */
    gpio_t bclk_pin;        /**< Bit clock pin */
    gpio_t wclk_pin;        /**< Word/LR clock pin */
    gpio_t mclk_pin;        /**< Master clock pin */
    uint32_t rccmask;       /**< bit in the RCC peripheral enable register */
    uint8_t apbbus;         /**< APBx bus the device is connected to */
    uint32_t input_clock;   /**< Peripheral input clock frequency */
    dma_t dma;           /**< Logical DMA stream used for TX */
    uint8_t dma_chan;    /**< DMA channel used for TX */
    uint8_t irqn;           /**< IRQ channel */
} i2s_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_I2S_H */
/** @} */
