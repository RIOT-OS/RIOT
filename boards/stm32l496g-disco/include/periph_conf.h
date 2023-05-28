/*
 * Copyright (C) 2018 Inria
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32l496g-disco
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32L496G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
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
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 1 },    /* DMA1 Channel 2 - SPI1_RX */
    { .stream = 2 },    /* DMA1 Channel 3 - SPI1_TX */
    { .stream = 3 },    /* DMA1 Channel 4 - SPI2_RX / USART1_TX */
    { .stream = 4 },    /* DMA1 Channel 5 - SPI2_TX */
    { .stream = 6 },    /* DMA1 Channel 7 - USART2_TX */
    { .stream = 13 },   /* DMA2 Channel 6 - LPUART1_TX */
    { .stream = 11 },   /* DMA2 Channel 4 - SDMMC1 */
};

#define DMA_0_ISR   isr_dma1_channel2
#define DMA_1_ISR   isr_dma1_channel3
#define DMA_2_ISR   isr_dma1_channel4
#define DMA_3_ISR   isr_dma1_channel5
#define DMA_4_ISR   isr_dma1_channel7
#define DMA_5_ISR   isr_dma2_channel6
#define DMA_6_ISR   isr_dma2_channel4

#define DMA_NUMOF   ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name   ADC configuration
 *
 * Note that we do not configure all ADC channels, and not in the STM32L496AG
 * order. Instead, we define 6 ADC channels for the Arduino header pins
 * A0-A5, one channel for V_REFINT and one channel that is used for the
 * STMod+ header pin STMOD+_ADC or the DAC1 output if `periph_dac` is used.
 *
 * The pin assignment can be found in board's user manual in Table 15, page 26
 * (Arduino compatible connectors) and Table 26, page 35 (32L496GDISCOVERY
 * Discovery board I/O assignment). These tables are showing pin assignments and
 * information about ADC - a text similar to `ADC[X]_IN[Y]`, where: \n
 * `[X]` - describes used device - indexed from 0,
 *         for example `ADC3_IN13` is device 2, \n
 * `[Y]` - describes used channel - indexed from 1,
 *         for example `ADC3_IN13` is channel 13
 *
 * Channels shared by multiple ADCs are referred to as `ADC[XXX]_IN[Y]`.
 * `ADC12_IN13` means for example that channel 13 can be used either with ADC1
 * or with ADC2.
 *
 * Output from internal reference voltage V_REFINT is connected to ADC1
 * channel 0.
 *
 * The V_REF+ pin is not connected to an external reference voltage on the
 * board by default. Instead the the VREFBUF must be used (`VREFBUF_ENABLE=1`).
 * The output voltage of VREF is around 2.048 V in this case.
 *
 * If an external reference voltage is used as V_REF+, either by soldering
 * R18 to use V_DDA as reference voltage or by using the AVVD pin of the
 * Arduino connector by closing the solder bridge SB2, the VREFBUF must not
 * be enabled by defining `VREFBUF_ENABLE=0`.
 *
 * @{
 */

static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_C, 4),  .dev = 0, .chan = 13 }, /* A0, ADC12_IN13 */
    { .pin = GPIO_PIN(PORT_C, 1),  .dev = 0, .chan =  2 }, /* A1, ADC123_IN2  */
    { .pin = GPIO_PIN(PORT_C, 3),  .dev = 0, .chan =  4 }, /* A2, ADC123_IN4 */
    { .pin = GPIO_PIN(PORT_F, 10), .dev = 2, .chan = 13 }, /* A3, ADC3_IN13  */
    { .pin = GPIO_PIN(PORT_A, 1),  .dev = 0, .chan =  6 }, /* A4, ADC12_IN6, SB26 closed */
    { .pin = GPIO_PIN(PORT_C, 0),  .dev = 1, .chan = 13 }, /* A5, ADC12_IN13, SB28 closed */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 0 },            /* V_REFINT, ADC1_IN0  */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 18 },           /* V_BAT, ADC1_IN18  */
#ifndef MODULE_PERIPH_DAC
    { .pin = GPIO_PIN(PORT_A, 4),  .dev = 0, .chan =  9 }, /* STMOD+_ADC, ADC12_IN9 */
#else
    { .pin = GPIO_UNDEF,  .dev = 1, .chan = 17 },          /* DAC1, ADC2_IN17 */
