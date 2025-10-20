/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  CPU specific definitions for internal peripheral handling
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#if !defined(CPUID_LEN) || defined(DOXYGEN)
#  define CPUID_LEN           (4U)
#endif

/**
 * @brief Power mode configuration
 */
#if !defined(PM_NUM_MODES) || defined(DOXYGEN)
#  define PM_NUM_MODES        (1U)
#endif

/**
 * @brief   We provide our own pm_off() function for the native CPU
 */
#define PROVIDES_PM_LAYERED_OFF

/* GPIO configuration only if the module is available (=Linux) */
#if defined(MODULE_PERIPH_GPIO_LINUX) || defined(DOXYGEN)
#  include <linux/gpio.h>

/* MARK: - GPIO Configuration */
/**
 * @name GPIO Configuration
 * @{
 */
/**
 * @brief   The offset between Port and Pin
 */
#  define GPIO_PORT_SHIFT     (24)

/**
 * @brief   Define a custom GPIO_PIN macro for native
 */
#  define GPIO_PIN(port, pin) (gpio_t)((port << GPIO_PORT_SHIFT) | pin)

/**
 * @brief Macro indicating whether GPIO modes are available on the native CPU
 */
#  define HAVE_GPIO_MODE_T
/**
 * @brief Pull-down
 */
#  if !defined(GPIOHANDLE_REQUEST_PULL_DOWN) || defined(DOXYGEN)
#    if defined(GPIOHANDLE_REQUEST_BIAS_PULL_DOWN)
#      define GPIOHANDLE_REQUEST_PULL_DOWN    GPIOHANDLE_REQUEST_BIAS_PULL_DOWN
#    else
#      define GPIOHANDLE_REQUEST_PULL_DOWN    (0xFF)
#    endif
#  endif
/**
 * @brief Pull-up
 */
#  if !defined(GPIOHANDLE_REQUEST_PULL_UP) || defined(DOXYGEN)
#    if defined(GPIOHANDLE_REQUEST_BIAS_PULL_UP)
#      define GPIOHANDLE_REQUEST_PULL_UP      GPIOHANDLE_REQUEST_BIAS_PULL_UP
#    else
#      define GPIOHANDLE_REQUEST_PULL_UP      (0xFF)
#    endif
#  endif

/**
 * @brief   Available pin modes
 *
 * Generally, a pin can be configured to be input or output. In output mode, a
 * pin can further be put into push-pull or open drain configuration. Though
 * this is supported by most platforms, this is not always the case, so driver
 * implementations may return an error code if a mode is not supported.
 */
typedef enum {
    GPIO_IN    = GPIOHANDLE_REQUEST_INPUT,
    GPIO_IN_PD = GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_PULL_DOWN,
    GPIO_IN_PU = GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_PULL_UP,
    GPIO_OUT   = GPIOHANDLE_REQUEST_OUTPUT,
    GPIO_OD    = GPIOHANDLE_REQUEST_OPEN_DRAIN,
    GPIO_OD_PU = GPIOHANDLE_REQUEST_OPEN_DRAIN | GPIOHANDLE_REQUEST_PULL_UP
} gpio_mode_t;

/**
 * @brief A macro indicating whether the native CPU supports GPIO edge behavior
 */
#  define HAVE_GPIO_FLANK_T

/**
 * @brief An enum for the type of flank that emit interrupts
 */
typedef enum {
    GPIO_FALLING = GPIOEVENT_EVENT_FALLING_EDGE,    /**< emit interrupt on falling flank */
    GPIO_RISING = GPIOEVENT_EVENT_RISING_EDGE,      /**< emit interrupt on rising flank */
    GPIO_BOTH = GPIO_FALLING | GPIO_RISING          /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

#elif defined(MODULE_PERIPH_GPIO_MOCK)

/**
 * @brief   Mocked GPIO
 *
 * Mocked GPIO representation for simulation.
 */
typedef struct {
    int value;              /**< current value */
    int mode;               /**< current mode */
    int flank;              /**< flank to trigger interrupts */
    void (*cb)(void *arg);  /**< ISR */
    void *arg;              /**< ISR arg */
} gpio_mock_t;

#  define GPIO_UNDEF          0

#  if !defined(GPIO_PORT_MAX) || defined(DOXYGEN)
#    define GPIO_PORT_MAX       (16)
#  endif

#  if !defined(GPIO_PIN_MAX) || defined(DOXYGEN)
#    define GPIO_PIN_MAX        (32)
#  endif

/**
 * @brief Mocked GPIO array
 */
extern gpio_mock_t gpio_mock[GPIO_PORT_MAX][GPIO_PIN_MAX];

#  define HAVE_GPIO_T
/**
 * @brief   Pointer on a mocked GPIO
 */
typedef gpio_mock_t* gpio_t;

/**
 * @brief   Define a custom GPIO_PIN macro for native mocked GPIO framework.
 *          Get the mocked GPIO object from mocked GPIO array.
 */
#  define GPIO_PIN(port, pin) \
    (((port >= 0) && (pin >= 0) && (port < GPIO_PORT_MAX) && (pin < GPIO_PIN_MAX)) \
     ? &gpio_mock[port][pin] \
     : GPIO_UNDEF)

#endif /* MODULE_PERIPH_GPIO_LINUX | DOXYGEN */

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/* MARK: - Power management configuration*/
/**
 * @name Power management configuration
 * @{
 */
#define PROVIDES_PM_OFF
#define PROVIDES_PM_SET_LOWEST
/** @} */

/* Configuration for the wrapper around the Linux SPI API (periph_spidev_linux)
 *
 * Needs to go here, otherwise the SPI_NEEDS_ are defined after inclusion of
 * spi.h.
 */
#if defined(MODULE_PERIPH_SPIDEV_LINUX) || defined(DOXYGEN)
/* MARK: - SPI Configuration */
/**
 * @name SPI Configuration
 * @{
 */
/**
 * @brief   Use the common `transfer_byte` SPI function
 */
#  define PERIPH_SPI_NEEDS_TRANSFER_BYTE
/**
 * @brief   Use the common `transfer_reg` SPI function
 */
#  define PERIPH_SPI_NEEDS_TRANSFER_REG
/**
 * @brief   Use the common `transfer_regs` SPI function
 */
#  define PERIPH_SPI_NEEDS_TRANSFER_REGS

#  ifndef DOXYGEN
/**
 * @brief   Use a custom clock speed type
 */
#    define HAVE_SPI_CLK_T
/**
 * @brief   SPI clock speed values
 *
 * The Linux userspace driver takes values in Hertz, which values are available
 * can only be determined at runtime.
 */
typedef enum {
    SPI_CLK_100KHZ = (100000U),
    SPI_CLK_400KHZ = (400000U),
    SPI_CLK_1MHZ   = (1000000U),
    SPI_CLK_5MHZ   = (5000000U),
    SPI_CLK_10MHZ  = (10000000U)
} spi_clk_t;
#  endif /* ndef DOXYGEN */
/** @} */
#endif /* MODULE_PERIPH_SPI | DOXYGEN */

/**
 * @brief EEPROM configuration
 */
#if !defined(EEPROM_SIZE) || defined(DOXYGEN)
#  define EEPROM_SIZE             (1024U)  /* 1kB */
#endif

#ifdef MODULE_PERIPH_CAN
#  include "candev_linux.h"
#endif

#ifdef __cplusplus
}
#endif

/** @} */
