/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Luis A. Ruiz <luisan00@hotmail.com>
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
#include "mii.h"

#if defined(MODULE_PERIPH_USBDEV_HS_ULPI) || DOXYGEN
#include "usbdev_synopsys_dwc2.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 15 }, /* DMA2 Stream 7 - USART1_TX */
    { .stream = 7 },  /* DMA1 Stream 7 - UART5_TX */
    { .stream = 14 }, /* DMA2 Stream 6 - USART6_TX */
    { .stream = 8 },  /* DMA2 Stream 0 - ETH_TX    */
    { .stream = 3 },  /* DMA1 Stream 3 - SPI2_RX   */
    { .stream = 4 },  /* DMA1 Stream 4 - SPI2_TX   */
    { .stream = 13 }, /* DMA2 Stream 5 - SDMMC2    */
};

#define DMA_0_ISR  isr_dma2_stream7
#define DMA_1_ISR  isr_dma1_stream7
#define DMA_2_ISR  isr_dma2_stream6
#define DMA_3_ISR  isr_dma2_stream0
#define DMA_4_ISR  isr_dma1_stream3
#define DMA_5_ISR  isr_dma1_stream4
#define DMA_6_ISR  isr_dma2_stream5

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    I2C configuration
 *
 * | Device     | Bus  | SCL               | SDA               | Usage                     |
 * |------------|------|-------------------|-------------------|---------------------------|
 * | I2C_DEV(0) | I2C1 | PB8 (Arduino D15) | PB9 (Arduino D14) | Arduino connector         |
 * | I2C_DEV(1) | I2C4 | PD12              | PB7               | Touch panel, audio codec  |
 *
 * I2C4 connects the FT6x06 touch panel controller (INT on PI13) and the
 * WM8994 audio codec (address 0x1a).
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
        .rcc_sw_mask = RCC_DCKCFGR2_I2C1SEL_1,
        .irqn = I2C1_ER_IRQn,
    },
    {
        .dev = I2C4,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_D, 12),
        .sda_pin = GPIO_PIN(PORT_B, 7),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF11,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C4EN,
        .rcc_sw_mask = RCC_DCKCFGR2_I2C4SEL_1,
        .irqn = I2C4_ER_IRQn,
    },
};
#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c4_er
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    UART configuration
 *
 * | Device      | UART   | RX               | TX               | Usage                    |
 * |-------------|--------|------------------|------------------|--------------------------|
 * | UART_DEV(0) | USART1 | PA10             | PA9              | ST-LINK virtual COM port |
 * | UART_DEV(1) | UART5  | PD2              | PC12             | Wi-Fi connector CN2      |
 * | UART_DEV(2) | USART6 | PC7 (Arduino D0) | PC6 (Arduino D1) | Arduino connector CN13   |
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
        .dma        = 0,
        .dma_chan   = 4,
#endif
    },
    {
        .dev        = UART5,
        .rcc_mask   = RCC_APB1ENR_UART5EN,
        .rx_pin     = GPIO_PIN(PORT_D, 2),  /* WIFI_TX on CN2 */
        .tx_pin     = GPIO_PIN(PORT_C, 12), /* WIFI_RX on CN2 */
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB1,
        .irqn       = UART5_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 1,
        .dma_chan   = 4,
#endif
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_C, 7),
        .tx_pin     = GPIO_PIN(PORT_C, 6),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = 5,
#endif
    },
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_uart5)
#define UART_2_ISR          (isr_usart6)
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 *
 * | Device     | Bus  | SCK                | MISO               | MOSI               |
 * |------------|------|--------------------|--------------------|--------------------|
 * | SPI_DEV(0) | SPI2 | PA12 (Arduino D13) | PB14 (Arduino D12) | PB15 (Arduino D11) |
 *
 * The Arduino D10 pin (PA11) is the usual chip select GPIO. Note that SCK is
 * shared with the LED LD3, which therefore flickers during SPI transfers.
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_A, 12),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 5,
        .tx_dma_chan = 0,
        .rx_dma   = 4,
        .rx_dma_chan = 0,
