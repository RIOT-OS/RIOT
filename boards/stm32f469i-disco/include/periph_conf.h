/*
 * Copyright (C) 2021 luisan00
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup   boards_stm32f469i-disco
 * @{
 * @file
 * @brief     Peripheral MCU configuration for the STM32F469I-DISCO
 * @author    Luis A. Ruiz <luisan00@hotmail.com>
 */

/* This board provides a LSE, marked as X3 on the board */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE 1
#endif

/* This board provides a HSE, marked as X2 on the board */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE 1
#endif

/* The HSE provides a 8MHz clock */
#ifndef CONFIG_CLOCK_HSE
#define CONFIG_CLOCK_HSE MHZ(8)
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    {.stream = 14}, /* DMA2 Stream 6 - SPI TX */
    {.stream = 13}, /* DMA2 Stream 5 - SPI RX */
};
#define DMA_0_ISR isr_dma2_stream6
#define DMA_1_ISR isr_dma2_stream5
#define DMA_NUMOF ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * UART_DEV(0): Is used as Virtual Com Port for the STLINK, not available pyshically
 * UART_DEV(1): Available on CN12 and Arduino(R) expansion connector CN7
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin = GPIO_PIN(PORT_B, 11),
        .tx_pin = GPIO_PIN(PORT_B, 10),
        .rx_af = GPIO_AF7,
        .tx_af = GPIO_AF7,
        .bus = APB1,
        .irqn = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = DMA_STREAM_UNDEF,
        .dma_chan = UINT8_MAX
#endif
    },
    {
        .dev = USART6,
        .rcc_mask = RCC_APB2ENR_USART6EN,
        .rx_pin = GPIO_PIN(PORT_C, 7),
        .tx_pin = GPIO_PIN(PORT_C, 6),
        .rx_af = GPIO_AF8,
        .tx_af = GPIO_AF8,
        .bus = APB2,
        .irqn = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = DMA_STREAM_UNDEF,
        .dma_chan = UINT8_MAX
#endif
    },
};
#define UART_0_ISR (isr_usart3)
#define UART_1_ISR (isr_usart6)
#define UART_NUMOF ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   I2C configuration
 *
 * DEV_I2C(0): Available on the connector CN11
 * and the Arduino(R) connector CN5 ONLY if we have the next configuration
 * on the solder bridges; open SB10 and SB12, close SB9 and SB11.
 * For more information see the user manual.
 *
 * DEV_I2C(1): Is not physically available but
 * it's used for the audio DAC-SAI, with address 0x94
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C1,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_B, 8),
        .sda_pin = GPIO_PIN(PORT_B, 9),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C1EN,
        .clk = CLOCK_APB1,
        .irqn = I2C1_EV_IRQn,
    },
    {
        .dev = I2C2,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_H, 4),
        .sda_pin = GPIO_PIN(PORT_H, 5),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C2EN,
        .clk = CLOCK_APB1,
        .irqn = I2C2_EV_IRQn,
    },
};
#define I2C_0_ISR isr_i2c1_ev
#define I2C_1_ISR isr_i2c2_ev
#define I2C_NUMOF ARRAY_SIZE(i2c_config)
/** @}*/

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_D, 3),
        .cs_pin = GPIO_PIN(PORT_H, 6),
        .mosi_af = GPIO_AF5,
        .miso_af = GPIO_AF5,
        .sclk_af = GPIO_AF5,
        .cs_af = GPIO_AF5,
        .rccmask = RCC_APB1ENR_I2C2EN,
        .apbbus = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma = 0,
        .tx_dma_chan = 6,
        .rx_dma = 1,
        .rx_dma_chan = 6,
