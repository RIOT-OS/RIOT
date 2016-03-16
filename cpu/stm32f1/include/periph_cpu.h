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
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

/**
 * @brief   All timers for the STM32F1 have 4 CC channels
 */
#define TIMER_CHANNELS      (4U)

/**
 * @brief   All timers have a width of 16-bit
 */
#define TIMER_MAXVAL        (0xffff)

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
 * @brief   Define alternate function modes
 *
 * On this CPU, only the output pins have alternate function modes. The input
 * pins have to be configured using the default gpio_init() function.
 */
typedef enum {
    GPIO_AF_OUT_PP = 0xb,   /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD = 0xf,   /**< alternate function output - open-drain */
} gpio_af_out_t;

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIM_TypeDef *dev;       /**< timer device */
    uint32_t rcc_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
} timer_conf_t;

/**
 * @brief   UART configuration options
 */
typedef struct {
    USART_TypeDef *dev;     /**< UART device */
    gpio_t rx_pin;          /**< TX pin */
    gpio_t tx_pin;          /**< RX pin */
    uint32_t rcc_pin;       /**< bit in the RCC register */
    uint8_t bus;            /**< peripheral bus */
    uint8_t irqn;           /**< interrupt number */
} uart_conf_t;

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @note    This is meant for internal use in STM32F1 peripheral drivers only
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_out_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

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
