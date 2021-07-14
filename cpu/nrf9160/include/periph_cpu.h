/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf9160
 * @{
 *
 * @file
 * @brief           nRF9160 specific definitions for handling peripherals
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 64MHz for all NRF9160 CPUs
 */
#define CLOCK_CORECLOCK     MHZ(64)

/**
 * @brief    Peripheral clock speed (fixed to 16MHz for nRF9160 based CPUs)
 */
#define PERIPH_CLOCK        MHZ(16)

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    NRF_UARTE_Type *dev;    /**< UART with EasyDMA device base
                             * register address */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t rts_pin;         /**< RTS pin */
    gpio_t cts_pin;         /**< CTS pin */
#endif
    uint8_t irqn;           /**< IRQ channel */
} uart_conf_t;

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE    (64)
#endif

#ifndef DOXYGEN
/**
 * @brief Wrapper to fix differences between nRF9160 and
  *       nRF52 vendor files
  */
#define UART_BAUDRATE_BAUDRATE_Baud1200 UARTE_BAUDRATE_BAUDRATE_Baud1200
#define UART_BAUDRATE_BAUDRATE_Baud2400 UARTE_BAUDRATE_BAUDRATE_Baud2400
#define UART_BAUDRATE_BAUDRATE_Baud4800 UARTE_BAUDRATE_BAUDRATE_Baud4800
#define UART_BAUDRATE_BAUDRATE_Baud9600 UARTE_BAUDRATE_BAUDRATE_Baud9600
#define UART_BAUDRATE_BAUDRATE_Baud14400 UARTE_BAUDRATE_BAUDRATE_Baud14400
#define UART_BAUDRATE_BAUDRATE_Baud19200 UARTE_BAUDRATE_BAUDRATE_Baud19200
#define UART_BAUDRATE_BAUDRATE_Baud28800 UARTE_BAUDRATE_BAUDRATE_Baud28800
#define UART_BAUDRATE_BAUDRATE_Baud31250 UARTE_BAUDRATE_BAUDRATE_Baud31250
#define UART_BAUDRATE_BAUDRATE_Baud38400 UARTE_BAUDRATE_BAUDRATE_Baud38400
#define UART_BAUDRATE_BAUDRATE_Baud56000 UARTE_BAUDRATE_BAUDRATE_Baud56000
#define UART_BAUDRATE_BAUDRATE_Baud57600 UARTE_BAUDRATE_BAUDRATE_Baud57600
#define UART_BAUDRATE_BAUDRATE_Baud76800 UARTE_BAUDRATE_BAUDRATE_Baud76800
#define UART_BAUDRATE_BAUDRATE_Baud115200 UARTE_BAUDRATE_BAUDRATE_Baud115200
#define UART_BAUDRATE_BAUDRATE_Baud230400 UARTE_BAUDRATE_BAUDRATE_Baud230400
#define UART_BAUDRATE_BAUDRATE_Baud250000 UARTE_BAUDRATE_BAUDRATE_Baud250000
#define UART_BAUDRATE_BAUDRATE_Baud460800 UARTE_BAUDRATE_BAUDRATE_Baud460800
#define UART_BAUDRATE_BAUDRATE_Baud921600 UARTE_BAUDRATE_BAUDRATE_Baud921600
#define UART_BAUDRATE_BAUDRATE_Baud1M UARTE_BAUDRATE_BAUDRATE_Baud1M
#endif /* ndef DOXYGEN */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
