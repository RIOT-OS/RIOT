/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf53
 * @{
 *
 * @file
 * @brief           nRF5340 specific definitions for handling peripherals
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
 * @brief    Peripheral clocks speed
 */
#define PERIPH_CLOCK_1MHZ   MHZ(1)          /**<  1MHz peripheral clock */
#define PERIPH_CLOCK_16MHZ  MHZ(16)         /**< 16MHz peripheral clock */
#define PERIPH_CLOCK_32MHZ  MHZ(32)         /**< 32MHz peripheral clock */
#define PERIPH_CLOCK_64MHZ  MHZ(64)         /**< 64MHz peripheral clock */
#define PERIPH_CLOCK        PERIPH_CLOCK_16MHZ /**< For driver compatibility */

#ifndef DOXYGEN
/**
 * @brief Wrapper to fix differences between nRF families vendor files
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
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
