/*
 * Copyright (C) 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_fe310
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Ken Rabold
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <inttypes.h>

#include "periph_cpu_common.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 */
#define HAVE_GPIO_T
typedef uint8_t gpio_t;
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (12U)

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      (x | y)

/**
 * @brief   GPIO interrupt priority
 */
#define GPIO_INTR_PRIORITY  (3)

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    uint32_t addr;              /**< UART control register address */
    gpio_t rx;                  /**< RX pin */
    gpio_t tx;                  /**< TX pin */
    irqn_t isr_num;             /**< ISR source number */
} uart_conf_t;

/**
 * @brief   UART interrupt priority
 */
#define UART_ISR_PRIO       (2)

/**
 * @name    This CPU makes use of the following shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  1
#define PERIPH_SPI_NEEDS_TRANSFER_REG   1
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  1
/** @} */

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    uint32_t addr;          /**< SPI control register address */
    gpio_t mosi;            /**< MOSI pin */
    gpio_t miso;            /**< MISO pin */
    gpio_t sclk;            /**< SCLK pin */
} spi_conf_t;

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @name    Use the shared I2C functions
 * @{
 */
/** Use read reg function from periph common */
#define PERIPH_I2C_NEED_READ_REG
/** Use write reg function from periph common */
#define PERIPH_I2C_NEED_WRITE_REG
/** Use read regs function from periph common */
#define PERIPH_I2C_NEED_READ_REGS
/** Use write regs function from periph common */
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_NORMAL,       /**< normal mode:  ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:    ~400kbit/s */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C configuration options
 */
typedef struct {
    uint32_t addr;              /**< device base address */
    gpio_t scl;                 /**< SCL pin */
    gpio_t sda;                 /**< SDA pin */
    i2c_speed_t speed;          /**< I2C speed */
} i2c_conf_t;

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT           (1)
/* Ensure the internal "count" variable stays within the uint32 bounds.
   This variable corresponds to max_time * RTC_FREQ / MS_PER_SEC. On fe310,
   RTC_FREQ is 32768Hz. The 15 right shift is equivalent to a division by RTC_FREQ.
 */
#define NWDT_TIME_UPPER_LIMIT           ((UINT32_MAX >> 15)  * MS_PER_SEC + 1)
/** @} */

/**
 * @brief   WDT interrupt priority: use highest priority
 */
#define WDT_INTR_PRIORITY               (PLIC_NUM_PRIORITIES)

/**
 * @brief   WDT can be stopped
 */
#define WDT_HAS_STOP                    (1)

/**
 * @name    RTT/RTC configuration
 *
 * @{
 */
#define RTT_INTR_PRIORITY   (2)

#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (32768U)                /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)   /* in Hz */
#define RTT_MIN_FREQUENCY   (1U)                    /* in Hz */

#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (RTT_MAX_FREQUENCY)     /* in Hz */
#endif

/**
 * @brief   Initialization of the clock
 */
void fe310_clock_init(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
