/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_qn908x
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          iosabi <iosabi@protonmail.com>
 */


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

#if DOXYGEN
/**
 * @name    ADC CPU configuration
 * @{
 */
/**
 * @brief Define if ADC external capacitor is connected to PA06 pin.
 *
 * The ADC block can use an external capacitor to better stabilize the reference
 * voltage. This capacitor is optional, but if it is present on the board this
 * macro should be defined by the board to make the ADC block use it.
 */
#define BOARD_HAS_ADC_PA06_CAP
/** @} */
#endif

#ifndef DOXYGEN
/**
 * @name   ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT = 6u,         /**< ADC resolution: 6 bit + sign */
    ADC_RES_8BIT = 8u,         /**< ADC resolution: 8 bit + sign */
    ADC_RES_10BIT = 10u,       /**< ADC resolution: 10 bit + sign */
    ADC_RES_12BIT = 12u,       /**< ADC resolution: 12 bit + sign */
    ADC_RES_14BIT = 14u,       /**< ADC resolution: 14 bit + sign */
    ADC_RES_16BIT = 16u,       /**< ADC resolution: 16 bit + sign */
    /* Extra modes supported by this CPU. */
    ADC_RES_MAX = 22u,         /**< Full ADC resolution: 22 bit + sign */
    ADC_RES_UV = 23u,          /**< ADC resolution: signed int in uV */
} adc_res_t;
/** @} */
#endif /* ifndef DOXYGEN */

/**
 * @brief   ADC oversample clock configuration
 *
 * The ADC runs at a given ADC clock frequency which is derived from either the
 * high frequency clock (16 or 32 MHz) or the low frequency one (32 or
 * 32.768 KHz). Running the ADC from the 32 KHz source can be useful in low
 * power applications where the high speed clock is not running.
 *
 * The ADC sample rate for adc_sample() will be about 128 times slower than the
 * ADC clock, due to the decimation filter, meaning that the maximum sampling
 * rate is 31.25 KHz.
 */
typedef enum {
    ADC_CLOCK_4M,     /**< 4 MHz from the high speed clock. */
    ADC_CLOCK_2M,     /**< 2 MHz from the high speed clock. */
    ADC_CLOCK_1M,     /**< 1 MHz from the high speed clock. */
    ADC_CLOCK_500K,   /**< 500 KHz from the high speed clock. */
    ADC_CLOCK_250K,   /**< 250 KHz from the high speed clock. */
    ADC_CLOCK_125K,   /**< 125 KHz from the high speed clock. */
    ADC_CLOCK_62K5,   /**< 62.5 KHz from the high speed clock. */
    ADC_CLOCK_31K25,  /**< 31.25 KHz from the high speed clock. */
    ADC_CLOCK_32K,    /**< 32 KHz or 32.768 KHz from the low speed clock. */
} qn908x_adc_clock_t;

#ifdef DOXYGEN
/** @brief Selected ADC oversample clock.
 *
 * Define to one of the qn908x_adc_clock_t values.
 */
#define QN908X_ADC_CLOCK
#endif /* ifdef DOXYGEN */

/**
 * @brief   ADC channel pair configuration
 *
 * The following are the possible combinations of + and - inputs to the ADC
 * sigma delta. Some of these combinations reference the "Vinn" signal which can
 * be independently selected, see @ref qn908x_adc_vinn_t for details.
 *
 * The first signal is connected to the positive side while the second one is
 * connected to the negative side. For example, ADC_CHANNEL_ADC0_ADC1 will read
 * a positive value if ADC0 voltage is higher than ADC1.
 *
 * The @ref ADC_CHANNEL_TEMP uses the internal temperature signal and
 * @ref ADC_CHANNEL_VCC4_VINN connects the + side to Vcc/4, which is useful to
 * measure the battery level when Vcc is directly connected to a battery.
 */