#endif
    },
};
#define SPI_NUMOF ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_A, 6), .cc_chan = 0}, /* D6 */
            {.pin = GPIO_PIN(PORT_B, 5), .cc_chan = 1}, /* CN12-P9 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
        },
        .af = GPIO_AF2,
        .bus = APB1,
    },
    {
        .dev = TIM5,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_A, 1), .cc_chan = 1}, /* D5 */
            {.pin = GPIO_PIN(PORT_A, 2), .cc_chan = 2}, /* D3 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
        },
        .af = GPIO_AF2,
        .bus = APB1,
    },
    {
        .dev = TIM12,
        .rcc_mask = RCC_APB1ENR_TIM12EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_H, 6), .cc_chan = 0},     /* D10 */
            {.pin = GPIO_PIN(PORT_B, 15), .cc_chan = 1},    /* D11 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
        },
        .af = GPIO_AF9,
        .bus = APB1,
    },
    {
        .dev = TIM14,
        .rcc_mask = RCC_APB1ENR_TIM14EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_A, 7), .cc_chan = 0},     /* D9 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
        },
        .af = GPIO_AF9,
        .bus = APB1,
    },
};
#define PWM_NUMOF   ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name ADC configuration
 *
 * Not all ADCs are configured, by now, only the 6 ones available
 * on the Arduino(R) connector A0-A5 and internal VBAT ADC channel.
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_B, 1), 2, 9},
    {GPIO_PIN(PORT_C, 2), 2, 12},
    {GPIO_PIN(PORT_C, 3), 2, 13},
    {GPIO_PIN(PORT_C, 4), 2, 14},
    {GPIO_PIN(PORT_C, 5), 2, 15},
    {GPIO_PIN(PORT_A, 4), 2, 4},
    {GPIO_UNDEF, 0, 18}, /* VBAT */
};

#define VBAT_ADC    ADC_LINE(6) /**< VBAT ADC line */
#define ADC_NUMOF   ARRAY_SIZE(adc_config)
/** @} */

/**
 * @brief DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    {GPIO_PIN(PORT_A, 4), .chan = 0},
    {GPIO_PIN(PORT_A, 5), .chan = 1},
};
/** @}*/

/**
 * @brief Number of DACs
 * @{
 */
