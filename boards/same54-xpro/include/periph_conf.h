/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_same54-xpro
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Microchip SAM E54 Xplained
 *              Pro board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Use the external oscillator to source all fast clocks.
 *          This allows us to use the buck voltage regulator for
 *          maximum power efficiency, but limits the maximum clock
 *          frequency to 12 MHz.
 */
#ifndef USE_XOSC_ONLY
#define USE_XOSC_ONLY       (0)
#endif

/**
 * @name    external Oscillator (XOSC1) configuration
 * @{
 */
#define XOSC1_FREQUENCY     MHZ(12)
/** @} */

/**
 * @name    desired core clock frequency
 * @{
 */
#ifndef CLOCK_CORECLOCK
#if USE_XOSC_ONLY
#define CLOCK_CORECLOCK     XOSC1_FREQUENCY
#else
#define CLOCK_CORECLOCK     MHZ(120)
#endif
#endif
/** @} */

/**
 * @name    32kHz Oscillator configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                    1
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE      0
/** @} */

/**
 * @brief Enable the internal DC/DC converter
 *        The board is equipped with the necessary inductor.
 */
#define USE_VREG_BUCK       (1)

/**
 * @name Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC0,
        .irq            = TC0_IRQn,
        .mclk           = &MCLK->APBAMASK.reg,
        .mclk_mask      = MCLK_APBAMASK_TC0 | MCLK_APBAMASK_TC1,
        .gclk_id        = TC0_GCLK_ID,
        .gclk_src       = SAM0_GCLK_TIMER,
        .flags          = TC_CTRLA_MODE_COUNT32,
    },
    {   /* Timer 1 */
        .dev            = TC2,
        .irq            = TC2_IRQn,
        .mclk           = &MCLK->APBBMASK.reg,
        .mclk_mask      = MCLK_APBBMASK_TC2 | MCLK_APBBMASK_TC3,
        .gclk_id        = TC2_GCLK_ID,
        .gclk_src       = SAM0_GCLK_TIMER,
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0

/* Timer 1 configuration */
#define TIMER_1_CHANNELS    2
#define TIMER_1_ISR         isr_tc2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @brief ATA6561 STANDBY pin definition
 * @{
 */
#define AT6561_STBY_PIN            GPIO_PIN(PC, 13)
/** @} */

/**
 * @name CAN configuration
 * @{
 */
/** Available CAN interfaces */
static const can_conf_t candev_conf[] = {
    {
        .can = CAN1,
        .rx_pin = GPIO_PIN(PB, 13),
        .tx_pin = GPIO_PIN(PB, 12),
        .gclk_src = SAM0_GCLK_PERIPH,
        .enable_pin = AT6561_STBY_PIN,
        .enable_pin_mode = GPIO_OUT,
        .enable_pin_active_low = true,
    }
};

/** CAN 1 configuration */
#define ISR_CAN1    isr_can1

/** Number of CAN interfaces */
#define CAN_NUMOF         ARRAY_SIZE(candev_conf)
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM2->USART,
        .rx_pin   = GPIO_PIN(PB, 24),
        .tx_pin   = GPIO_PIN(PB, 25),
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {    /* EXT1 */
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA, 5),
        .tx_pin   = GPIO_PIN(PA, 4),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_PIN(PA, 6),
        .cts_pin  = GPIO_PIN(PA, 7),
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
#ifdef MODULE_PERIPH_UART_HW_FC
        .tx_pad   = UART_PAD_TX_0_RTS_2_CTS_3,
#else
        .tx_pad   = UART_PAD_TX_0,
#endif
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {    /* EXT2 */
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PB, 17),
        .tx_pin   = GPIO_PIN(PB, 16),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {    /* EXT3 */
        .dev      = &SERCOM1->USART,
        .rx_pin   = GPIO_PIN(PC, 23),
        .tx_pin   = GPIO_PIN(PC, 22),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom2_2
#define UART_0_ISR_TX       isr_sercom2_0

#define UART_1_ISR          isr_sercom0_2
#define UART_1_ISR_TX       isr_sercom0_0

#define UART_2_ISR          isr_sercom5_2
#define UART_2_ISR_TX       isr_sercom5_0

#define UART_3_ISR          isr_sercom1_2
#define UART_3_ISR_TX       isr_sercom1_0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name PWM configuration
 * @{
 */

/* PWM0 channels */
static const pwm_conf_chan_t pwm_chan0_config[] = {
    /* GPIO pin, MUX value, TCC channel */
    {
        .pin  = GPIO_PIN(PC, 18),
        .mux  = GPIO_MUX_F,
        .chan = 2
    },
};

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
    {
        .tim  = TCC_CONFIG(TCC0),
        .chan = pwm_chan0_config,
        .chan_numof = ARRAY_SIZE(pwm_chan0_config),
        .gclk_src = SAM0_GCLK_48MHZ,
    },
};

/* number of devices that are actually defined */
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {    /* EXT1 */
        .dev      = &(SERCOM4->SPI),
        .miso_pin = GPIO_PIN(PB, 29),
        .mosi_pin = GPIO_PIN(PB, 27),
        .clk_pin  = GPIO_PIN(PB, 26),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM4_DMAC_ID_TX,
        .rx_trigger = SERCOM4_DMAC_ID_RX,
#endif

    },
    {    /* EXT2, EXT3 */
        .dev      = &(SERCOM6->SPI),
        .miso_pin = GPIO_PIN(PC, 7),
        .mosi_pin = GPIO_PIN(PC, 4),
        .clk_pin  = GPIO_PIN(PC, 5),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_48MHZ,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM6_DMAC_ID_TX,
        .rx_trigger = SERCOM6_DMAC_ID_RX,
#endif
    },
#ifdef MODULE_PERIPH_SPI_ON_QSPI
    {    /* QSPI in SPI mode */
        .dev      = QSPI,
        .miso_pin = SAM0_QSPI_PIN_DATA_1,
        .mosi_pin = SAM0_QSPI_PIN_DATA_0,
        .clk_pin  = SAM0_QSPI_PIN_CLK,
        .miso_mux = SAM0_QSPI_MUX,
        .mosi_mux = SAM0_QSPI_MUX,
        .clk_mux  = SAM0_QSPI_MUX,
        .miso_pad = SPI_PAD_MISO_0,         /* unused */
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,   /* unused */
        .gclk_src = SAM0_GCLK_MAIN,         /* unused */
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = QSPI_DMAC_ID_TX,
        .rx_trigger = QSPI_DMAC_ID_RX,
#endif
    },
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {    /* EXT1 */
        .dev      = &(SERCOM3->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 23),
        .sda_pin  = GPIO_PIN(PA, 22),
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_PERIPH,
        .flags    = I2C_FLAG_NONE
    },
    {    /* EXT2, EXT3 */
        .dev      = &(SERCOM7->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PD, 9),
        .sda_pin  = GPIO_PIN(PD, 8),
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_PERIPH,
        .flags    = I2C_FLAG_NONE
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (32768U)
#endif
/** @} */

/**
 * @name USB peripheral configuration
 * @{
 */
static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm     = GPIO_PIN(PA, 24),
        .dp     = GPIO_PIN(PA, 25),
        .d_mux  = GPIO_MUX_H,
        .device = &USB->DEVICE,
        .gclk_src = SAM0_GCLK_PERIPH,
    }
};
/** @} */

/**
 * @name ADC Configuration
 * @{
 */

/* ADC Default values */
#define ADC_GCLK_SRC                        SAM0_GCLK_PERIPH    /**< clock used for ADC */
#define ADC_PRESCALER                       ADC_CTRLA_PRESCALER_DIV8

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC1

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos, dev */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA03, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA05, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA07, .dev = ADC0 }
};

