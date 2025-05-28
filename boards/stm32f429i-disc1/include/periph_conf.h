/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_stm32f429i-disc1
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the STM32F429I-DISC1 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE    1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE    1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_timer_tim5.h"
#include "cfg_usb_otg_hs_phy_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 14 },   /* DMA2 Stream 6 - SPI5_TX */
    { .stream = 13 },   /* DMA2 Stream 5 - SPI5_RX */
};

#define DMA_0_ISR           isr_dma2_stream6
#define DMA_1_ISR           isr_dma2_stream5

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = DMA_STREAM_UNDEF,
        .dma_chan   = UINT8_MAX,
#endif
    }
};

#define UART_0_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI5,
        .mosi_pin       = GPIO_PIN(PORT_F, 9),
        .miso_pin       = GPIO_PIN(PORT_F, 8),
        .sclk_pin       = GPIO_PIN(PORT_F, 7),
        .cs_pin         = GPIO_UNDEF,
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB2ENR_SPI5EN,
        .apbbus         = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma         = 0,
        .tx_dma_chan    = 7,
        .rx_dma         = 1,
        .rx_dma_chan    = 7,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 8),
        .sda_pin        = GPIO_PIN(PORT_C, 9),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C3_EV_IRQn,
    }
};

#define I2C_0_ISR           isr_i2c3_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
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
    .ba0_pin = { .pin = GPIO_PIN(PORT_G, 4), .af = GPIO_AF12, },     /* FMC_BA0 signal */
    .ba1_pin = { .pin = GPIO_PIN(PORT_G, 5), .af = GPIO_AF12, },     /* FMC_BA1 signal */
    .sdclk_pin = { .pin = GPIO_PIN(PORT_G, 8), .af = GPIO_AF12, },   /* FMC_SDCLK signal */
    .sdnwe_pin = { .pin = GPIO_PIN(PORT_C, 0), .af = GPIO_AF12, },   /* FMC_SDNWE signal */
    .sdnras_pin = { .pin = GPIO_PIN(PORT_F, 11), .af = GPIO_AF12, }, /* FMC_SDNRAS signal */
    .sdncas_pin = { .pin = GPIO_PIN(PORT_G, 15), .af = GPIO_AF12, }, /* FMC_SDNCAS signal */
    .sdcke1_pin = { .pin = GPIO_PIN(PORT_B, 5), .af = GPIO_AF12, },  /* FMC_SDCKE1 signal */
    .sdne1_pin = { .pin = GPIO_PIN(PORT_B, 6), .af = GPIO_AF12, },   /* FMC_SDNE1 signal */
    .addr = {
        { .pin = GPIO_PIN(PORT_F, 0), .af = GPIO_AF12, },  /* FMC_A0 signal */
        { .pin = GPIO_PIN(PORT_F, 1), .af = GPIO_AF12, },  /* FMC_A1 signal */
        { .pin = GPIO_PIN(PORT_F, 2), .af = GPIO_AF12, },  /* FMC_A2 signal */
        { .pin = GPIO_PIN(PORT_F, 3), .af = GPIO_AF12, },  /* FMC_A3 signal */
        { .pin = GPIO_PIN(PORT_F, 4), .af = GPIO_AF12, },  /* FMC_A4 signal */
        { .pin = GPIO_PIN(PORT_F, 5), .af = GPIO_AF12, },  /* FMC_A5 signal */
        { .pin = GPIO_PIN(PORT_F, 12), .af = GPIO_AF12, }, /* FMC_A6 signal */
        { .pin = GPIO_PIN(PORT_F, 13), .af = GPIO_AF12, }, /* FMC_A7 signal */
        { .pin = GPIO_PIN(PORT_F, 14), .af = GPIO_AF12, }, /* FMC_A8 signal */
        { .pin = GPIO_PIN(PORT_F, 15), .af = GPIO_AF12, }, /* FMC_A9 signal */
        { .pin = GPIO_PIN(PORT_G, 0), .af = GPIO_AF12, },  /* FMC_A10 signal */
        { .pin = GPIO_PIN(PORT_G, 1), .af = GPIO_AF12, },  /* FMC_A11 signal */
    },