#endif
};

/**
 * @brief   Number of ADC devices
 */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)

/**
 * @brief   V_BAT ADC line
 */
#define VBAT_ADC            ADC_LINE(7)

/**
 * @brief   V_REFINT ADC line
 */
#define VREFINT_ADC         ADC_LINE(6)

/**
 * @brief   Enable VREFBUF as V_REF+
 *
 * If an external reference voltage is used as V_REF+, either by soldering
 * R18 to use V_DDA as reference voltage or by using the AVVD pin of the
 * Arduino connector by closing the solder bridge SB2, the VREFBUF must not
 * be enabled by defining `VREFBUF_ENABLE=0`.
 */
#ifndef VREFBUF_ENABLE
#define VREFBUF_ENABLE      (1)
#endif

/** @} */

/**
 * @name    DAC configuration
 *
 * The V_REF+ pin is not connected to an external reference voltage on the
 * board by default. Instead the the VREFBUF must be used (`VREFBUF_ENABLE=1`).
 * The output voltage of VREF is around 2.048 V in this case.
 *
 * @{
 */
static const dac_conf_t dac_config[] = {
    { GPIO_PIN(PORT_A, 4), .chan = 0 },     /* STMod+_ADC pin */
#ifndef MODULE_PERIPH_SPI
    { GPIO_PIN(PORT_A, 5), .chan = 1 },     /* Arduino D13, conflicts with SPI_DEV(0) */
#endif
};
/** @}*/

/**
 * @brief   Number of DACs
 * @{
 */
#define DAC_NUMOF   ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name    I2C configuration
 *
 * I2C1 is exposed at Arduino Connector D14/D15 (SDA/SCL) and STmod+ connector
 * I2C2 is used for MFX (Multi Function eXpander)
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {   /* Shared between Arduino D14/D15 and STMod+ connector */
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C1SEL_1,          /* HSI (16 MHz) */
        .irqn           = I2C1_ER_IRQn,
    },
    {   /* Multi Function eXpander (MFX_x) I2C Address 0x42,
         * Stereo Codec Cirrus Logic CS42L51-CNZ (CODEC_x), I2C Address 0x4a (AD0 = 0)
         * Capacitive Touch Panel (CTP_x) FT6206, I2C Address 0x38
         * Digital Camera Module (DCMI_x),
         */
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_H, 4),
        .sda_pin        = GPIO_PIN(PORT_B, 14),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C2EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C2SEL_1,          /* HSI (16 MHz) */
        .irqn           = I2C2_ER_IRQn,
    },
};

