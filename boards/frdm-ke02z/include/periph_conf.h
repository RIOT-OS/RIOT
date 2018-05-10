/*
 * Copyright (C) 2018 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-ke02z
 * @{
 *
 * @file
 * @name        MCU peripheral configuration for the FRDM-KE02Z
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

/*
 * The system clock is initialised in FEI mode providing 20MHz and
 * 20MHz as peripheral bus clock.
 *
 * Currently there is no support for runtime clock mode and frequency
 * modification.
 */
#define CLOCK_CORECLOCK (20000000ul)
#define CLOCK_BUSCLOCK  (CLOCK_CORECLOCK)

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @name RGB LED GPIO configuration
* @{
*/
#define LED_RGB_RED_PIN   GPIO_PIN(PORT_H, 1)
#define LED_RGB_GREEN_PIN GPIO_PIN(PORT_H, 2)
#define LED_RGB_BLUE_PIN  GPIO_PIN(PORT_E, 7)

/* Used in LED test */
#define LED0_MASK   (1 << 25)
#define LED0_ON     (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF    (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE (GPIOB->PTOR = LED0_MASK)

#define LED1_MASK   (1 << 26)
#define LED1_ON     (GPIOB->PCOR = LED1_MASK)
#define LED1_OFF    (GPIOB->PSOR = LED1_MASK)
#define LED1_TOGGLE (GPIOB->PTOR = LED1_MASK)

#define LED2_MASK   (1 << 7)
#define LED2_ON     (GPIOB->PCOR = LED2_MASK)
#define LED2_OFF    (GPIOB->PSOR = LED2_MASK)
#define LED2_TOGGLE (GPIOB->PTOR = LED2_MASK)

/**
* @name UART configuration
* @{
*/
static const uart_conf_t uart_config[] = {
    {
        .dev       = UART0,
        .freq      = CLOCK_BUSCLOCK,
        .pin_rx    = GPIO_UNDEF,
        .pin_tx    = GPIO_UNDEF,
        .pcr_rx    = GPIO_UNDEF,
        .pcr_tx    = GPIO_UNDEF,
        .irqn      = UART0_RX_TX_IRQn,
        .scgc_addr = &(SIM->SCGC),
        .scgc_bit  = SIM_SCGC_UART0_SHIFT,
        .mode      = UART_MODE_8N1,
        .type      = KINETIS_UART
    },
    {
        .dev       = UART1,
        .freq      = CLOCK_BUSCLOCK,
        .pin_rx    = GPIO_UNDEF,
        .pin_tx    = GPIO_UNDEF,
        .pcr_rx    = GPIO_UNDEF,
        .pcr_tx    = GPIO_UNDEF,
        .irqn      = UART1_RX_TX_IRQn,
        .scgc_addr = &(SIM->SCGC),
        .scgc_bit  = SIM_SCGC_UART1_SHIFT,
        .mode      = UART_MODE_8N1,
        .type      = KINETIS_UART
    },
    {
        .dev       = UART2,
        .freq      = CLOCK_BUSCLOCK,
        .pin_rx    = GPIO_UNDEF,
        .pin_tx    = GPIO_UNDEF,
        .pcr_rx    = GPIO_UNDEF,
        .pcr_tx    = GPIO_UNDEF,
        .irqn      = UART2_RX_TX_IRQn,
        .scgc_addr = &(SIM->SCGC),
        .scgc_bit  = SIM_SCGC_UART2_SHIFT,
        .mode      = UART_MODE_8N1,
        .type      = KINETIS_UART
    },
};

#define UART_0_ISR (isr_uart0_rx_tx)
#define UART_1_ISR (isr_uart1_rx_tx)
#define UART_2_ISR (isr_uart2_rx_tx)
#define UART_NUMOF (sizeof(uart_config) / sizeof(uart_config[0]))

/**
 * @name   SPI configuration
 *
 * Clock configuration values based on the configured 20MHz module clock.
 *
 * Divisor = (SPPR + 1) * 2^(SPR + 1)
 *
 * Baudrate = Clock / Divisor
 *
 * @{
 */
static const uint32_t spi_clk_config[] = {
    (
        SPI_BR_SPPR(5) | SPI_BR_SPR(4) /* -> 100kHz */
    ),
    (
        SPI_BR_SPPR(5) | SPI_BR_SPR(2) /* -> 400kHz */
    ),
    (
        SPI_BR_SPPR(1) | SPI_BR_SPR(2) /* -> 1MHz */
    ),
    (
        SPI_BR_SPPR(1) | SPI_BR_SPR(0) /* -> 5MHz */
    ),
    (
        SPI_BR_SPPR(0) | SPI_BR_SPR(0) /* -> 10MHz */
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_UNDEF,
        .pin_mosi = GPIO_UNDEF,
        .pin_clk  = GPIO_UNDEF,
        .pin_cs   = {
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = (gpio_pcr_t)-1,
        .simmask  = SIM_SCGC_SPI0_MASK
    },
    {
        .dev      = SPI1,
        .pin_miso = GPIO_UNDEF,
        .pin_mosi = GPIO_UNDEF,
        .pin_clk  = GPIO_UNDEF,
        .pin_cs   = {
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = (gpio_pcr_t)-1,
        .simmask  = SIM_SCGC_SPI1_MASK
    }
};

#define SPI_NUMOF (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
* @name I2C configuration
* @{
*/
#define I2C_NUMOF                    (1U)
#define I2C_0_EN                     1
/* Low (10 kHz): MUL = 2, SCL divider = 1536, total: 3072 */
#define KINETIS_I2C_F_ICR_LOW        (0x36)
#define KINETIS_I2C_F_MULT_LOW       (1)
/* Normal (100 kHz): MUL = 2, SCL divider = 160, total: 320 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1D)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 80, total: 80 */
#define KINETIS_I2C_F_ICR_FAST       (0x14)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 30, total: 30 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x05)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)

/* I2C 0 device configuration */
#define I2C_0_DEV                    I2C0
#define I2C_0_CLKEN()                (SIM->SCGC |= (SIM_SCGC_IIC_MASK))
#define I2C_0_CLKDIS()               (SIM->SCGC &= ~(SIM_SCGC_IIC_MASK))
#define I2C_0_IRQ                    I2C0_IRQn
#define I2C_0_IRQ_HANDLER            isr_i2c0
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF (1U)
#define PIT_CONFIG {       \
    {                      \
        .prescaler_ch = 0, \
        .count_ch = 1,     \
    }                      \
}

#define PIT_BASECLOCK (CLOCK_BUSCLOCK)
#define PIT_ISR_0     isr_pit1
#define PIT_ISR_1     isr_pit2
#define LPTMR_NUMOF   (0)
#define TIMER_NUMOF   (PIT_NUMOF + LPTMR_NUMOF)

/**
* @name RTT and RTC configuration
* @{
*/
#define RTT_NUMOF     (1U)
#define RTC_NUMOF     (1U)
#define RTT_DEV       RTC
#define RTT_IRQ       RTC_IRQn
#define RTT_IRQ_PRIO  10
#define RTT_UNLOCK()  (SIM->SCGC |= (SIM_SCGC_RTC_MASK))
#define RTT_LOCK()    (SIM->SCGC &= ~(SIM_SCGC_RTC_MASK))
#define RTT_ISR       isr_rtc
#define RTT_FREQUENCY (10) /* Tick every 100ms */
#define RTT_MAX_VALUE (0x0000ffff) /* 16-bit timer */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
