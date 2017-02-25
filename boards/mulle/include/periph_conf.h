/*
 * Copyright (C) 2015 Eistec AB
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */


/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the Eistec Mulle
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MULLE_PERIPH_CONF_H
#define MULLE_PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define KINETIS_CPU_USE_MCG               1

#define KINETIS_MCG_USE_ERC               1
#define KINETIS_MCG_USE_PLL               0
#define KINETIS_MCG_DCO_RANGE             (96000000U)
#define KINETIS_MCG_ERC_OSCILLATOR        0
#define KINETIS_MCG_ERC_FRDIV             0
#define KINETIS_MCG_ERC_RANGE             0
#define KINETIS_MCG_ERC_FREQ              (32768U)

/* Base clocks, used by SystemCoreClockUpdate */
/** Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL_CLK_HZ                 8000000u
/** Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              32768u
/** Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_SLOW_CLK_HZ             32768u
/** Value of the fast internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             4000000u
/** Default System clock value */
#define DEFAULT_SYSTEM_CLOCK            (CPU_XTAL32k_CLK_HZ * 2929u)

/* bus clock for the peripherals */
#define CLOCK_BUSCLOCK                  (DEFAULT_SYSTEM_CLOCK / 2)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (2U)
#define PIT_CONFIG {                 \
        {                            \
            .prescaler_ch = 0,       \
            .count_ch = 1,           \
        },                           \
        {                            \
            .prescaler_ch = 2,       \
            .count_ch = 3,           \
        },                           \
    }
#define LPTMR_NUMOF             (1U)
#define LPTMR_CONFIG { \
        { \
            .dev = LPTMR0, \
            .clk_gate = (uint32_t volatile *)BITBAND_REGADDR(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT), \
            .index = 0, \
        } \
    }
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))

#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define PIT_CLOCKGATE           (BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_PIT_SHIFT))
#define PIT_ISR_0               isr_pit1
#define PIT_ISR_1               isr_pit3
#define LPTMR_ISR_0             isr_lptmr0

/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_4_EN           0
#define UART_IRQ_PRIO       CPU_DEFAULT_IRQ_PRIO

/* UART 0 device configuration */
#define UART_0_DEV          UART1
#define UART_0_CLKEN()      (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_UART1_SHIFT) = 1)
#define UART_0_CLKDIS()     (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_UART1_SHIFT) = 0)
#define UART_0_CLK          (SystemSysClock)
#define UART_0_IRQ_CHAN     UART1_RX_TX_IRQn
#define UART_0_ISR          isr_uart1_status
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define UART_0_PORT         PORTC
#define UART_0_TX_PIN       4
#define UART_0_RX_PIN       3
/* Function number in pin multiplex, see K60 Sub-Family Reference Manual,
 * section 10.3.1 K60 Signal Multiplexing and Pin Assignments */
#define UART_0_AF           3
#define UART_0_TX_PCR_MUX   3
#define UART_0_RX_PCR_MUX   3

/* UART 1 device configuration */
#define UART_1_DEV          UART0
#define UART_1_CLKEN()      (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_UART0_SHIFT) = 1)
#define UART_1_CLKDIS()     (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_UART0_SHIFT) = 0)
#define UART_1_CLK          (SystemSysClock)
#define UART_1_IRQ_CHAN     UART0_RX_TX_IRQn
#define UART_1_ISR          isr_uart0_status
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTA_SHIFT) = 1)
#define UART_1_PORT         PORTA
#define UART_1_TX_PIN       14
#define UART_1_RX_PIN       15
/* Function number in pin multiplex, see K60 Sub-Family Reference Manual,
 * section 10.3.1 K60 Signal Multiplexing and Pin Assignments */
#define UART_1_AF           3
#define UART_1_TX_PCR_MUX   3
#define UART_1_RX_PCR_MUX   3

/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* dev, pin, channel */
    [ 0] = { ADC1, GPIO_UNDEF, 26 },       /* internal: temperature sensor */
    [ 1] = { ADC1, GPIO_UNDEF, 27 },       /* internal: band gap */
    [ 2] = { ADC1, GPIO_UNDEF, 29 },       /* internal: V_REFSH */
    [ 3] = { ADC1, GPIO_UNDEF, 30 },       /* internal: V_REFSL */
    [ 4] = { ADC1, GPIO_UNDEF, 23 },       /* internal: DAC0 module output level */
    [ 5] = { ADC1, GPIO_UNDEF, 18 },       /* internal: VREF module output level */
    [ 6] = { ADC1, GPIO_UNDEF,  0 },       /* on board connection to Mulle Vbat/2 on PGA1_DP pin */
    [ 7] = { ADC1, GPIO_UNDEF, 19 },       /* on board connection to Mulle Vchr/2 on PGA1_DM pin */
    [ 8] = { ADC0, GPIO_UNDEF,  0 },       /* expansion port PGA0_DP pin */
    [ 9] = { ADC0, GPIO_UNDEF, 19 },       /* expansion port PGA0_DM pin */
    [10] = { ADC1, GPIO_PIN(PORT_A, 17), 17 }, /* expansion port PTA17 */
    [11] = { ADC1, GPIO_PIN(PORT_B,  0),  8 }, /* expansion port PTB0  */
    [12] = { ADC0, GPIO_PIN(PORT_C,  0), 14 }, /* expansion port PTC0  */
    [13] = { ADC1, GPIO_PIN(PORT_C,  8),  4 }, /* expansion port PTC8  */
    [14] = { ADC1, GPIO_PIN(PORT_C,  9),  5 }, /* expansion port PTC9  */
    [15] = { ADC1, GPIO_PIN(PORT_C, 10),  6 }, /* expansion port PTC10 */
    [16] = { ADC1, GPIO_PIN(PORT_C, 11),  7 }, /* expansion port PTC11 */
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/** @} */