#define ADC_NUMOF                           ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name DAC configuration
 * @{
 */
                            /* Must not exceed 12 MHz */
#define DAC_CLOCK           SAM0_GCLK_TIMER
                            /* Use external reference voltage on PA03 */
                            /* (You have to manually connect PA03 with Vcc) */
                            /* Internal reference only gives 1V */
#define DAC_VREF            DAC_CTRLB_REFSEL_VREFPU
/** @} */

/**
 * @name SDHC configuration
 *
 *       This is entirely optional, but allows us to save a few bytes if only
 *       a single SDHC instance is used.
 * @{
 */
#define SDHC_DEV            SDHC1       /**< The SDHC instance to use */
#define SDHC_DEV_ISR        isr_sdhc1   /**< Interrupt service routing for SDHC1 */

/** SDHC devices */
static const sdhc_conf_t sdhc_config[] = {
    {
        .sdhc = SDHC1,
        .cd = GPIO_PIN(PD, 20),
        .wp = GPIO_UNDEF,
    },
};

/** Number of configured SDHC devices */
#define SDHC_CONFIG_NUMOF  1
/** @} */

/**
 * @name Ethernet peripheral configuration
 * @{
 */
static const sam0_common_gmac_config_t sam_gmac_config[] = {
    {
        .dev = GMAC,
        .refclk = GPIO_PIN(PA, 14),
        .txen = GPIO_PIN(PA, 17),
        .txd0 = GPIO_PIN(PA, 18),
        .txd1 = GPIO_PIN(PA, 19),
        .crsdv = GPIO_PIN(PC, 20),
        .rxd0 = GPIO_PIN(PA, 13),
        .rxd1 = GPIO_PIN(PA, 12),
        .rxer = GPIO_PIN(PA, 15),
        .mdc = GPIO_PIN(PC, 11),
        .mdio = GPIO_PIN(PC, 12),
        .rst_pin = GPIO_PIN(PC, 21),
        .int_pin = GPIO_PIN(PD, 12),
    }
};
/** @} */

/**
 * @name FREQM peripheral configuration
 * @{
 */
static const freqm_config_t freqm_config[] = {
    {
        .pin = GPIO_PIN(PB, 17),
        .gclk_src = SAM0_GCLK_32KHZ
    }
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