#endif
    },
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    ADC configuration
 *
 * ADC lines for the Arduino connector pins A0-A5, plus the internal VBAT
 * channel.
 *
 * | Line        | Pin  | Channel   |
 * |-------------|------|-----------|
 * | ADC_LINE(0) | PA6  | ADC1_IN6  |
 * | ADC_LINE(1) | PA4  | ADC1_IN4  |
 * | ADC_LINE(2) | PC2  | ADC1_IN12 |
 * | ADC_LINE(3) | PF10 | ADC3_IN8  |
 * | ADC_LINE(4) | PF8  | ADC3_IN6  |
 * | ADC_LINE(5) | PF9  | ADC3_IN7  |
 * | ADC_LINE(6) | -    | VBAT      |
 * @{
 */
static const adc_conf_t adc_config[] = {
    { GPIO_PIN(PORT_A, 6), 0, 6 },   /* Arduino A0 */
    { GPIO_PIN(PORT_A, 4), 0, 4 },   /* Arduino A1 */
    { GPIO_PIN(PORT_C, 2), 0, 12 },  /* Arduino A2 */
    { GPIO_PIN(PORT_F, 10), 2, 8 },  /* Arduino A3 */
    { GPIO_PIN(PORT_F, 8), 2, 6 },   /* Arduino A4 */
    { GPIO_PIN(PORT_F, 9), 2, 7 },   /* Arduino A5 */
    { GPIO_UNDEF, 0, 18 },           /* VBAT */
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define ADC_CLK_MAX         MHZ(36)     /**< Max ADC clock (36 MHz for the F7) */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    DAC configuration
 *
 * DAC channel 1 (DAC_OUT1) is available on PA4, the Arduino A1 pin.
 * @{
 */
static const dac_conf_t dac_config[] = {
    { GPIO_PIN(PORT_A, 4), .chan = 0 },
};

#define DAC_NUMOF           ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name    Ethernet configuration
 *
 * RMII interface to the on-board LAN8742A PHY at address 0.
 * @{
 */
static const eth_conf_t eth_config = {
    .mode = RMII,
    .speed = MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX,
    .dma = 3,
    .dma_chan = 8,
    .phy_addr = 0x00,
    .pins = {
        GPIO_PIN(PORT_G, 13),   /* TXD0 */
        GPIO_PIN(PORT_G, 14),   /* TXD1 */
        GPIO_PIN(PORT_G, 11),   /* TX_EN */
        GPIO_PIN(PORT_C, 4),    /* RXD0 */
        GPIO_PIN(PORT_C, 5),    /* RXD1 */
        GPIO_PIN(PORT_A, 7),    /* CRS_DV */
        GPIO_PIN(PORT_C, 1),    /* MDC */
        GPIO_PIN(PORT_A, 2),    /* MDIO */
        GPIO_PIN(PORT_A, 1),    /* REF_CLK */
    }
};
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
 * at address 0xc0000000. The CAS latency and timings are the ones used by
 * ST's board support package (BSP) for this board.
 */
static const fmc_bank_conf_t fmc_bank_config[] = {
    /* bank 5 is used for SDRAM */
    {
        .bank = FMC_BANK_5,
        .mem_type = FMC_SDRAM,
        .data_width = FMC_BUS_WIDTH_32BIT,
        .address = 0xc0000000,               /* Bank 5 is mapped to 0xc0000000 */
        .size = MiB(16),                     /* Size in MByte, 4M x 32 Bit */
        .sdram = {
            .clk_period = 2,                 /* SDCLK = 2 x HCLK */
            .row_bits = 12,                  /* A11..A0 used for row address */
            .col_bits = 8,                   /* A7..A0 used for column address */
            .cas_latency = 3,                /* CAS latency is 3 clock cycles */
            .read_delay = 0,                 /* No read delay after CAS */
            .four_banks = true,              /* SDRAM has four internal banks */
            .write_protect = false,          /* No write protection */
            .burst_read = true,              /* Burst read mode enabled */
            .burst_write = false,            /* Burst write mode disabled */
            .burst_interleaved = false,      /* Burst mode interleaved */
            .burst_len = FMC_BURST_LENGTH_1, /* Burst length is 1 */
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

/**
 * @name    PWM configuration
 *
 * | Device     | Timer | Channel | Pin  | Arduino |
 * |------------|-------|---------|------|---------|
 * | PWM_DEV(0) | TIM10 | 0       | PF6  | CN13 D3 |
 * | PWM_DEV(1) | TIM3  | 0       | PC8  | CN13 D5 |
 * | PWM_DEV(2) | TIM11 | 0       | PF7  | CN13 D6 |
 * | PWM_DEV(3) | TIM12 | 0       | PH6  | CN9 D9  |
 * | PWM_DEV(3) | TIM12 | 1       | PB15 | CN9 D11 |
 * | PWM_DEV(4) | TIM1  | 0       | PA11 | CN9 D10 |
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIM10,
        .rcc_mask = RCC_APB2ENR_TIM10EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_F, 6), .cc_chan = 0 }, /* CN13 D3 */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
        },
        .af = GPIO_AF3,
        .bus = APB2,
    },
    {
        .dev = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_C, 8), .cc_chan = 2 }, /* CN13 D5 */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
        },
        .af = GPIO_AF2,
        .bus = APB1,
    },
    {
        .dev = TIM11,
        .rcc_mask = RCC_APB2ENR_TIM11EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_F, 7), .cc_chan = 0 }, /* CN13 D6 */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },          /* unused */
        },
        .af = GPIO_AF3,
        .bus = APB2,
    },
    {
        .dev = TIM12,
        .rcc_mask = RCC_APB1ENR_TIM12EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_H, 6), .cc_chan = 0 },  /* CN9 D9 */
            { .pin = GPIO_PIN(PORT_B, 15), .cc_chan = 1 }, /* CN9 D11 */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },           /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },           /* unused */
        },
        .af = GPIO_AF9,
        .bus = APB1,
    },
    {
        .dev = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_A, 11), .cc_chan = 3 }, /* CN9 D10 */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },           /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },           /* unused */
            { .pin = GPIO_UNDEF, .cc_chan = 0 },           /* unused */
        },
        .af = GPIO_AF1,
        .bus = APB2,
    },
};
#define PWM_NUMOF   ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    SDIO/SDMMC configuration
 *
 * The on-board microSD card slot (CN5) is connected to SDMMC2, with the card
 * detect signal on PI15 (low is active)
 * @{
 */
static const sdmmc_conf_t sdmmc_config[] = {
    {
        .dev = SDMMC2,
        .bus = APB2,
        .rcc_mask = RCC_APB2ENR_SDMMC2EN,
        .cd = GPIO_PIN(PORT_I, 15),
        .cd_active = 0,
        .cd_mode = GPIO_IN_PU,
        .clk = { GPIO_PIN(PORT_D, 6), GPIO_AF11 },
        .cmd = { GPIO_PIN(PORT_D, 7), GPIO_AF11 },
        .dat0 = { GPIO_PIN(PORT_G, 9), GPIO_AF11 },
        .dat1 = { GPIO_PIN(PORT_G, 10), GPIO_AF11 },
        .dat2 = { GPIO_PIN(PORT_B, 3), GPIO_AF10 },
        .dat3 = { GPIO_PIN(PORT_B, 4), GPIO_AF10 },
#ifdef MODULE_PERIPH_DMA
        .dma = 6,
        .dma_chan = 11,
#endif
        .irqn = SDMMC2_IRQn,
    },
};

/**
 * @brief   Number of configured SDIO/SDMMC peripherals
 */
#define SDMMC_CONFIG_NUMOF  1
/** @} */

#if defined(MODULE_PERIPH_USBDEV_HS_ULPI) || DOXYGEN
/**
 * @name    USB OTG HS configuration with ULPI PHY
 *
 * The only user USB port of the board (CN15) is connected to the USB OTG HS
 * peripheral through an external ULPI PHY.
 * @{
 */

/**
 * @brief   Enable the high speed USB OTG peripheral
 */
#define DWC2_USB_OTG_HS_ENABLED

/**
 * @brief   USB OTG HS configuration with ULPI PHY
 */
static const dwc2_usb_otg_fshs_config_t dwc2_usb_otg_fshs_config[] = {
    {
        .periph   = USB_OTG_HS_PERIPH_BASE,
        .type     = DWC2_USB_OTG_HS,
        .phy      = DWC2_USB_OTG_PHY_ULPI,
        .rcc_mask = RCC_AHB1ENR_OTGHSEN,
        .irqn     = OTG_HS_IRQn,
        .ahb      = AHB1,
        .ulpi_af  = GPIO_AF10,
        .ulpi_clk = GPIO_PIN(PORT_A, 5),
        .ulpi_d0  = GPIO_PIN(PORT_A, 3),
        .ulpi_d1  = GPIO_PIN(PORT_B, 0),
        .ulpi_d2  = GPIO_PIN(PORT_B, 1),
        .ulpi_d3  = GPIO_PIN(PORT_B, 10),
        .ulpi_d4  = GPIO_PIN(PORT_B, 11),
        .ulpi_d5  = GPIO_PIN(PORT_B, 12),
        .ulpi_d6  = GPIO_PIN(PORT_B, 13),
        .ulpi_d7  = GPIO_PIN(PORT_B, 5),
        .ulpi_dir = GPIO_PIN(PORT_I, 11),
        .ulpi_stp = GPIO_PIN(PORT_C, 0),
        .ulpi_nxt = GPIO_PIN(PORT_H, 4),
    }
};

/**
 * @brief   Number of available USB OTG peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(dwc2_usb_otg_fshs_config)
/** @} */
#endif /* defined(MODULE_PERIPH_USBDEV_HS_ULPI) || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
