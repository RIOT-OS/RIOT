/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
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
 * @brief           UART CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph/cpu_dma.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART hardware module types
 */
typedef enum {
    STM32_USART,            /**< STM32 USART module type */
    STM32_LPUART,           /**< STM32 Low-power UART (LPUART) module type */
} uart_type_t;

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE    (64)
#endif

#ifndef DOXYGEN
/**
 * @brief   Invalid UART mode mask
 *
 * This mask is also used to force data_bits_t to be uint32_t type
 * since it may be assigned a uint32_t variable in uart_mode
 */
#define UART_INVALID_MODE   (0x8000000)

/**
 * @brief   Override parity values
 * @{
 */
#define HAVE_UART_PARITY_T
typedef enum {
   UART_PARITY_NONE = 0,                               /**< no parity */
   UART_PARITY_EVEN = USART_CR1_PCE,                   /**< even parity */
   UART_PARITY_ODD = (USART_CR1_PCE | USART_CR1_PS),   /**< odd parity */
   UART_PARITY_MARK = UART_INVALID_MODE | 4,           /**< not supported */
   UART_PARITY_SPACE = UART_INVALID_MODE  | 5          /**< not supported */
} uart_parity_t;
/** @} */

/**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = UART_INVALID_MODE | 1,   /**< not supported */
    UART_DATA_BITS_6 = UART_INVALID_MODE | 2,   /**< not supported unless parity is set */
#if defined(USART_CR1_M1)
    UART_DATA_BITS_7 = USART_CR1_M1,            /**< 7 data bits */
#else
    UART_DATA_BITS_7 = UART_INVALID_MODE | 3,   /**< not supported unless parity is set */
#endif
    UART_DATA_BITS_8 = 0,                       /**< 8 data bits */
} uart_data_bits_t;
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,                  /**< 1 stop bit */
   UART_STOP_BITS_2 = USART_CR2_STOP_1,   /**< 2 stop bits */
} uart_stop_bits_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_TypeDef *dev;     /**< UART device base register address */
    uint32_t rcc_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t rx_af;        /**< alternate function for RX pin */
    gpio_af_t tx_af;        /**< alternate function for TX pin */
#endif
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    gpio_t rts_pin;         /**< RTS pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t cts_af;       /**< alternate function for CTS pin */
    gpio_af_t rts_af;       /**< alternate function for RTS pin */
#endif
#endif
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32MP1) || defined(CPU_FAM_STM32WL)
    uart_type_t type;       /**< hardware module type (USART or LPUART) */
    uint32_t clk_src;       /**< clock source used for UART */
#endif
#ifdef MODULE_PERIPH_DMA
    dma_t dma;              /**< Logical DMA stream used for TX */
    uint8_t dma_chan;       /**< DMA channel used for TX */
#endif
} uart_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
