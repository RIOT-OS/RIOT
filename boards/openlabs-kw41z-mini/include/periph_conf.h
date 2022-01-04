/*
 * Copyright (C) 2017 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openlabs-kw41z-mini
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for openlabs-kw41z-mini
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running with the internal clock
     * with the following clock frequencies:
     * Core:  48 MHz
     * Bus:   24 MHz
     * Flash: 24 MHz
     */

    .clkdiv1            = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1),
    .rtc_clc            = RTC_CR_SC16P_MASK | RTC_CR_SC4P_MASK | RTC_CR_SC2P_MASK,

    /* Use the 32 kHz oscillator as ERCLK32K. Note that the values here have a
     * different mapping for the KW41Z than the values used in the Kinetis
     * K series */
    .osc32ksel          = SIM_SOPT1_OSC32KSEL(0),

    /* enable clocks */
    .clock_flags =
        KINETIS_CLOCK_OSC0_EN | /* Enable RSIM oscillator */
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        KINETIS_CLOCK_MCGIRCLK_EN | /* Used for LPUART clocking */
        KINETIS_CLOCK_MCGIRCLK_STOP_EN |
        0,

    /* Using FEI mode by default, the external crystal settings below are only
     * used if mode is changed to an external mode (PEE, FBE, or FEE) */
    .default_mode       = KINETIS_MCG_MODE_FEI,

    /* The crystal connected to RSIM OSC is 32 MHz */
    .erc_range          = KINETIS_MCG_ERC_RANGE_VERY_HIGH,

    .osc_clc            = 0, /* not used by kw41z */
    .oscsel             = MCG_C7_OSCSEL(0), /* Use RSIM for external clock */
    .fcrdiv             = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */

    .fll_frdiv          = MCG_C1_FRDIV(0b101), /* Divide by 1024 */
    .fll_factor_fei     = KINETIS_MCG_FLL_FACTOR_1464, /* FEI FLL freq = 48 MHz */
    .fll_factor_fee     = KINETIS_MCG_FLL_FACTOR_1280, /* FEE FLL freq = 40 MHz */
};
/* Radio xtal frequency, either 32 MHz or 26 MHz */
#define CLOCK_RADIOXTAL              (32000000ul)
/* CPU core clock, the MCG clock output frequency */
#define CLOCK_CORECLOCK              (48000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 2)
#define CLOCK_MCGFLLCLK              (CLOCK_CORECLOCK)
#define CLOCK_OSCERCLK               (CLOCK_RADIOXTAL)
#define CLOCK_MCGIRCLK               (4000000ul)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (1U)
#define PIT_CONFIG {                 \
        {                            \
            .prescaler_ch = 0,       \
            .count_ch = 1,           \
        },                           \
    }
#define LPTMR_NUMOF             (1U)
#define LPTMR_CONFIG {               \
        {                            \
            .dev = LPTMR0,           \
            .base_freq = 32768u,     \
            .src = 2,                \
            .irqn = LPTMR0_IRQn,     \
        },                           \
    }
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))
#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define LPTMR_ISR_0             isr_lptmr0
/** @} */

/**
 * @name UART configuration
 * @{
 */
#ifndef LPUART_0_SRC
#define LPUART_0_SRC                1
#endif

#if (LPUART_0_SRC == 3)
/* Use MCGIRCLK (4 MHz internal reference - not available in KINETIS_PM_LLS) */
#define LPUART_0_CLOCK                  CLOCK_MCGIRCLK
#define UART_CLOCK_PM_BLOCKER           KINETIS_PM_LLS
#define UART_MAX_UNCLOCKED_BAUDRATE     19200ul
#elif (LPUART_0_SRC == 2)
#define LPUART_0_CLOCK                  CLOCK_OSCERCLK
#elif (LPUART_0_SRC == 1)
/* Use CLOCK_MCGFLLCLK (48 MHz FLL output - not available in KINETIS_PM_STOP) */
#define LPUART_0_CLOCK                  CLOCK_MCGFLLCLK
#define UART_CLOCK_PM_BLOCKER           KINETIS_PM_STOP
#define UART_MAX_UNCLOCKED_BAUDRATE     57600ul
#endif

