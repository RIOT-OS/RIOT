/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_p-nucleo-wb55
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the p-nucleo-wb55 board
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            1
#endif

#ifndef CONFIG_CLOCK_HSE
#define CONFIG_CLOCK_HSE                       MHZ(32)
#endif

/* EXTAHB (HCLK2) max freq 32 Mhz*/
#define CLOCK_EXTAHB_DIV    RCC_EXTCFGR_C2HPRE_3
#define CLOCK_EXTAHB        (CLOCK_CORECLOCK / 2)

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    {
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0, /* Use APB clock */
    },
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_lpuart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * To find appriopate device and channel find in the MCU datasheet table
 * concerning "Alternate function AF0 to AF7" a text similar to TIM[X]_CH[Y],
 * where:
 * TIM[X] - is device,
 * [Y] - describes used channel (indexed from 0), for example TIM2_CH1 is
 * channel 0 in configuration structure (cc_chan - field),
 * Port column in the table describes connected port.
 *
 * For Nucleo-WB55 this information is in the datasheet, Table 18, page 72.
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A,  8) /*  CN9 D6 */, .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_A,  9) /*  CN5 D9 */, .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_A, 10) /*  CN9 D3 */, .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_A, 11) /* CC10 14 */, .cc_chan = 3} },
        .af       = GPIO_AF1,
        .bus      = APB2
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)

/** @} */

/**
 * @brief USB device FS configuration
 */
static const stm32_usbdev_fs_config_t stm32_usbdev_fs_config[] = {
    {
        .base_addr  = (uintptr_t)USB,
        .rcc_mask   = RCC_APB1ENR1_USBEN | RCC_APB1ENR1_CRSEN,
        .irqn       = USB_LP_IRQn,
        .apb        = APB1,
        .dm         = GPIO_PIN(PORT_A, 11),
        .dp         = GPIO_PIN(PORT_A, 12),
        .af         = GPIO_AF10,
        .disconn    = GPIO_UNDEF,
    },
};

/**
 * @brief Interrupt function name mapping
 */
#define USBDEV_ISR              isr_usb_lp

/**
 * @brief Number of available USB device FS peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(stm32_usbdev_fs_config)

/**
 * @brief    ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32WB55RG order. Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5 and the internal VBAT channel.
 *
 * To find appropriate device and channel find in the
 * board manual, table showing pin assignments and
 * information about ADC - a text similar to ADC[X]_IN[Y],
 * where:
 * [X] - describes used device - indexed from 0,
 * for example ADC1_IN10 is device 0,
 * [Y] - describes used channel - indexed from 1,
 * for example ADC1_IN10 is channel 10
 *
 * For P-NUCLEO-WB55 this information is in board manual,
 * Table 10, page 39.
 *
 * VBAT is connected ADC1_IN18 or ADC3_IN18 and a voltage divider
 * is used, so that only 1/3 of the actual VBAT is measured. This
 * allows for a supply voltage higher than the reference voltage.
 *
 * For P-NUCLEO-WB55 more information is provided in the Reference Manual,
 * in section 16.4.31 - Vbat supply monitoring, page 475.
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_C, 0), 0, 1},  /*< ADC1_IN1 */
    {GPIO_PIN(PORT_C, 1), 0, 2},  /*< ADC1_IN2 */
    {GPIO_PIN(PORT_A, 1), 0, 6},  /*< ADC1_IN6 */
    {GPIO_PIN(PORT_A, 0), 0, 5},  /*< ADC1_IN5 */
    {GPIO_PIN(PORT_C, 3), 0, 4},  /*< ADC1_IN4 */
    {GPIO_PIN(PORT_C, 2), 0, 3},  /*< ADC1_IN3 */
    {GPIO_UNDEF, 0, 18}, /* VBAT */
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
