/*
 * Copyright (C) 2015 Engineering-Spirit
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f2
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Aurelien Gonce <aurelien.gonce@altran.fr>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))

/**
 * @brief   Override GPIO mode options
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< open-drain w/o pull R */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1)     /**< open-drain with pull-up */
} gpio_mode_t;
/** @} */

/**
 * @brief   Available ports on the STM32F2 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    PORT_H = 7,             /**< port H */
    PORT_I = 8              /**< port I */
};

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x03000000,  /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x02000000,  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01000000,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x00000000,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 1,           /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = 2            /**< ADC resolution: 16 bit (not supported)*/
} adc_res_t;
/** @} */

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @brief   Power on the DMA device the given stream belongs to
 *
 * @param[in] stream    logical DMA stream
 */
static inline void dma_poweron(int stream)
{
    if (stream < 8) {
        periph_clk_en(AHB1, RCC_AHB1ENR_DMA1EN);
    } else {
        periph_clk_en(AHB1, RCC_AHB1ENR_DMA2EN);
    }
}

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA stream handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 stream0 equals 0, DMA2 stream0 equals 8,
 * DMA2 stream 7 equals 15 and so on.
 *
 * @param[in] stream    logical DMA stream
 */
static inline DMA_TypeDef *dma_base(int stream)
{
    return (stream < 8) ? DMA1 : DMA2;
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] stream    logical DMA stream
 *
 * @return  base address for the selected DMA stream
 */
static inline DMA_Stream_TypeDef *dma_stream(int stream)
{
    uint32_t base = (uint32_t)dma_base(stream);
    return (DMA_Stream_TypeDef *)(base + (0x10 + (0x18 * (stream & 0x7))));
}

/**
 * @brief   Select high or low DMA interrupt register based on stream number
 *
 * @param[in] stream    logical DMA stream
 *
 * @return  0 for streams 0-3, 1 for streams 3-7
 */
static inline int dma_hl(int stream)
{
    return ((stream & 0x4) >> 2);
}

/**
 * @brief   Get the interrupt flag clear bit position in the DMA LIFCR register
 *
 * @param[in] stream    logical DMA stream
 */
static inline uint32_t dma_ifc(int stream)
{
    switch (stream & 0x3) {
        case 0: /* 0 and 4 */
            return (1 << 5);
        case 1: /* 1 and 5 */
            return (1 << 11);
        case 2: /* 2 and 6 */
            return (1 << 21);
        case 3: /* 3 and 7 */
            return (1 << 27);
        default:
            return 0;
    }
}

/**
 * @brief   Enable DMA interrupts
 *
 * @param[in] stream    logical DMA stream
 */
static inline void dma_isr_enable(int stream)
{
    if (stream < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 7) {
        NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    }
    else if (stream < 13) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream - 8)));
    }
    else if (stream < 16) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream - 13)));
    }
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
