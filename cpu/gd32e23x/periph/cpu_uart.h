/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       UART CPU specific definitions for the GD32E23x family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef PERIPH_CPU_UART_H
#define PERIPH_CPU_UART_H

#include <stdint.h>

#include "cpu.h"
// #include "periph/cpu_dma.h"
// #include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART hardware module types
 */
typedef enum {
    STM32_USART,
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
   UART_PARITY_NONE, //= USART_PM_NONE,                 /**< no parity */
   UART_PARITY_EVEN, //= USART_PM_EVEN,                 /**< even parity */
   UART_PARITY_ODD, //= USART_PM_ODD,                   /**< odd parity */
   UART_PARITY_MARK,// = UART_INVALID_MODE,             /**< not supported */
   UART_PARITY_SPACE,// = UART_INVALID_MODE             /**< not supported */
} uart_parity_t;
/** @} */

/**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5, //= UART_INVALID_MODE | 1,   /**< not supported */
    UART_DATA_BITS_6 ,//= UART_INVALID_MODE | 2,   /**< not supported unless parity is set */
    UART_DATA_BITS_7,// = UART_INVALID_MODE | 3,   /**< not supported unless parity is set */
    UART_DATA_BITS_8,// = 0,                       /**< 8 data bits */
} uart_data_bits_t;
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,                  /**< 1 stop bit */
   UART_STOP_BITS_2// = USART_CR2_STOP_1,   /**< 2 stop bits */
} uart_stop_bits_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_Type *dev;     /**< UART device base register address */
    uint32_t rcu_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_af_t rx_af;        /**< alternate function for RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_af_t tx_af;        /**< alternate function for TX pin */
    // gpio_t de_pin;          /**< DE pin */
    // gpio_af_t de_af;        /**< alternate function for DE pin */
    // gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    // gpio_af_t cts_af;       /**< alternate function for CTS pin */
    // gpio_t rts_pin;         /**< RTS pin */
    // gpio_af_t rts_af;       /**< alternate function for RTS pin */
    // gpio_t ck_pin;          /**< CK pin */
    // gpio_af_t ck_af;        /**< alternate function for CK pin */
//     uart_type_t type;       /**< hardware module type (USART or LPUART) */
//     uint32_t clk_src;       /**< clock source used for UART */
//     dma_t dma;              /**< Logical DMA stream used for TX */
//     uint8_t dma_chan;       /**< DMA channel used for TX */
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
} uart_conf_t;


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_UART_H */
/** @} */
