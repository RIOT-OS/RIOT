/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32l476g-disco
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32L476G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim5

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @brief    ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32L476VG order. Instead, we
 * just define 5 ADC channels, for the next adjacent
 * 5 pins, from 10 to 14, in the header P1 and the
 * internal VBAT channel.
 *
 * To find appropriate device and channel find in the
 * board manual, table showing pin assignments and
 * information about ADC - a text similar to ADC[X]_IN[Y],
 * where:
 * [X] - describes used device - indexed from 0,
 * for example ADC12_IN10 is device 0 or device 1,
 * [Y] - describes used channel - indexed from 1,
 * for example ADC12_IN10 is channel 10
 *
 * For STM32L476VG this information is in MCU datasheet,
 * Table 16, page 73.
 *
 * VBAT is connected ADC1_IN18 or ADC3_IN18 and a voltage divider
 * is used, so that only 1/3 of the actual VBAT is measured. This
 * allows for a supply voltage higher than the reference voltage.
 *
 * For STM32L476VG more information is provided in MCU datasheet,
 * in section 3.15.3 - Vbat battery voltage monitoring, page 42.
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_A, 0), 0, 5},  /*< ADC12_IN5  */
    {GPIO_PIN(PORT_A, 5), 0, 10}, /*< ADC12_IN10 */
    {GPIO_PIN(PORT_A, 1), 0, 6},  /*< ADC12_IN6  */
    {GPIO_PIN(PORT_A, 2), 0, 7},  /*< ADC12_IN7  */
    {GPIO_PIN(PORT_A, 3), 0, 8},  /*< ADC12_IN8  */
    {GPIO_UNDEF, 0, 18}, /* VBAT */
};

/**
 * @brief VBAT ADC line
 */
#define VBAT_ADC            ADC_LINE(5)

/**
 * @brief Number of ADC devices
 */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {{}};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
