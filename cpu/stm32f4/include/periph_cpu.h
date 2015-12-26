/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f4
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Available ports on the STM32F4 family
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
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
    GPIO_AF0 = 0,           /**< use alternate function 0 */
    GPIO_AF1,               /**< use alternate function 1 */
    GPIO_AF2,               /**< use alternate function 2 */
    GPIO_AF3,               /**< use alternate function 3 */
    GPIO_AF4,               /**< use alternate function 4 */
    GPIO_AF5,               /**< use alternate function 5 */
    GPIO_AF6,               /**< use alternate function 6 */
    GPIO_AF7,               /**< use alternate function 7 */
    GPIO_AF8,               /**< use alternate function 8 */
    GPIO_AF9,               /**< use alternate function 9 */
    GPIO_AF10,              /**< use alternate function 10 */
    GPIO_AF11,              /**< use alternate function 11 */
    GPIO_AF12,              /**< use alternate function 12 */
    GPIO_AF13,              /**< use alternate function 13 */
    GPIO_AF14               /**< use alternate function 14 */
} gpio_af_t;

/**
 * @brief   Structure for UART configuration data
 * @{
 */
typedef struct {
    USART_TypeDef *dev;     /**< UART device base register address */
    uint32_t rcc_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_af_t af;           /**< alternate pin function to use */
    uint8_t irqn;           /**< IRQ channel */
    uint8_t dma_stream;     /**< DMA stream used for TX */
    uint8_t dma_chan;       /**< DMA channel used for TX */
} uart_conf_t;
/** @} */

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @note    This is meant for internal use in STM32F4 peripheral drivers only
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Power on the DMA device the given stream belongs to
 *
 * @param[in] stream    logical DMA stream
 */
static inline void dma_poweron(int stream)
{
    if (stream < 8) {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    } else {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
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
        case 0:
            return (1 << 5);
        case 1:
            return (1 << 11);
        case 2:
            return (1 << 21);
        case 3:
            return (1 << 27);
        default:
            return 0;
    }
}

static inline void dma_isr_enable(int stream)
{
    if (stream < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 8) {
        NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    }
    else if (stream < 14) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + stream));
    }
    else if (stream < 17) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + stream));
    }
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
