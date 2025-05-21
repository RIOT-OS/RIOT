/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Leon M. George <leon@georgemail.de>
 */


#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (&FCFG->MAC_BLE_0)
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST_CORTEXM
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override GPIO mode values
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = (IOCFG_INPUT_ENABLE | IOCFG_PULLCTL_OFF),      /**< input w/o pull R */
    GPIO_IN_PD = (IOCFG_INPUT_ENABLE | IOCFG_PULLCTL_DOWN),     /**< input with pull-down */
    GPIO_IN_PU = (IOCFG_INPUT_ENABLE | IOCFG_PULLCTL_UP),       /**< input with pull-up */
    GPIO_OUT   = (IOCFG_PULLCTL_OFF),                           /**< push-pull output */
    GPIO_OD    = (IOCFG_IOMODE_OPEN_DRAIN | IOCFG_PULLCTL_OFF), /**< open-drain w/o pull R */
    GPIO_OD_PU = (IOCFG_IOMODE_OPEN_DRAIN | IOCFG_PULLCTL_UP)   /**< open-drain with pull-up */
} gpio_mode_t;

/**
 * @brief   Override GPIO flank values
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = IOCFG_EDGEDET_FALLING,
    GPIO_RISING = IOCFG_EDGEDET_RISING,
    GPIO_BOTH = IOCFG_EDGEDET_BOTH
} gpio_flank_t;

/**
 * @brief   CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)  (((x) & 0) | (y))

/*
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
   UART_PARITY_NONE = 0,
   UART_PARITY_EVEN = (UART_LCRH_PEN | UART_LCRH_EPS),
   UART_PARITY_ODD = UART_LCRH_PEN,
   UART_PARITY_MARK = UART_INVALID_MODE | 4,
   UART_PARITY_SPACE = UART_INVALID_MODE | 5
} uart_parity_t;
/** @} */

 /**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = UART_LCRH_WLEN_5,
    UART_DATA_BITS_6 = UART_LCRH_WLEN_6,
    UART_DATA_BITS_7 = UART_LCRH_WLEN_7,
    UART_DATA_BITS_8 = UART_LCRH_WLEN_8
} uart_data_bits_t;
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,
   UART_STOP_BITS_2 = UART_LCRH_STP2,
} uart_stop_bits_t;
/** @} */

/**
 * @brief   UART device configuration
 * @{
 */
typedef struct {
   uart_regs_t *regs;
   int tx_pin;
   int rx_pin;
#ifdef MODULE_PERIPH_UART_HW_FC
   int rts_pin;
   int cts_pin;
#endif
   int intn;
} uart_conf_t;
/** @} */

/**
 * @brief   Configuration of low-level general purpose timers
 *
 * General purpose timers (GPT[0-3]) are configured consecutively and in order
 * (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
 */
typedef struct {
    uint8_t     cfg;    /**< timer config [16,32 Bit] */
    uint8_t     chn;    /**< number of channels [1,2] */
} timer_conf_t;

/**
 * @brief   Maximum number of channels
 *
 * @note    32 bit timers only support one channel instead of two. But knowing
 *          the worst case is useful e.g. for static allocation. Users are
 *          expected to either do proper error handling with `timer_set()` and
 *          `timer_set_absolute()`, or at least verify with
 *          @ref timer_query_channel_numof what the actual number of channels
 *          of a timer is.
 */
#define TIMER_CHANNEL_NUMOF 2

#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS

#endif /* ifndef DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