#endif
    .data = {
        { .pin = GPIO_PIN(PORT_D, 14), .af = GPIO_AF12, }, /* FMC_D0 signal */
        { .pin = GPIO_PIN(PORT_D, 15), .af = GPIO_AF12, }, /* FMC_D1 signal */
        { .pin = GPIO_PIN(PORT_D, 0), .af = GPIO_AF12, },  /* FMC_D2 signal */
        { .pin = GPIO_PIN(PORT_D, 1), .af = GPIO_AF12, },  /* FMC_D3 signal */
        { .pin = GPIO_PIN(PORT_E, 7), .af = GPIO_AF12, },  /* FMC_D4 signal */
        { .pin = GPIO_PIN(PORT_E, 8), .af = GPIO_AF12, },  /* FMC_D5 signal */
        { .pin = GPIO_PIN(PORT_E, 9), .af = GPIO_AF12, },  /* FMC_D6 signal */
        { .pin = GPIO_PIN(PORT_E, 10), .af = GPIO_AF12, }, /* FMC_D7 signal */
#if MODULE_PERIPH_FMC_16BIT
        { .pin = GPIO_PIN(PORT_E, 11), .af = GPIO_AF12, }, /* FMC_D8 signal */
        { .pin = GPIO_PIN(PORT_E, 12), .af = GPIO_AF12, }, /* FMC_D9 signal */
        { .pin = GPIO_PIN(PORT_E, 13), .af = GPIO_AF12, }, /* FMC_D10 signal */
        { .pin = GPIO_PIN(PORT_E, 14), .af = GPIO_AF12, }, /* FMC_D11 signal */
        { .pin = GPIO_PIN(PORT_E, 15), .af = GPIO_AF12, }, /* FMC_D12 signal */
        { .pin = GPIO_PIN(PORT_D, 8), .af = GPIO_AF12, },  /* FMC_D13 signal */
        { .pin = GPIO_PIN(PORT_D, 9), .af = GPIO_AF12, },  /* FMC_D14 signal */
        { .pin = GPIO_PIN(PORT_D, 10), .af = GPIO_AF12, }, /* FMC_D15 signal */
#endif
    },
    .nbl0_pin = { .pin = GPIO_PIN(PORT_E, 0), .af = GPIO_AF12, },    /* FMC_NBL0 signal (LB) */
    .nbl1_pin = { .pin = GPIO_PIN(PORT_E, 1), .af = GPIO_AF12, },    /* FMC_NBL1 signal (UB) */
};

/**
 * @brief   FMC Bank configuration
 *
 * The board has a SDRAM IS42S16400J-7TL with 64 MBit on-board.
 * It is organized in 4 banks of 1M x 16 bits each and connected to bank 6
 * at address 0xd0000000.
 */
static const fmc_bank_conf_t fmc_bank_config[] = {
    /* bank 6 is used for SDRAM */
    {
        .bank = FMC_BANK_6,
        .mem_type = FMC_SDRAM,
        .data_width = FMC_BUS_WIDTH_16BIT,
        .address = 0xd0000000,               /* Bank 6 is mapped to 0xd0000000 */
        .size = MiB(8),                      /* Size in Mbyte, 4M x 16 bit */
        .sdram = {
            .clk_period = 2,                 /* SDCLK = 2 x HCLK */
            .row_bits = 12,                  /* A11..A0 used for row address */
            .col_bits = 8,                   /* A8..A0 used for column address */
            .cas_latency = 3,                /* CAS latency is 3 clock cycles */
            .read_delay = 0,                 /* No read delay after CAS */
            .burst_read = false,             /* Burst read mode disabled */
            .burst_write = false,            /* Burst write mode disabled */
            .burst_len = FMC_BURST_LENGTH_1, /* Burst length is 1 if enabled */
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
                .refresh_period = 64,        /* Refresh period in ms */
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
