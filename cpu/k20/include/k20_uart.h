/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k20
 * @{
 *
 * @file
 * @brief           k20 specific UART functions
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 * @}
 */


#ifndef __K20_UART_H
#define __K20_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief K20 UART output enum
 *
 * It selects between the different pins:
 *
 * 0 - RX: PTA1  ;  TX: PTA2
 * 1 - RX: PTB16 ;  TX: PTB17
 * 2 - RX: PTD6  ;  TX: PTD7
 * Default value is PTD 6/7
 */
typedef enum {
    K20_UART0_OUTPUT_PTD_6_7 = 0,
    K20_UART0_OUTPUT_PTA_1_2 = 1,
    K20_UART0_OUTPUT_PTB_16_17 = 2,
} k20_uart0_output_t;

/**
 * @brief Select UART0 pins to use
 *
 * Run this function before setting up UART0
 *
 * @param[in]  output    The output pins to use for UART0
 */
void k20_uart0_select_output(k20_uart0_output_t output);

#ifdef __cplusplus
}
#endif

#endif /* __K20_UART_H */
