/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rp2040
 * @{
 *
 * @file periph_cpu.h
 * @brief CPU definitions for handling peripherals
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "cpu.h"

#include "hardware/regs/uart.h"
#include "hardware/regs/xosc.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/watchdog.h"
#include "hardware/address_mapped.h"
#include "hardware/regs/addressmap.h"

#include "hardware/structs/uart.h"
#include "hardware/structs/uart.h"
#include "hardware/structs/xosc.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/resets.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/watchdog.h"
#include "hardware/structs/padsbank0.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief GPIO configuration options
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define GPIO_PIN(x, y) ((gpio_t)((x << 16) | y))

enum {
    GPIO_BANK_USER = 0,
    GPIO_BANK_QSPI = 1,
};

/**
 * @brief UART configuration options
 */
#define HAVE_UART_PARITY_T
typedef enum {

   UART_PARITY_ODD,
   UART_PARITY_EVEN,
   UART_PARITY_NONE,
} uart_parity_t;

#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = 5,
    UART_DATA_BITS_6,
    UART_DATA_BITS_7,
    UART_DATA_BITS_8,
} uart_data_bits_t;

#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,
   UART_STOP_BITS_2,
} uart_stop_bits_t;

typedef struct {
    gpio_t tx_pin;
    gpio_t rx_pin;
    uart_hw_t *dev;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
    uart_data_bits_t data_bits;
} uart_conf_t;

/**
 * @brief Timer configuration options
 */
#define TIM_FLAG_RESET_ON_SET (0U)
#define TIM_FLAG_RESET_ON_MATCH (0U)

typedef struct {
    uint32_t period_us;
} timer_channel_conf_t;

typedef struct {
    bool is_running;
    timer_channel_conf_t channel[4];
} timer_conf_t;

uint32_t get_clk_khz(unsigned int clk_src_idx);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