static const uart_conf_t uart_config[] = {
    {
        .dev    = LPUART0,
        .freq   = LPUART_0_CLOCK,
        .pin_rx = GPIO_PIN(PORT_C,  6),
        .pin_tx = GPIO_PIN(PORT_C,  7),
        .pcr_rx = PORT_PCR_MUX(4) | GPIO_IN_PU,
        .pcr_tx = PORT_PCR_MUX(4),
        .irqn   = LPUART0_IRQn,
        .scgc_addr = &SIM->SCGC5,
        .scgc_bit = SIM_SCGC5_LPUART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_LPUART,
#ifdef MODULE_PERIPH_LLWU /* TODO remove ifdef after #11789 is merged */
        .llwu_rx = LLWU_WAKEUP_PIN_PTC6,
#endif
    },
};
#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define LPUART_0_ISR        isr_lpuart0
/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* ADC0_SE1 A0 */
    [0] = { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  1),  .chan = 1, .avg = ADC_AVG_MAX },
    /* ADC0_SE2 A1 */
    [1] = { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  2),  .chan = 3, .avg = ADC_AVG_MAX },
    /* ADC0_SE3 A2 */
    [2] = { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  3),  .chan = 2, .avg = ADC_AVG_MAX },
    /* ADC0_SE4 A3 */
    [3] = { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  18), .chan = 4, .avg = ADC_AVG_MAX },

    /* internal: temperature sensor */
    /* The temperature sensor has a very high output impedance, it must not be
     * sampled using hardware averaging, or the sampled values will be garbage */
    [4] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 26, .avg = ADC_AVG_NONE },
    /* internal: band gap */
    /* Note: the band gap buffer uses a bit of current and is turned off
     * by default,
     * Set PMC->REGSC |= PMC_REGSC_BGBE_MASK before reading or the input will
     * be floating */
    [5] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 27, .avg = ADC_AVG_MAX },
    /* internal: DCDC divided battery level */
    [6] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 23, .avg = ADC_AVG_MAX },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/*
 * KW41Z ADC reference settings:
 * 0: VREFH external pin or VREF_OUT 1.2 V signal (if VREF module is enabled)
 * 1: VDDA (analog supply input voltage)
 * 2-3: reserved
 */
#define ADC_REF_SETTING     1
#if ADC_REF_SETTING
#define ADC_REF_VOLTAGE     (3.3f)
#else
#define ADC_REF_VOLTAGE     (1.2f)
#endif

#define ADC_TEMPERATURE_CHANNEL     (4)
/** @} */

/**
 * @name   DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    {
        /* PTB18 | ADC0_SE4 | A3 */
        .dev       = DAC0,
        .scgc_addr = &SIM->SCGC6,
        .scgc_bit  = SIM_SCGC6_DAC0_SHIFT,
    },
};

#define DAC_NUMOF           ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name   PWM mode configuration
 * @{
 */
#define HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT   = (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK),  /**< left aligned */
    PWM_RIGHT  = (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK),  /**< right aligned */
    PWM_CENTER = (TPM_CnSC_MSB_MASK)                        /**< center aligned */
} pwm_mode_t;

/**
 * @brief   PWM configuration structure
 */
#define PWM_CHAN_MAX        (4U)
typedef struct {
    TPM_Type *tpm;          /**< used TPM */
    struct {
        gpio_t pin;         /**< GPIO pin used, set to GPIO_UNDEF */
        uint8_t af;         /**< alternate function mapping */
        uint8_t ftm_chan;   /**< the actual FTM channel used */
    } chan[PWM_CHAN_MAX];   /**< logical channel configuration */
    uint8_t chan_numof;     /**< number of actually configured channels */
    uint8_t tpm_num;        /**< FTM number used */
} pwm_conf_t;

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .tpm        = TPM0,
        .chan       = {
            { .pin = GPIO_PIN(PORT_B, 0), .af = 5, .ftm_chan = 1 }
        },
        .chan_numof = 1,
        .tpm_num    = 0
    },
    {
        .tpm        = TPM1,
        .chan       = {
            { .pin = GPIO_PIN(PORT_C, 4), .af = 5, .ftm_chan = 0 }
        },
        .chan_numof = 1,
        .tpm_num    = 1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   SPI configuration
 *
 * Clock configuration values based on the configured 16Mhz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis/dist/calc_spi_scalers/calc_spi_scalers.c
 *
 * @{
 */
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(5) |          /* -> 100000Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(4) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(4) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(4)
    ),
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(3) |          /* -> 400000Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(2) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(2) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(2)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) |          /* -> 1000000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) |          /* -> 4000000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(1) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(1) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(1)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) |          /* -> 4000000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(0)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_C, 18),
        .pin_mosi = GPIO_PIN(PORT_C, 17),
        .pin_clk  = GPIO_PIN(PORT_C, 16),
        .pin_cs   = {
            GPIO_PIN(PORT_C, 19),
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
        },
        .pcr      = (gpio_pcr_t)(GPIO_AF_2 | GPIO_IN_PU),
        .simmask  = SIM_SCGC6_SPI0_MASK
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .i2c = I2C1,
        .scl_pin = GPIO_PIN(PORT_C, 2),
        .sda_pin = GPIO_PIN(PORT_C, 3),
        .freq = CLOCK_CORECLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C1_IRQn,
        .scl_pcr = (PORT_PCR_MUX(3)),
        .sda_pcr = (PORT_PCR_MUX(3)),
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           (isr_i2c1)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define KINETIS_TRNG                TRNG
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
