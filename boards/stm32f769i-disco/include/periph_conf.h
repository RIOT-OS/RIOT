/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_stm32f769i-disco
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F769I-DISCO board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE    1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE    1
#endif

/* The HSE provides a 25MHz clock */
#ifndef CONFIG_CLOCK_HSE
#define CONFIG_CLOCK_HSE               MHZ(25)
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#include "cfg_usb_otg_fs.h"

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
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
    }
};

#define UART_0_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
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
    .sdnwe_pin = { .pin = GPIO_PIN(PORT_H, 5), .af = GPIO_AF12, },   /* FMC_SDNWE signal */
    .sdnras_pin = { .pin = GPIO_PIN(PORT_F, 11), .af = GPIO_AF12, }, /* FMC_SDNRAS signal */
    .sdncas_pin = { .pin = GPIO_PIN(PORT_G, 15), .af = GPIO_AF12, }, /* FMC_SDNCAS signal */
    .sdcke0_pin = { .pin = GPIO_PIN(PORT_H, 2), .af = GPIO_AF12, },  /* FMC_SDCKE0 signal */
    .sdne0_pin = { .pin = GPIO_PIN(PORT_H, 3), .af = GPIO_AF12, },   /* FMC_SDNE0 signal */
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
#if MODULE_PERIPH_FMC_32BIT
        { .pin = GPIO_PIN(PORT_E, 11), .af = GPIO_AF12, }, /* FMC_D8 signal */
        { .pin = GPIO_PIN(PORT_E, 12), .af = GPIO_AF12, }, /* FMC_D9 signal */
        { .pin = GPIO_PIN(PORT_E, 13), .af = GPIO_AF12, }, /* FMC_D10 signal */
        { .pin = GPIO_PIN(PORT_E, 14), .af = GPIO_AF12, }, /* FMC_D11 signal */
        { .pin = GPIO_PIN(PORT_E, 15), .af = GPIO_AF12, }, /* FMC_D12 signal */
        { .pin = GPIO_PIN(PORT_D, 8), .af = GPIO_AF12, },  /* FMC_D13 signal */
        { .pin = GPIO_PIN(PORT_D, 9), .af = GPIO_AF12, },  /* FMC_D14 signal */
        { .pin = GPIO_PIN(PORT_D, 10), .af = GPIO_AF12, }, /* FMC_D15 signal */
        { .pin = GPIO_PIN(PORT_H, 8), .af = GPIO_AF12, },  /* FMC_D16 signal */
        { .pin = GPIO_PIN(PORT_H, 9), .af = GPIO_AF12, },  /* FMC_D17 signal */
        { .pin = GPIO_PIN(PORT_H, 10), .af = GPIO_AF12, }, /* FMC_D18 signal */
        { .pin = GPIO_PIN(PORT_H, 11), .af = GPIO_AF12, }, /* FMC_D19 signal */
        { .pin = GPIO_PIN(PORT_H, 12), .af = GPIO_AF12, }, /* FMC_D20 signal */
        { .pin = GPIO_PIN(PORT_H, 13), .af = GPIO_AF12, }, /* FMC_D21 signal */
        { .pin = GPIO_PIN(PORT_H, 14), .af = GPIO_AF12, }, /* FMC_D22 signal */
        { .pin = GPIO_PIN(PORT_H, 15), .af = GPIO_AF12, }, /* FMC_D23 signal */
        { .pin = GPIO_PIN(PORT_I, 0), .af = GPIO_AF12, },  /* FMC_D24 signal */
        { .pin = GPIO_PIN(PORT_I, 1), .af = GPIO_AF12, },  /* FMC_D25 signal */
        { .pin = GPIO_PIN(PORT_I, 2), .af = GPIO_AF12, },  /* FMC_D26 signal */
        { .pin = GPIO_PIN(PORT_I, 3), .af = GPIO_AF12, },  /* FMC_D27 signal */
        { .pin = GPIO_PIN(PORT_I, 6), .af = GPIO_AF12, },  /* FMC_D28 signal */
        { .pin = GPIO_PIN(PORT_I, 7), .af = GPIO_AF12, },  /* FMC_D29 signal */
        { .pin = GPIO_PIN(PORT_I, 9), .af = GPIO_AF12, },  /* FMC_D30 signal */
        { .pin = GPIO_PIN(PORT_I, 10), .af = GPIO_AF12, }, /* FMC_D31 signal */
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
 * The board has a SDRAM MT48LC4M32B2B5-6A with 128 MBit on-board.
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
