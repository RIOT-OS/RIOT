/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <inttypes.h>

#include "cpu.h"
#include "clic.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
/** @} */

/**
 * @brief   On-Chip buses
 */
typedef enum {
    AHB,    /**< Advanced High-performance Bus */
    APB1,   /**< Advanced Peripheral Bus 1     */
    APB2,   /**< Advanced Peripheral Bus 2     */
} bus_t;

/**
 * @brief   Available GPIO ports
 */
enum {
#ifdef GPIOA
    PORT_A  = 0,            /**< port A */
#endif
#ifdef GPIOB
    PORT_B  = 1,            /**< port B */
#endif
#ifdef GPIOC
    PORT_C  = 2,            /**< port C */
#endif
#ifdef GPIOD
    PORT_D  = 3,            /**< port D */
#endif
#ifdef GPIOE
    PORT_E  = 4,            /**< port E */
#endif
};

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))
/**
 * @brief   Override GPIO mode options
 *
 * We use 4 bit to encode CNF and MODE.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN     = GPIO_MODE(0, 1, 0),   /**< input w/o pull R */
    GPIO_IN_PD  = GPIO_MODE(0, 2, 0),   /**< input with pull-down */
    GPIO_IN_PU  = GPIO_MODE(0, 2, 1),   /**< input with pull-up */
    GPIO_OUT    = GPIO_MODE(3, 0, 0),   /**< push-pull output */
    GPIO_OD     = GPIO_MODE(3, 1, 0),   /**< open-drain w/o pull R */
    GPIO_OD_PU  = (0xff)                /**< not supported by HW */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Override alternative GPIO mode options
 */
typedef enum {
    GPIO_AF_OUT_PP  = 0xb,  /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD  = 0xf,  /**< alternate function output - open-drain */
} gpio_af_t;

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIMER_Type *dev;        /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint32_t rcu_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
} timer_conf_t;

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_Type *dev;        /**< UART device base register address */
    uint32_t rcu_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
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
 * @brief   No brakes on the WDT train
 */
#define WDT_HAS_STOP                    (0)

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
 * @brief   Enable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCU enable register
 */
void periph_clk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCU enable register
 */
void periph_clk_dis(bus_t bus, uint32_t mask);

/**
 * @brief   Get the actual bus clock frequency for the APB buses
 *
 * @param[in] bus       target APBx bus
 *
 * @return              bus clock frequency in Hz
 */
uint32_t periph_apb_clk(bus_t bus);

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

void gd32vf103_clock_init(void);
void gd32v_enable_irc8(void);
void gd32v_disable_irc8(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
