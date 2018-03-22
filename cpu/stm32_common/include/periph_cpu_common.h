/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32_common
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CPU specific LSI clock speed
 */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
#define CLOCK_LSI           (40000U)
#elif defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1)
#define CLOCK_LSI           (37000U)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32L4)
#define CLOCK_LSI           (32000U)
#else
#error "error: LSI clock speed not defined for your target CPU"
#endif

/**
 * @brief   Length of the CPU_ID in octets
 *
 * This is the same for all members of the stm32 family
 */
#define CPUID_LEN           (12U)

/**
 * @brief   We provide our own pm_off() function for all STM32-based CPUs
 */
#define PROVIDES_PM_LAYERED_OFF

/**
 * @brief   All STM timers have 4 capture-compare channels
 */
#define TIMER_CHAN          (4U)

/**
 * @brief   All STM QDEC timers have 2 capture channels
 */
#define QDEC_CHAN           (2U)

/**
 * @brief   Use the shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Number of usable low power modes
 */
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L0) || defined(DOXYGEN)
#define PM_NUM_MODES    (2U)

/**
 * @name    Power modes
 * @{
 */
#define STM32_PM_SLEEP        (2U)
#define STM32_PM_STOP         (1U)
#define STM32_PM_STANDBY      (0U)
/** @} */
#endif

/**
 * @brief   Available peripheral buses
 */
typedef enum {
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
#if defined(CPU_FAM_STM32L0)
    AHB,            /**< AHB bus */
    IOP,            /**< IOP bus */
#elif defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1) \
    || defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    AHB,            /**< AHB bus */
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) \
    || defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7)
    AHB1,           /**< AHB1 bus */
    AHB2,           /**< AHB2 bus */
    AHB3            /**< AHB3 bus */
#else
#warning "unsupported stm32XX family"
#endif
} bus_t;

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

/**
 * @brief   Define a magic number that tells us to use hardware chip select
 *
 * We use a random value here, that does clearly differentiate from any possible
 * GPIO_PIN(x) value.
 */
#define SPI_HWCS_MASK       (0xffffff00)

/**
 * @brief   Override the default SPI hardware chip select access macro
 *
 * Since the CPU does only support one single hardware chip select line, we can
 * detect the usage of non-valid lines by comparing to SPI_HWCS_VALID.
 */
#define SPI_HWCS(x)         (SPI_HWCS_MASK | x)

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
#ifdef CPU_FAM_STM32F1
    GPIO_AF_OUT_PP = 0xb,   /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD = 0xf,   /**< alternate function output - open-drain */
#else
    GPIO_AF0 = 0,           /**< use alternate function 0 */
    GPIO_AF1,               /**< use alternate function 1 */
    GPIO_AF2,               /**< use alternate function 2 */
    GPIO_AF3,               /**< use alternate function 3 */
    GPIO_AF4,               /**< use alternate function 4 */
    GPIO_AF5,               /**< use alternate function 5 */
    GPIO_AF6,               /**< use alternate function 6 */
    GPIO_AF7,               /**< use alternate function 7 */
#ifndef CPU_FAM_STM32F0
    GPIO_AF8,               /**< use alternate function 8 */
    GPIO_AF9,               /**< use alternate function 9 */
    GPIO_AF10,              /**< use alternate function 10 */
    GPIO_AF11,              /**< use alternate function 11 */
    GPIO_AF12,              /**< use alternate function 12 */
    GPIO_AF13,              /**< use alternate function 13 */
    GPIO_AF14,              /**< use alternate function 14 */
    GPIO_AF15               /**< use alternate function 15 */
#endif
#endif
} gpio_af_t;

#ifndef CPU_FAM_STM32F1
/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO mode options
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< open-drain w/o pull R */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1)     /**< open-drain with pull-up */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */
#endif /* ndef CPU_FAM_STM32F1 */

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIM_TypeDef *dev;       /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint32_t rcc_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
} timer_conf_t;

/**
 * @brief   PWM channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    pwm_chan_t chan[TIMER_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0}
                                     *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
} pwm_conf_t;

/**
 * @brief   QDEC channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} qdec_chan_t;

/**
 * @brief   QDEC configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t max;                   /**< Maximum counter value */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    qdec_chan_t chan[QDEC_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0}
                                     *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
    uint8_t irqn;                   /**< global IRQ channel */
} qdec_conf_t;

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_TypeDef *dev;     /**< UART device base register address */
    uint32_t rcc_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t rx_af;        /**< alternate function for RX pin */
    gpio_af_t tx_af;        /**< alternate function for TX pin */
#endif
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
#if 0 /* TODO */
    uint8_t dma_stream;     /**< DMA stream used for TX */
    uint8_t dma_chan;       /**< DMA channel used for TX */
#endif
#ifdef MODULE_STM32_PERIPH_UART_HW_FC
    gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    gpio_t rts_pin;         /**< RTS pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t cts_af;       /**< alternate function for CTS pin */
    gpio_af_t rts_af;       /**< alternate function for RTS pin */
#endif
#endif
} uart_conf_t;

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    SPI_TypeDef *dev;       /**< SPI device base register address */
    gpio_t mosi_pin;        /**< MOSI pin */
    gpio_t miso_pin;        /**< MISO pin */
    gpio_t sclk_pin;        /**< SCLK pin */
    gpio_t cs_pin;          /**< HWCS pin, set to GPIO_UNDEF if not mapped */
#ifndef CPU_FAM_STM32F1
    gpio_af_t af;           /**< pin alternate function */
#endif
    uint32_t rccmask;       /**< bit in the RCC peripheral enable register */
    uint8_t apbbus;         /**< APBx bus the device is connected to */
} spi_conf_t;

/**
 * @brief   Get the actual bus clock frequency for the APB buses
 *
 * @param[in] bus       target APBx bus
 *
 * @return              bus clock frequency in Hz
 */
uint32_t periph_apb_clk(uint8_t bus);

/**
 * @brief   Get the actual timer clock frequency
 *
 * @param[in] bus       corresponding APBx bus
 *
 * @return              timer clock frequency in Hz
 */
uint32_t periph_timer_clk(uint8_t bus);

/**
 * @brief   Enable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_dis(bus_t bus, uint32_t mask);

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