typedef enum {
    ADC_CHANNEL_ADC0_ADC1 =  0u << 9u,   /**< Sample ADC0 / ADC1 */
    ADC_CHANNEL_ADC2_ADC3 =  1u << 9u,   /**< Sample ADC2 / ADC3 */
    ADC_CHANNEL_ADC4_ADC5 =  2u << 9u,   /**< Sample ADC4 / ADC5 */
    ADC_CHANNEL_ADC6_ADC7 =  3u << 9u,   /**< Sample ADC6 / ADC7 */
    ADC_CHANNEL_ADC0_VINN =  4u << 9u,   /**< Sample ADC0 / Vinn */
    ADC_CHANNEL_ADC1_VINN =  5u << 9u,   /**< Sample ADC1 / Vinn */
    ADC_CHANNEL_ADC2_VINN =  6u << 9u,   /**< Sample ADC2 / Vinn */
    ADC_CHANNEL_ADC3_VINN =  7u << 9u,   /**< Sample ADC3 / Vinn */
    ADC_CHANNEL_ADC4_VINN =  8u << 9u,   /**< Sample ADC4 / Vinn */
    ADC_CHANNEL_ADC5_VINN =  9u << 9u,   /**< Sample ADC5 / Vinn */
    ADC_CHANNEL_ADC6_VINN = 10u << 9u,   /**< Sample ADC6 / Vinn */
    ADC_CHANNEL_ADC7_VINN = 11u << 9u,   /**< Sample ADC7 / Vinn */
    ADC_CHANNEL_TEMP      = 13u << 9u,   /**< Sample internal temperature */
    ADC_CHANNEL_VCC4_VINN = 14u << 9u,   /**< Sample 1/4 Vcc / Vinn */
    ADC_CHANNEL_VINN_VINN = 15u << 9u,   /**< Sample Vinn / Vinn */
    ADC_CHANNEL_VINN_VSS  = 20u << 9u,   /**< Sample Vinn / Vss */
} qn908x_adc_channel_t;

/**
 * @brief   ADC Vref configuration
 *
 * This value affects the reference voltage used by the ADC as the full range.
 * It is also used in some cases to generate the Vinn signal are is only relevant for the channels that reference Vinn when it was
 * set by @ref qn908x_adc_vinn_t to use Vref. The actual values match the field
 * VREF_SEL in ADC CTRL register.
 */
typedef enum {
    ADC_VREF_1V2  = 0x0000u, /**< Vref := internal 1.2V. */
    ADC_VREF_VREF = 0x4000u, /**< Vref := external ADC_VREFI pin */
    ADC_VREF_VEXT = 0x8000u, /**< Vref := external ADC_VREFI with the driver */
    ADC_VREF_VCC  = 0xC000u, /**< Vref := Vcc */
} qn908x_adc_vref_t;

/**
 * @brief   ADC Vref x1.5 multiplier flag
 *
 * Note, this is the same value as ADC_CFG_VREF_GAIN_MASK. When enabled the
 * Vref voltage will be multiplied by 1.5x.
 */
#define ADC_VREF_GAIN_X15 (0x100u)

/**
 * @brief   ADC PGA Enabled flag
 *
 * Flag to enable the Programmable Gain Amplifier (PGA) with a gain of 1x. This
 * is only useful if the source signal doesn't have any driving capability since
 * the gain is set to 1x. The hardware supports other gain combinations but
 * those are not supported by the driver.
 *
 * Note: this value is defined as the inverse of ADC_CFG_PGA_BP_MASK which is
 * defined if the PGA is bypassed.
 */
#define ADC_PGA_ENABLE (0x08u)

/**
 * @brief   ADC Vinn configuration
 *
 * This value is only relevant for the channels that reference Vinn. The value
 * is the same as the PGA_VINN in ADC CFG register with a logic xor 0x30u to
 * make the default AVSS (analog Vss pad).
 */
typedef enum {
    ADC_VINN_VREF     = 0x30u, /**< Use Vinn := Vref */
    ADC_VINN_VREF_3_4 = 0x20u, /**< Use Vinn := 3/4 * Vref */
    ADC_VINN_VREF_2   = 0x10u, /**< Use Vinn := 1/2 * Vref */
    ADC_VINN_AVSS     = 0x00u, /**< Use Vinn := Vss */
} qn908x_adc_vinn_t;

/**
 * @brief   ADC SD Gain configuration
 *
 * This multiplies the sampled value (difference between +/- signals) by the
 * given value.
 *
 * Note: these values logic xor 0x40 match the values for ADC_CFG_ADC_GAIN
 * field. This is selected so that omitting this flag in the config field
 * defaults to x1.0 gain but it can still be converted to the ADC_GAIN field
 * with a simple logic xor.
 */
typedef enum {
    ADC_GAIN_X05 = 0x40u, /**< Use gain := 0.5 */
    ADC_GAIN_X1  = 0x00u, /**< Use gain := 1 */
    ADC_GAIN_X15 = 0xC0u, /**< Use gain := 1.5 */
    ADC_GAIN_X20 = 0x80u, /**< Use gain := 2 */
} qn908x_adc_gain_t;