/**
 * @name DAC configuration
 * @{
 */

#define DAC_CONFIG { \
    { DAC0, (uint32_t volatile *)BITBAND_REGADDR(SIM->SCGC2, SIM_SCGC2_DAC0_SHIFT) }, \
  }
#define DAC_NUMOF 1

/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .ftm        = FTM0,
        .chan       = {
            { .pin = GPIO_PIN(PORT_C, 1), .af = 4, .ftm_chan = 0 },
            { .pin = GPIO_PIN(PORT_C, 2), .af = 4, .ftm_chan = 1 },
            { .pin = GPIO_UNDEF,          .af = 0, .ftm_chan = 0 },
            { .pin = GPIO_UNDEF,          .af = 0, .ftm_chan = 0 }
        },
        .chan_numof = 2,
        .ftm_num    = 0
    },
    {
        .ftm        = FTM1,
        .chan       = {
            { .pin = GPIO_PIN(PORT_A, 12), .af = 3, .ftm_chan = 0 },
            { .pin = GPIO_PIN(PORT_A, 13), .af = 3, .ftm_chan = 1 },
            { .pin = GPIO_UNDEF,           .af = 0, .ftm_chan = 0 },
            { .pin = GPIO_UNDEF,           .af = 0, .ftm_chan = 0 }
        },
        .chan_numof = 2,
        .ftm_num    = 1
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @name SPI configuration
 *
 * Clock configuration values based on the configured 47988736Hz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis_common/dist/calc_spi_scalers/calc_spi_scalers.c
 *
 * @{
 */
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(8) |          /* -> 93728Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(8) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(8) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(8)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(6) |          /* -> 374912Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(6) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(6) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(6)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(4) |          /* -> 999765Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) |          /* -> 4798873Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(0)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) |          /* -> 7998122Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(0)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_D, 3),
        .pin_mosi = GPIO_PIN(PORT_D, 2),
        .pin_clk  = GPIO_PIN(PORT_D, 1),
        .pin_cs   = {
            GPIO_PIN(PORT_D, 0),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI0_MASK
    },
    {
        .dev      = SPI1,
        .pin_miso = GPIO_PIN(PORT_E, 3),
        .pin_mosi = GPIO_PIN(PORT_E, 1),
        .pin_clk  = GPIO_PIN(PORT_E, 2),
        .pin_cs   = {
            GPIO_PIN(PORT_E, 4),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI1_MASK
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C baud rate configuration
 * @{
 */
/* Low (10 kHz): MUL = 4, SCL divider = 2560, total: 10240 */
#define KINETIS_I2C_F_ICR_LOW        (0x3D)
#define KINETIS_I2C_F_MULT_LOW       (2)
/* Normal (100 kHz): MUL = 2, SCL divider = 240, total: 480 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1F)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 128, total: 128 */
#define KINETIS_I2C_F_ICR_FAST       (0x17)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 48, total: 48 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x10)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF               (1U)
#define I2C_CLK                 SystemBusClock
#define I2C_0_EN                1
#define I2C_1_EN                0
#define I2C_IRQ_PRIO            CPU_DEFAULT_IRQ_PRIO

/* I2C 0 device configuration */
#define I2C_0_DEV               I2C0
#define I2C_0_CLKEN()           (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT) = 1)
#define I2C_0_CLKDIS()          (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT) = 0)
#define I2C_0_IRQ               I2C0_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT              PORTB
#define I2C_0_PORT_CLKEN()      (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define I2C_0_PIN_AF            2
#define I2C_0_SDA_PIN           1
#define I2C_0_SCL_PIN           2
#define I2C_0_PORT_CFG          (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_IRQ_PRIO       CPU_DEFAULT_IRQ_PRIO
/** @} */


/**
 * @name RTC configuration
 * @{
 */
/* RIOT RTC implementation uses RTT for underlying timekeeper */
#define RTC_NUMOF           (1U)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_IRQ_PRIO        CPU_DEFAULT_IRQ_PRIO
#define RTT_IRQ             RTC_IRQn
#define RTT_ISR             isr_rtc_alarm
#define RTT_DEV             RTC
#define RTT_UNLOCK()        (BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_RTC_SHIFT) = 1)
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (1)             /* in Hz */

/**
 * RTC module crystal load capacitance configuration bits.
 */
/* The crystal on the Mulle is designed for 12.5 pF load capacitance. According
 * to the data sheet, the K60 will have a 5 pF parasitic capacitance on the
 * XTAL32/EXTAL32 connection. The board traces might give some minor parasitic
 * capacitance as well. */
/* enable 6pF load capacitance, might need adjusting.. */
#define RTT_LOAD_CAP_BITS   (RTC_CR_SC4P_MASK | RTC_CR_SC2P_MASK | RTC_CR_SC1P_MASK)
/** @} */


/**
 * @name Random Number Generator configuration
 * @{
 */
#define HWRNG_CLKEN()       (BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT) = 1)
#define HWRNG_CLKDIS()      (BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT) = 0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MULLE_PERIPH_CONF_H */
/** @} */
