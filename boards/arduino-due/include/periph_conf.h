/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-due
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Arduino Due board
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock configuration
 * @{
 */
/* targeted system core clock */
#define CLOCK_CORECLOCK     (84000000UL)
/* external oscillator clock */
#define CLOCK_EXT_OSC       (12000000UL)
/* define PLL configuration
 *
 * The values must fulfill this equation:
 * CORECLOCK = (EXT_OCS / PLL_DIV) * (PLL_MUL + 1)
 */
#define CLOCK_PLL_MUL       (83)
#define CLOCK_PLL_DIV       (12)

/* number of wait states before flash read and write operations */
#define CLOCK_FWS           (4)         /* 4 is save for 84MHz */
/** @} */

/**
 * @name Timer peripheral configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    /* dev, channel 0 ID */
    { TC0, ID_TC0 },
    { TC1, ID_TC3 },
};

#define TIMER_0_ISR         isr_tc0
#define TIMER_1_ISR         isr_tc3

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    /* device, rx port, tx port, rx pin, tx pin, mux, PMC bit, IRGn line */
    {(Uart *)UART,   PIOA, PIOA,  8,  9, GPIO_MUX_A, ID_UART,   UART_IRQn},
    {(Uart *)USART0, PIOA, PIOA, 10, 11, GPIO_MUX_A, ID_USART0, USART0_IRQn},
    {(Uart *)USART1, PIOA, PIOA, 12, 13, GPIO_MUX_A, ID_USART1, USART1_IRQn},
    {(Uart *)USART3, PIOD, PIOD,  4,  5, GPIO_MUX_B, ID_USART3, USART3_IRQn}
};

/* define interrupt vectors */
#define UART_0_ISR          isr_uart
#define UART_1_ISR          isr_usart0
#define UART_2_ISR          isr_usart1
#define UART_3_ISR          isr_usart3

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
* @name SPI configuration
* @{
*/
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1

/* SPI 0 device config */
#define SPI_0_DEV           SPI0
#define SPI_0_CLKEN()       (PMC->PMC_PCER0 |= (1 << ID_SPI0));
#define SPI_0_CLKDIS()      (PMC->PMC_PCER0 &= ~(1 << ID_SPI0));
#define SPI_0_IRQ           SPI0_IRQn
#define SPI_0_IRQ_HANDLER   isr_spi0
#define SPI_0_IRQ_PRIO      1

/* SPI 0 pin configuration */
#define SPI_0_MISO_PIN      PIO_PA25A_SPI0_MISO
#define SPI_0_MOSI_PIN      PIO_PA26A_SPI0_MOSI
#define SPI_0_SCK_PIN       PIO_PA27A_SPI0_SPCK
#define SPI_0_MISO_PORT     PIOA
#define SPI_0_MOSI_PORT     PIOA
#define SPI_0_SCK_PORT      PIOA
#define SPI_0_MISO_PORT_CLKEN()  (PMC->PMC_PCER0 |= (1 << ID_PIOA));
#define SPI_0_MOSI_PORT_CLKEN()  (PMC->PMC_PCER0 |= (1 << ID_PIOA));
#define SPI_0_SCK_PORT_CLKEN()   (PMC->PMC_PCER0 |= (1 << ID_PIOA));
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (1U)
#define PWM_0_EN            (1)
#define PWM_MAX_VALUE       (0xffff)
#define PWM_MAX_CHANNELS    (4U)

/* PWM_0 configuration */
#define PWM_0_DEV           PWM
#define PWM_0_PID           ID_PWM
#define PWM_0_CHANNELS      (4U)
#define PWM_0_DEV_CH0       (&(PWM_0_DEV->PWM_CH_NUM[4]))
#define PWM_0_ENA_CH0       PWM_ENA_CHID4
#define PWM_0_PORT_CH0      PIOC
#define PWM_0_PIN_CH0       PIO_PC21B_PWML4
#define PWM_0_DEV_CH1       (&(PWM_0_DEV->PWM_CH_NUM[5]))
#define PWM_0_ENA_CH1       PWM_ENA_CHID5
#define PWM_0_PORT_CH1      PIOC
#define PWM_0_PIN_CH1       PIO_PC22B_PWML5
#define PWM_0_DEV_CH2       (&(PWM_0_DEV->PWM_CH_NUM[6]))
#define PWM_0_ENA_CH2       PWM_ENA_CHID6
#define PWM_0_PORT_CH2      PIOC
#define PWM_0_PIN_CH2       PIO_PC23B_PWML6
#define PWM_0_DEV_CH3       (&(PWM_0_DEV->PWM_CH_NUM[7]))
#define PWM_0_ENA_CH3       PWM_ENA_CHID7
#define PWM_0_PORT_CH3      PIOC
#define PWM_0_PIN_CH3       PIO_PC24B_PWML7
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