/**
 * @brief   CPU specific ADC configuration
 *
 * ADC Channel, Vinn, Vref and gain configuration.
 *
 * This value should be set to the logic or between the following values:
 *  * bit 3: the optional flag @ref ADC_PGA_ENABLE,
 *  * bits 4-5: a @ref qn908x_adc_vinn_t value defining Vinn if needed,
 *  * bits 6-7: a @ref qn908x_adc_gain_t optional gain value,
 *  * bit 8: the optional flag @ref ADC_VREF_GAIN_X15,
 *  * bits 9-13: the selected @ref qn908x_adc_channel_t, and
 *  * bits 14-15: the @ref qn908x_adc_vref_t value defining Vref.
 *
 * The same channels with different settings can be configured as different ADC
 * lines in the board, just using different adc_conf_t entries.
 */
typedef uint16_t adc_conf_t;

/**
 * @brief   CPU specific timer Counter/Timers (CTIMER) configuration
 * @{
 */
#define TIMER_CHANNEL_NUMOF (4)
#define TIMER_MAX_VALUE     (0xffffffff)
/**
 * @brief   The nRF5x periph_timer implements timer_set()
 */
#define PERIPH_TIMER_PROVIDES_SET   1
/** @} */

/**
 * @brief   I2C bus speed values in kbit/s.
 *
 * @note    We support arbitrary speed values up to 400 kbit/s.
 *
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 10000u,   /**< low speed mode:          ~10 kbit/s */
    I2C_SPEED_NORMAL    = 100000u,  /**< normal mode:            ~100 kbit/s */
    I2C_SPEED_FAST      = 400000u,  /**< fast mode:              ~400 kbit/s */
    I2C_SPEED_FAST_PLUS = 400000u,  /**< not supported, capped at 400 kbit/s */
    I2C_SPEED_HIGH      = 400000u,  /**< not supported, capped at 400 kbit/s */
} i2c_speed_t;
/** @} */

/**
 * @brief   I2C configuration options
 */
typedef struct {
    I2C_Type *dev;          /**< hardware device */
    gpio_t pin_scl;         /**< SCL pin */
    gpio_t pin_sda;         /**< SDA pin */
    uint32_t speed;         /**< bus speed in bit/s */
} i2c_conf_t;

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

/**
 * @name    Define macros for sda and scl pins.
 * @{
 */
#define i2c_pin_sda(dev) i2c_config[dev].pin_sda
#define i2c_pin_scl(dev) i2c_config[dev].pin_scl
/** @} */

/**
 * @brief   Use some common SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Define a CPU specific SPI hardware chip select line macro
 *
 * GPIO numbers use the lower 5 bits and the bit 12. We define the CS numbers
 * to have the bit 15 set.
 */
#define SPI_HWCS(x)         (1u << 15u | (x))

/**
 * @brief   Number of HW CS pins supported
 */
#define SPI_HWCS_NUMOF 4

/**
 * @brief   SPI mode select helper macro
 *
 * The polarity is determined by the bits CPOL and CPHA in the SPI CFG register.
 */
#define SPI_MODE_SEL(pol, pha)          (SPI_CFG_CPOL(pol) | SPI_CFG_CPHA(pha))

/**
 * @name   Override the SPI mode bitmask
 *
 * Override the SPI mode value so we can use it directly as a bitmask to CFG.
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0  = SPI_MODE_SEL(0, 0),   /**< mode 0 */
    SPI_MODE_1  = SPI_MODE_SEL(0, 1),   /**< mode 1 */
    SPI_MODE_2  = SPI_MODE_SEL(1, 0),   /**< mode 2 */
    SPI_MODE_3  = SPI_MODE_SEL(1, 1)    /**< mode 3 */
} spi_mode_t;
/** @} */

/**
 * @name   Override SPI speed values
 *
 * The speed is configured at run time based on the AHB clock speed using an
 * arbitrary divider between /1 and /65536. The standard macro values just map
 * to the frequency in Hz. The maximum possible speed is 32 MHz assuming a
 * core clock and AHB bus clock of 32 MHz.
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ  =   100000u,    /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ  =   400000u,    /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ    =  1000000u,    /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ    =  5000000u,    /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ   = 10000000u     /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   SPI pin getters
 * @{
 */
#define spi_pin_mosi(bus) spi_config[bus].copi_pin
#define spi_pin_miso(bus) spi_config[bus].cipo_pin
#define spi_pin_clk(bus)  spi_config[bus].clk_pin
/** @} */

/**
 * @brief   SPI module configuration options
 */
typedef struct {
    SPI_Type *dev;                   /**< SPI device to use */
    gpio_t cipo_pin;                 /**< Controller Input Peripheral Output */
    gpio_t copi_pin;                 /**< Controller Output Peripheral Input */
    gpio_t clk_pin;                  /**< CLK pin */
    gpio_t cs_pin[SPI_HWCS_NUMOF];   /**< pins used for HW cs lines */
} spi_conf_t;

#endif /* ifndef DOXYGEN */

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

/** @} */
