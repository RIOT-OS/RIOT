/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_qn908x
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          iosabi <iosabi@protonmail.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    CPU specific gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro.
 *
 * This generates the GPIO port base address with a mask of the GPIO_PIN value
 * to avoid a memory access. The value 12 here is selected as an optimization
 * to be able to derive the GPIO port address with a simple mask of the GPIO_PIN
 * value.
 */
#define GPIO_PIN(x, y)      (((x) << 12u) | (y))

/* QN908x has a unique default Bluetooth address in the Flash Information Page
 * descriptor. This value is set in the factory and cannot be modified by
 * users. However, the actual Bluetooth address used by the stack may be
 * different, this is just the default. */
/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (FSL_FEATURE_FLASH_ADDR_OF_VENDOR_BD_ADDR)

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (6U)

/**
 * @brief   Watchdog clock can be stopped independently of other clocks.
 */
#define WDT_HAS_STOP        (1)

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
/** The WDT clock can run up to 16MHz (via CLK_APB) and the WDT counter is
 * 32-bit so the maximum value in ms we can wait is ((1 << 32) - 1) / 16000.
 * TODO: A much larger limit (~1.5 days) can be set if the WDT runs from the
 * 32 KHz clock. However, this is likely decided by the board and depends on the
 * clocks installed on the board. Figure out a way to configure this limit based
 * on the clock used.
 */
#define NWDT_TIME_LOWER_LIMIT (1U)
#define NWDT_TIME_UPPER_LIMIT (268435U)
#define WWDT_TIME_LOWER_LIMIT (1U)
#define WWDT_TIME_UPPER_LIMIT (268435U)
/** @} */

/**
 * @brief   Generate GPIO mode bitfields
 *
 * The GPIO_MODE has the following structure:
 * - bit 0: open-drain: 1 for enabled (open-drain mode) and 0 for disabled.
 * - bit 1: output-enabled: 1 output mode, 0 input mode.
 * - bit 4-5: pull_mode: 0 for hi-z (no pull-up or down), 1 for pull-down and 2
 *            for pull-up. These correspond to the IOCON_MODE macros.
 */
#define GPIO_MODE(open_drain, out_enabled, pull_mode) \
    ((open_drain) | ((out_enabled) << 1) | ((pull_mode) << 4))

#ifndef DOXYGEN
/**
 * @name    GPIO pin modes
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN     = GPIO_MODE(0, 0, 0),       /**< IN */
    GPIO_IN_PD  = GPIO_MODE(0, 0, 1),       /**< IN with pull-down */
    GPIO_IN_PU  = GPIO_MODE(0, 0, 2),       /**< IN with pull-up */
    GPIO_OUT    = GPIO_MODE(0, 1, 0),       /**< OUT (push-pull) */
    GPIO_OD     = GPIO_MODE(1, 1, 0),       /**< OD */
    GPIO_OD_PU  = GPIO_MODE(1, 1, 2),       /**< OD with pull-up */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifndef DOXYGEN
/**
 * @name    GPIO flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LOW        = 0,    /**< emit interrupt when the value is low */
    GPIO_HIGH       = 1,    /**< emit interrupt when the value is high */
    GPIO_RISING     = 2,    /**< emit interrupt on rising flank */
    GPIO_FALLING    = 3,    /**< emit interrupt on falling flank */
    GPIO_BOTH       = 4,    /**< not supported -- rising and falling flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Available ports on the QN908x.
 */
enum {
    PORT_A  = 0,            /**< port A */
    PORT_B  = 1,            /**< port B */
    GPIO_PORTS_NUMOF        /**< overall number of available ports */
};

/**
 * @brief   CPU specific timer Counter/Timers (CTIMER) configuration
 * @{
 */
#define TIMER_CHANNELS      (4)
#define TIMER_MAX_VALUE     (0xffffffff)
/** @} */

/**
 * @brief UART module configuration options
 *
 * QN908x doesn't have any UART standalone blocks, but it has two FLEXCOMM
 * blocks that can be put in UART mode. The USART_Type* address is one of the
 * FLEXCOMM_Type* addresses as well.
 */
typedef struct {
    USART_Type *dev;    /**< Pointer to module hardware registers */
    gpio_t rx_pin;      /**< RX pin, GPIO_UNDEF disables RX. */
    gpio_t tx_pin;      /**< TX pin, GPIO_UNDEF disables TX. */
} uart_conf_t;

/**
 * @brief   Invalid UART mode mask
 *
 * Signals that the mode is invalid or not supported by the CPU.
 */
#define UART_INVALID_MODE   (0x80)

/**
 * @brief   Definition of possible parity modes
 *
 * These are defined to match the values of the USART->CFG : PARITYSEL bit
 * field.
 * @{
 */
typedef enum {
    UART_PARITY_NONE = 0,                         /**< no parity */
    UART_PARITY_EVEN = 2,                         /**< even parity */
    UART_PARITY_ODD = 3,                          /**< odd parity */
    UART_PARITY_MARK = 0x10 | UART_INVALID_MODE,  /**< mark parity */
    UART_PARITY_SPACE = 0x20 | UART_INVALID_MODE, /**< space parity */
} uart_parity_t;
#define HAVE_UART_PARITY_T
/** @} */

/**
 * @brief   Definition of possible data bits lengths in a UART frame
 *
 * These are defined to match the values of the USART->CFG : DATALEN bit field.
 * @{
 */
typedef enum {
    UART_DATA_BITS_5 = 0x10 | UART_INVALID_MODE, /**< 5 data bits */
    UART_DATA_BITS_6 = 0x20 | UART_INVALID_MODE, /**< 6 data bits */
    UART_DATA_BITS_7 = 0,                        /**< 7 data bits */
    UART_DATA_BITS_8 = 1,                        /**< 8 data bits */
    /* Note: There's a UART_DATA_BITS_9 possible in this hardware. */
} uart_data_bits_t;
#define HAVE_UART_DATA_BITS_T
/** @} */

/**
 * @brief   Definition of possible stop bits lengths
 *
 * These are defined to match the values of the USART->CFG : STOPLEN bit field.
 * @{
 */
typedef enum {
    UART_STOP_BITS_1    = 0,    /**< 1 stop bit */
    UART_STOP_BITS_2    = 1,    /**< 2 stop bits */
} uart_stop_bits_t;
#define HAVE_UART_STOP_BITS_T
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
