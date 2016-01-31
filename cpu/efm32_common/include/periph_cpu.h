/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#include "mutex.h"

#include "cpu_conf.h"

#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_adc.h"

#ifdef _SILICON_LABS_32B_PLATFORM_1
#include "em_dac.h"
#else
#include "em_idac.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable support for Low-power peripherals (if supported by CPU).
 * @{
 */
#ifndef LOW_POWER_ENABLED
#define LOW_POWER_ENABLED   (1)
#endif
/** @} */

/**
 * @brief   Override the timer undefined value
 */
#define TIMER_UNDEF         (0xffffffff)

/**
 * @brief   Override the timer type
 * @{
 */
#define HAVE_TIMER_T
typedef uint32_t tim_t;
/** @} */

/**
 * @brief   Define timer configuration values
 *
 * @note    The two timers must be adjacent to each other (e.g. TIMER0 and
 *          TIMER1, or TIMER2 and TIMER3, etc.).
 * @{
 */
typedef struct {
    TIMER_TypeDef *dev;
    CMU_Clock_TypeDef cmu;
} timer_dev_t;

typedef struct {
    timer_dev_t prescaler;     /**< the lower numbered neighboring timer */
    timer_dev_t timer;         /**< the higher numbered timer */
    IRQn_Type irq;             /**< number of the higher timer IRQ channel */
} timer_conf_t;
/** @} */

/**
 * @brief   Define a custom type for GPIO pins
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
 * @brief   Mandatory function for defining a GPIO pins
 */
#define GPIO_PIN(x, y)      ((gpio_t) ((x << 4) | y))

/**
 * @brief   Available ports on the EFM32
 */
enum {
    PA = gpioPortA,         /**< port A */
    PB = gpioPortB,         /**< port B */
    PC = gpioPortC,         /**< port C */
    PD = gpioPortD,         /**< port D */
#if _GPIO_PORT_E_PIN_COUNT
    PE = gpioPortE,         /**< port E */
#endif
    PF = gpioPortF          /**< port F */
};

/**
 * @brief   Override direction values
 * @{
 */
#define HAVE_GPIO_DIR_T
typedef enum {
    GPIO_DIR_IN = 0,        /**< configure pin as input */
    GPIO_DIR_OUT = 1,       /**< configure pin as output */
    GPIO_DIR_BI = 2         /**< configure pin as bidirectional (open drain) */
} gpio_dir_t;
/** @} */

/**
 * @brief   Override pull register configuration values
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 0,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 1,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 2       /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @brief   ADC device configuration
 * @{
 */
typedef struct {
#ifdef _SILICON_LABS_32B_PLATFORM_1
    ADC_SingleInput_TypeDef input;  /**< input channel */
#else
    ADC_PosSel_TypeDef input;       /**< input channel */
#endif
    ADC_Ref_TypeDef reference;      /**< channel voltage reference */
    ADC_AcqTime_TypeDef acq_time;   /**< channel acquisition time */
} adc_chan_conf_t;

typedef struct {
    ADC_TypeDef *dev;       /**< ADC device used */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    uint8_t channels;       /**< the number of ADC channels */
    uint8_t channel_offset; /**< channel offset in channel config */
} adc_conf_t;
/** @} */

/**
 * @brief   DAC device configuration
 * @{
 */
#if defined(DAC_COUNT) && DAC_COUNT > 0
typedef struct {
    uint8_t index;          /**< channel index */
} dac_chan_conf_t;

typedef struct {
    DAC_TypeDef *dev;       /**< DAC device used */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    DAC_Ref_TypeDef ref;    /**< the DAC internal reference */
    uint8_t channels;       /**< the number of DAC channels */
    uint8_t channel_offset; /**< channel offset in channel config */
} dac_conf_t;
#endif
/** @} */

/**
 * @brief   I2C device configuration
 */
typedef struct {
    I2C_TypeDef *dev;       /**< USART device used */
    gpio_t sda_pin;         /**< pin used for SDA */
    gpio_t scl_pin;         /**< pin used for SCL */
    uint32_t loc;           /**< location of I2C pins */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
    mutex_t lock;           /**< lock for exclusive access to bus */
} i2c_conf_t;

/**
 * @brief   PWM device configuration
 * @{
 */
typedef struct {
    uint8_t index;          /**< TIMER channel to use */
    gpio_t pin;             /**< GPIO pin */
    uint32_t loc;           /**< location of the pin */
} pwm_chan_conf_t;

typedef struct {
    TIMER_TypeDef *dev;     /**< TIMER device used */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
    uint8_t channels;       /**< the number of available channels */
    uint8_t channel_offset; /**< channel offset in channel config */
} pwm_conf_t;
/** @} */

/**
 * @brief   SPI device configuration
 */
typedef struct {
    USART_TypeDef *dev;     /**< USART device used */
    gpio_t mosi_pin;        /**< Pin used for MOSI */
    gpio_t miso_pin;        /**< Pin used for MISO */
    gpio_t clk_pin;         /**< Pin used for CLK */
    uint32_t loc;           /**< location of USART pins */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
    mutex_t lock;           /**< lock for exclusive access to bus */
} spi_dev_t;

/**
 * @brief   UART device configuration
 */
typedef struct {
    void *dev;              /**< UART, USART or LEUART device used */
    gpio_t rx_pin;          /**< Pin used for RX */
    gpio_t tx_pin;          /**< Pin used for TX */
    uint32_t loc;           /**< location of USART pins */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
} uart_conf_t;

/**
 * @brief   Declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H_ */
/** @} */
