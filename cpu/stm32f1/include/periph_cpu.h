/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f1
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#undef PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 4 bit to determine the pin functions:
 * - bit 4: ODR value
 * - bit 2+3: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(mode, cnf, odr)       (mode | (cnf << 2) | (odr << 4))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO mode options
 *
 * We use 4 bit to encode CNF and MODE.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 2, 1),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(3, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(3, 1, 0),    /**< open-drain w/o pull R */
    GPIO_OD_PU = (0xff)                 /**< not supported by HW */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 4,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 9,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 8       /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Available ports on the STM32F1 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
};

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

#define PM_NUM_MODES    (2U)

/**
 * @brief   Power on the DMA device the given stream belongs to
 *
 * @param[in] channel   logical DMA channel
 */
static inline void dma_poweron(int channel)
{
#if defined(DMA2)
    if (channel < 7) {
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    } else {
        RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    }
#else
    (void) channel;

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
#endif
}

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA channel handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 channel1 equals 0, DMA2 channel1 equals 7,
 * DMA2 channel7 equals 14 and so on.
 *
 * @param[in] stream    logical DMA channel
 */
static inline DMA_TypeDef *dma_base(int channel)
{
#if defined(DMA2)
    return (channel < 7) ? DMA1 : DMA2;
#else
    return DMA1;
#endif
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] channel   logical DMA stream
 *
 * @return  base address for the selected DMA channel
 */
static inline DMA_Channel_TypeDef *dma_channel(int channel)
{
    /**
     * DMA Channel register map is calculated with:
     *      Address offset: 0x08 + 0d20 × (channel number – 1)
     *
     * STM32's Channel 1 is our Channel 0 so no - 1 needed
     */
    uint32_t base = (uint32_t)dma_base(channel);
    if (channel >= 7) {
        channel -= 7;
    }
    return (DMA_Channel_TypeDef *)(base + (0x08 + (0x14 * channel)));
}

/**
 * @brief   Get the transfer complete flag position for the corresponding channel
 *
 * @param[in] channel   logical DMA channel
 */
static inline uint32_t dma_tcif(int channel)
{
    /**
     * DMA_ISR & DMA_IFCR have per channel 4 interrupt bits
     * | b3      | b2     | b1       | b0     |
     * |--------------------------------------|
     * | TEIFx   | HTIFx  | TCIFx    | GIFx   |
     * | Error   | Half   | Complete | Global |
     *
     * `2 << (channel * 4)` marks the TCIFx bit
     */

    if (channel >= 7) {
        channel -= 7;
    }

    return (0x2 << (channel << 2));
}

static inline void dma_isr_enable(int channel)
{
    /**
     * | ch   | IRQn                | ISR   |
     * -------------------------------------|
     * | 0    | DMA1_Channel1_IRQn  | 11    |
     * | 1    | DMA1_Channel2_IRQn  | 12    |
     * | 2    | DMA1_Channel3_IRQn  | 13    |
     * | 3    | DMA1_Channel4_IRQn  | 14    |
     * | 4    | DMA1_Channel5_IRQn  | 15    |
     * | 5    | DMA1_Channel6_IRQn  | 16    |
     * | 6    | DMA1_Channel7_IRQn  | 17    |
     * | 7    | DMA2_Channel1_IRQn  | 56    |
     * | 8    | DMA2_Channel2_IRQn  | 57    |
     * | 9    | DMA2_Channel3_IRQn  | 58    |
     * | 10-11| DMA2_Channel4_5_IRQn| 59    |
     */

    if (channel < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Channel1_IRQn + channel));
    }
    else if (channel < 11) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Channel1_IRQn + (channel - 7)));
    }
    else if (channel == 11) {
        NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