#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c2_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * Four PWM channels are defined for the Arduino header pins D3, D5, D6 and D9,
 * and one for the STMod+ header pin STMOD+_PWM.
 *
 * The pin assignment can be found in board's user manual in Table 15, page 26
 * (Arduino compatible connectors) and Table 26, page 35 (32L496GDISCOVERY
 * Discovery board I/O assignment).
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM8,
        .rcc_mask = RCC_APB2ENR_TIM8EN,
        .chan     = { { .pin = GPIO_PIN(PORT_H, 15), .cc_chan = 6}, /* D3, TIM8_CH3N  */
                      { .pin = GPIO_PIN(PORT_I,  6), .cc_chan = 1}, /* D6, TIM8_CH2  */
                      { .pin = GPIO_PIN(PORT_H, 13), .cc_chan = 4}, /* D9, TIM8_CH1N */
                      { .pin = GPIO_UNDEF,           .cc_chan = 0} },
        .af       = GPIO_AF3,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR1_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B,  9), .cc_chan = 3}, /* D5, TIM4_CH4 */
                      { .pin = GPIO_UNDEF,           .cc_chan = 0},
                      { .pin = GPIO_UNDEF,           .cc_chan = 0},
                      { .pin = GPIO_UNDEF,           .cc_chan = 0} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
    {
        .dev      = TIM5,
        .rcc_mask = RCC_APB1ENR1_TIM5EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A,  0), .cc_chan = 0}, /* STMOD+_PWM, TIM5_CH1 */
                      { .pin = GPIO_UNDEF,           .cc_chan = 0},
                      { .pin = GPIO_UNDEF,           .cc_chan = 0},
                      { .pin = GPIO_UNDEF,           .cc_chan = 0} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   SPI configuration
 *
 * @note By default, solder bridges SB6, SB7, SB8 are closed and USART1 is
 * connected to the Pmod/STMmod+ connector. To use SPI2 on the Pmod/STMmod+
 * connector, these jumpers have to be opened and solder bridges SB4, SB5
 * and SB9 have to be closed instead. Use the `periph_spi_stmod` module
 * to use this configuration, disable USART1 and enable SPI2. Otherwise
 * SPI2 is not available.
 *
 * @{
 */
static const spi_conf_t spi_config[] = {
    { /* Arduino connector */
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 15),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
#if IS_USED(MODULE_PERIPH_DMA)
        .rx_dma   = 0,                        /* DMA1 Channel 2 */
        .rx_dma_chan = 1,                     /* CxS = 1 */
        .tx_dma   = 1,                        /* DMA1 Channel 3 */
        .tx_dma_chan = 1,                     /* CxS = 1 */
#endif
    },
#if IS_USED(MODULE_PERIPH_SPI_STMOD)
    {   /* Pmod/STMod+ connector if solder bridges SB4, SB5, SB9 are closed */
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_I, 2),
        .sclk_pin = GPIO_PIN(PORT_I, 1),
        .cs_pin   = GPIO_PIN(PORT_G, 1),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR1_SPI2EN,
        .apbbus   = APB1,
#if IS_USED(MODULE_PERIPH_DMA)
        .rx_dma   = 2,                        /* DMA1 Channel 4 */
        .rx_dma_chan = 1,                     /* CxS = 1 */
        .tx_dma   = 3,                        /* DMA1 Channel 5 */
        .tx_dma_chan = 1,                     /* CxS = 1 */
#endif
    },
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .dev      = TIM3,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    },
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim3

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 *
 * @note By default, solder bridges SB6, SB7, SB8 are closed and USART1 is
 * connected to the Pmod/STMmod+ connector. In this case the hardware flow
 * control for USART1 is used. If these solder bridges are open and solder
 * bridges SB4, SB5 and SB9 are closed instead, SPI2 is connected to the
 * Pmod/STMmod+ connector and USART1 is not available.
 *
 * @{
 */
static const uart_conf_t uart_config[] = {
    {   /* Virtual COM Port / ST-Link */
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#if IS_USED(MODULE_PERIPH_UART_HW_FC)
        .cts_pin    = GPIO_UNDEF,               /* CTS is not connected */
        .rts_pin    = GPIO_UNDEF,               /* RTS is not connected */
#endif
        .type       = STM32_USART,
        .clk_src    = 0,                        /* Use APB clock */
#if IS_USED(MODULE_PERIPH_DMA)
        .dma        = 4,                        /* DMA1 Channel 7 */
        .dma_chan   = 2,                        /* CxS = 2 */
#endif
    },
    {   /* Arduino connector RX/TX (D0/D1) */
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_G, 8),
        .tx_pin     = GPIO_PIN(PORT_G, 7),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
#if IS_USED(MODULE_PERIPH_UART_HW_FC)
        .cts_pin    = GPIO_UNDEF,               /* CTS is not connected */
        .rts_pin    = GPIO_UNDEF,               /* RTS is not connected */
#endif
        .type       = STM32_LPUART,
        .clk_src    = 0,                        /* Use APB clock */
#if IS_USED(MODULE_PERIPH_DMA)
        .dma        = 5,                        /* DMA2 Channel 6 */
        .dma_chan   = 4,                        /* CxS = 4 */
#endif
    },

#if !IS_USED(MODULE_PERIPH_SPI_STMOD)
    {   /* Pmod/STMod+ connector if solder bridges SB6, SB7, SB8 are closed (default) */
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_G, 10),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#if IS_USED(MODULE_PERIPH_UART_HW_FC)
        .cts_pin    = GPIO_PIN(PORT_G, 11),
        .rts_pin    = GPIO_PIN(PORT_G, 12),
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
        .type       = STM32_USART,
        .clk_src    = 0,                        /* Use APB clock */
#if IS_USED(MODULE_PERIPH_DMA)
        .dma        = 2,                        /* DMA1 Channel 4 */
        .dma_chan   = 2,                        /* CxS = 2 */
#endif
    },
#endif /* !IS_USED(MODULE_PERIPH_SPI_STMOD) */
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_lpuart1)
#define UART_2_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