#define DAC_NUMOF   ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name    FMC configuration
 * @{
 */
/**
 * @brief   FMC controller configuration
 */
static const fmc_conf_t fmc_config = {
    .bus = AHB3,
    .rcc_mask = RCC_AHB3ENR_FMCEN,
#if MODULE_PERIPH_FMC_SDRAM
    .ba0_pin = { .pin = GPIO_PIN(PORT_G, 4), .af = GPIO_AF12, },     /* A14 signal */
    .ba1_pin = { .pin = GPIO_PIN(PORT_G, 5), .af = GPIO_AF12, },     /* A15 signal */
    .sdclk_pin = { .pin = GPIO_PIN(PORT_G, 8), .af = GPIO_AF12, },   /* SDCLK signal */
    .sdnwe_pin = { .pin = GPIO_PIN(PORT_C, 0), .af = GPIO_AF12, },   /* SDNWE signal */
    .sdnras_pin = { .pin = GPIO_PIN(PORT_F, 11), .af = GPIO_AF12, }, /* SDNRAS signal */
    .sdncas_pin = { .pin = GPIO_PIN(PORT_G, 15), .af = GPIO_AF12, }, /* SDNCAS signal */
    .sdcke0_pin = { .pin = GPIO_PIN(PORT_H, 2), .af = GPIO_AF12, },  /* SDCKE0 signal */
    .sdne0_pin = { .pin = GPIO_PIN(PORT_H, 3), .af = GPIO_AF12, },   /* SDNE0 signal */
    .addr = {
        { .pin = GPIO_PIN(PORT_F, 0), .af = GPIO_AF12, },  /* A0 signal */
        { .pin = GPIO_PIN(PORT_F, 1), .af = GPIO_AF12, },  /* A1 signal */
        { .pin = GPIO_PIN(PORT_F, 2), .af = GPIO_AF12, },  /* A2 signal */
        { .pin = GPIO_PIN(PORT_F, 3), .af = GPIO_AF12, },  /* A3 signal */
        { .pin = GPIO_PIN(PORT_F, 4), .af = GPIO_AF12, },  /* A4 signal */
        { .pin = GPIO_PIN(PORT_F, 5), .af = GPIO_AF12, },  /* A5 signal */
        { .pin = GPIO_PIN(PORT_F, 12), .af = GPIO_AF12, }, /* A6 signal */
        { .pin = GPIO_PIN(PORT_F, 13), .af = GPIO_AF12, }, /* A7 signal */
        { .pin = GPIO_PIN(PORT_F, 14), .af = GPIO_AF12, }, /* A8 signal */
        { .pin = GPIO_PIN(PORT_F, 15), .af = GPIO_AF12, }, /* A9 signal */
        { .pin = GPIO_PIN(PORT_G, 0), .af = GPIO_AF12, },  /* A10 signal */
        { .pin = GPIO_PIN(PORT_G, 1), .af = GPIO_AF12, },  /* A11 signal */
    },
#endif
    .data = {
        { .pin = GPIO_PIN(PORT_D, 14), .af = GPIO_AF12, }, /* D0 signal */
        { .pin = GPIO_PIN(PORT_D, 15), .af = GPIO_AF12, }, /* D1 signal */
        { .pin = GPIO_PIN(PORT_D, 0), .af = GPIO_AF12, },  /* D2 signal */
        { .pin = GPIO_PIN(PORT_D, 1), .af = GPIO_AF12, },  /* D3 signal */
        { .pin = GPIO_PIN(PORT_E, 7), .af = GPIO_AF12, },  /* D4 signal */
        { .pin = GPIO_PIN(PORT_E, 8), .af = GPIO_AF12, },  /* D5 signal */
        { .pin = GPIO_PIN(PORT_E, 9), .af = GPIO_AF12, },  /* D6 signal */
        { .pin = GPIO_PIN(PORT_E, 10), .af = GPIO_AF12, }, /* D7 signal */
#if MODULE_PERIPH_FMC_32BIT
        { .pin = GPIO_PIN(PORT_E, 11), .af = GPIO_AF12, }, /* D8 signal */
        { .pin = GPIO_PIN(PORT_E, 12), .af = GPIO_AF12, }, /* D9 signal */
        { .pin = GPIO_PIN(PORT_E, 13), .af = GPIO_AF12, }, /* D10 signal */
        { .pin = GPIO_PIN(PORT_E, 14), .af = GPIO_AF12, }, /* D11 signal */
        { .pin = GPIO_PIN(PORT_E, 15), .af = GPIO_AF12, }, /* D12 signal */
        { .pin = GPIO_PIN(PORT_D, 8), .af = GPIO_AF12, },  /* D13 signal */
        { .pin = GPIO_PIN(PORT_D, 9), .af = GPIO_AF12, },  /* D14 signal */
        { .pin = GPIO_PIN(PORT_D, 10), .af = GPIO_AF12, }, /* D15 signal */
        { .pin = GPIO_PIN(PORT_H, 8), .af = GPIO_AF12, },  /* D16 signal */
        { .pin = GPIO_PIN(PORT_H, 9), .af = GPIO_AF12, },  /* D17 signal */
        { .pin = GPIO_PIN(PORT_H, 10), .af = GPIO_AF12, }, /* D18 signal */
        { .pin = GPIO_PIN(PORT_H, 11), .af = GPIO_AF12, }, /* D19 signal */
        { .pin = GPIO_PIN(PORT_H, 12), .af = GPIO_AF12, }, /* D20 signal */
        { .pin = GPIO_PIN(PORT_H, 13), .af = GPIO_AF12, }, /* D21 signal */
        { .pin = GPIO_PIN(PORT_H, 14), .af = GPIO_AF12, }, /* D22 signal */
        { .pin = GPIO_PIN(PORT_H, 15), .af = GPIO_AF12, }, /* D23 signal */
        { .pin = GPIO_PIN(PORT_I, 0), .af = GPIO_AF12, },  /* D24 signal */
        { .pin = GPIO_PIN(PORT_I, 1), .af = GPIO_AF12, },  /* D25 signal */
        { .pin = GPIO_PIN(PORT_I, 2), .af = GPIO_AF12, },  /* D26 signal */
        { .pin = GPIO_PIN(PORT_I, 3), .af = GPIO_AF12, },  /* D27 signal */
        { .pin = GPIO_PIN(PORT_I, 6), .af = GPIO_AF12, },  /* D28 signal */
        { .pin = GPIO_PIN(PORT_I, 7), .af = GPIO_AF12, },  /* D29 signal */
        { .pin = GPIO_PIN(PORT_I, 9), .af = GPIO_AF12, },  /* D30 signal */
        { .pin = GPIO_PIN(PORT_I, 10), .af = GPIO_AF12, }, /* D31 signal */
#endif
    },
    .nbl0_pin = { .pin = GPIO_PIN(PORT_E, 0), .af = GPIO_AF12, },   /* FMC_NBL0 signal (DQM0) */
    .nbl1_pin = { .pin = GPIO_PIN(PORT_E, 1), .af = GPIO_AF12, },   /* FMC_NBL1 signal (DQM1) */
    .nbl2_pin = { .pin = GPIO_PIN(PORT_I, 4), .af = GPIO_AF12, },   /* FMC_NBL2 signal (DQM2) */
    .nbl3_pin = { .pin = GPIO_PIN(PORT_I, 5), .af = GPIO_AF12, },   /* FMC_NBL3 signal (DQM3) */
};

/**
 * @brief   FMC Bank configuration
 *
 * The board has a SDRAM IS42S32400F-6BL with 128 MBit on-board.
 * It is organized in 4 banks of 1M x 32 bits each and connected to bank 5
 * at address 0xc0000000.
 */
static const fmc_bank_conf_t fmc_bank_config[] = {
    /* bank 5 is used for SDRAM */
    {
        .bank = FMC_BANK_5,
        .mem_type = FMC_SDRAM,
        .data_width = FMC_BUS_WIDTH_32BIT,
        .address = 0xc0000000,               /* Bank 6 is mapped to 0xc0000000 */
        .size = MiB(16),                      /* Size in MByte, 4M x 32 Bit */
        .sdram = {
            .clk_period = 2,                 /* SDCLK = 2 x HCLK */
            .row_bits = 12,                  /* A11..A0 used for row address */
            .col_bits = 8,                   /* A7..A0 used for column address */
            .cas_latency = 2,                /* CAS latency is 2 clock cycles */
            .read_delay = 0,                 /* No read delay after CAS */
            .burst_read = true,              /* Burst read mode enabled */
            .burst_write = false,            /* Burst write mode disabled */
            .burst_len = FMC_BURST_LENGTH_1, /* Burst length is 1 */
            .burst_interleaved = false,      /* Burst mode interleaved */
            .write_protect = false,          /* No write protection */
            .four_banks = true,              /* SDRAM has four internal banks */
            .timing = {                      /* SDRAM Timing parameters */
                .row_to_col_delay = 2,       /* Row to column delay (2 clock cycles) */
                .row_precharge = 2,          /* Row precharge delay (2 clock cycles) */
                .recovery_delay = 2,         /* Recovery delay is (2 clock cycles) */
                .row_cylce = 7,              /* Row cycle delay is (7 clock cycles) */
                .self_refresh = 4,           /* Self refresh time is (4 clock cycles) */
                .exit_self_refresh = 7,      /* Exit self-refresh delay (7 clock cycles) */
                .load_mode_register = 2,     /* Load Mode Register to Activate delay */
                .refresh_period = 16,        /* Refresh period in ms */
            },
        },
    },
};

/**
 * @brief   Number of configured FMC banks
 */
#define FMC_BANK_NUMOF  ARRAY_SIZE(fmc_bank_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
