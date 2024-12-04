/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define GPIO_UNDEF          (0xffffffff)

#define GPIO_PIN(x, y)      (((uint32_t)PIOA + (x << 9)) | y)
#endif /* DOXYGEN */

/**
 * @name Declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   All SAM3 timers are 32-bit wide
 */
#define TIMER_MAX_VAL       (0xffffffff)

/**
 * @brief   We use one channel for each defined timer
 *
 * While the peripheral provides three channels, the current interrupt
 * flag handling leads to a race condition where calling timer_clear() on one
 * channel can disable a pending flag for other channels.
 * Until resolved, limit the peripheral to only one channel.
 */
#define TIMER_CHANNEL_NUMOF (1)

/**
 * @name    RTT configuration
 * @{
 */
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (CHIP_FREQ_XTAL_32K)          /* in Hz */
#define RTT_MIN_FREQUENCY   (1) /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)         /* in Hz */
/** @} */

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(io, pu, od)   (io | (pu << 1) | (od << 2))

/**
 * @name    ADC configuration, valid for all boards using this CPU
 *
 * The sam3 has a fixed mapping of ADC pins and a fixed number of ADC channels,
 * so this ADC configuration is valid for all boards using this CPU. No need for
 * any board specific configuration.
 */
#define ADC_NUMOF           (16U)

/**
 * @brief   DAC configuration, valid for all boards using this CPU
 *
 * The sam3 has a fixed mapping of DAC pins and a fixed number of DAC channels,
 * so this DAC configuration is valid for all boards using this CPU. No need for
 * any board specific configuration.
 *
 * The sam3's DAC channels are mapped to the following fixed pins:
 * - line 0 (ch0): PB15
 * - line 1 (ch1): PB16
 */
#define DAC_NUMOF           (2U)

#ifndef DOXYGEN
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< IN */
    GPIO_IN_PD = 0xf,                   /**< not supported by HW */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< OD */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1),    /**< OD with pull-up */
} gpio_mode_t;

#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* ndef DOXYGEN */

/**
 * @brief Available ports on the SAM3X8E
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
    PD = 3,                 /**< port D */
};

/**
 * @brief   GPIO mux configuration
 */
typedef enum {
    GPIO_MUX_A = 0,         /**< alternate function A */
    GPIO_MUX_B = 1,         /**< alternate function B */
} gpio_mux_t;

#ifndef DOXYGEN
/**
 * @brief   Override default SPI modes
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = (SPI_CSR_NCPHA),                   /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = (0),                               /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (SPI_CSR_CPOL | SPI_CSR_NCPHA),    /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CSR_CPOL)                     /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override default SPI clock values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = (100000),                      /**< 100KHz */
    SPI_CLK_400KHZ = (400000),                      /**< 400KHz */
    SPI_CLK_1MHZ   = (1000000),                     /**< 1MHz */
    SPI_CLK_5MHZ   = (5000000),                     /**< 5MHz */
    SPI_CLK_10MHZ  = (10000000)                     /**< 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifndef DOXYGEN
/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x1,                    /**< not applicable */
    ADC_RES_8BIT  = 0x2,                    /**< not applicable */
    ADC_RES_10BIT = ADC_MR_LOWRES_BITS_10,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_MR_LOWRES_BITS_12,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0x4,                    /**< not applicable */
    ADC_RES_16BIT = 0x8                     /**< not applicable */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Timer configuration data
 */
typedef struct {
    Tc *dev;                /**< timer device */
    uint8_t id_ch0;         /**< ID of the timer's first channel */
} timer_conf_t;

/**
 * @brief   UART configuration data
 */
typedef struct {
    Uart *dev;              /**< U(S)ART device used */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_mux_t mux;         /**< MUX used for pins */
    uint8_t pmc_id;         /**< bit in the PMC register of the device*/
    uint8_t irqn;           /**< interrupt number of the device */
} uart_conf_t;

/**
 * @brief   PWM channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin connected to the channel */
    uint8_t hwchan;         /**< the HW channel used for a logical channel */
} pwm_chan_conf_t;

/**
 * @brief   SPI configuration data
 */
typedef struct {
    Spi *dev;               /**< SPI module to use */
    uint8_t id;             /**< corresponding ID of that module */
    gpio_t clk;             /**< pin mapped to the CLK line */
    gpio_t mosi;            /**< pin mapped to the MOSI line */
    gpio_t miso;            /**< pin mapped to the MISO line */
    gpio_mux_t mux;         /**< pin MUX setting */
} spi_conf_t;

/**
 * @brief   Configure the given GPIO pin to be used with the given MUX setting
 *
 * @param[in] pin           GPIO pin to configure
 * @param[in] mux           MUX setting to use
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
