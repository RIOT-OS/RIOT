/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f746g-disco
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for STM32F746G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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

#include <stdint.h>

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#if defined(MODULE_PERIPH_USBDEV_HS_ULPI)
#include "usbdev_synopsys_dwc2.h"
#else
#include "cfg_usb_otg_fs.h"
#endif
#include "mii.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 15 },   /* DMA2 Stream 7 - USART1_TX */
    { .stream = 14 },   /* DMA2 Stream 6 - USART6_TX */
    { .stream = 6 },    /* DMA1 Stream 6 - USART2_TX */
    { .stream = 3 },    /* DMA1 Stream 3 - SPI2_RX   */
    { .stream = 4 },    /* DMA1 Stream 4 - SPI2_TX   */
    { .stream = 11 },   /* DMA2 Stream 3 - SPI4_RX Ch5 / SDMMC1 Ch 4 */
    { .stream = 12 },   /* DMA2 Stream 4 - SPI4_TX   */
    { .stream = 8 },    /* DMA2 Stream 0 - ETH_TX    */
};

#define DMA_0_ISR  isr_dma2_stream7
#define DMA_1_ISR  isr_dma2_stream6
#define DMA_2_ISR  isr_dma1_stream6

#define DMA_3_ISR  isr_dma2_stream2
#define DMA_4_ISR  isr_dma2_stream5
#define DMA_5_ISR  isr_dma2_stream3
#define DMA_6_ISR  isr_dma2_stream4

#define DMA_7_ISR  isr_dma2_stream0

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
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = 0,
        .dma_chan   = 4
#endif
    },
    {  /* Arduino connectors */
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_C, 6),
        .tx_pin     = GPIO_PIN(PORT_C, 7),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = 1,
        .dma_chan   = 5
#endif
    },
};

#define UART_0_ISR          (isr_usart1)
#define UART_0_DMA_ISR      (isr_dma2_stream7)
#define UART_6_ISR          (isr_usart6)
#define UART_6_DMA_ISR      (isr_dma2_stream6)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_I, 1),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 4,
        .tx_dma_chan = 0,
        .rx_dma   = 3,
        .rx_dma_chan = 0,
#endif
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .rcc_sw_mask    = RCC_DCKCFGR2_I2C1SEL_1,
        .irqn           = I2C1_ER_IRQn,
    },
    {
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_H, 7),
        .sda_pin        = GPIO_PIN(PORT_H, 8),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .rcc_sw_mask    = RCC_DCKCFGR2_I2C3SEL_1,
        .irqn           = I2C3_ER_IRQn,
    },
};

#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c3_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name ETH configuration
 * @{
 */
static const eth_conf_t eth_config = {
    .mode = RMII,
    .speed = MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX,
    .dma = 7,
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

#define ETH_DMA_ISR        isr_dma2_stream0
/** @} */

/**
 * @name LTDC configuration
 * @{
 */
/** LTDC static configuration struct */
static const ltdc_conf_t ltdc_config = {
    .bus        = APB2,
    .rcc_mask   = RCC_APB2ENR_LTDCEN,
    .clk_pin    = { .pin = GPIO_PIN(PORT_I, 14), .af = GPIO_AF14, },
    .de_pin     = { .pin = GPIO_PIN(PORT_K, 7), .af = GPIO_AF14, },
    .hsync_pin  = { .pin = GPIO_PIN(PORT_I, 10), .af = GPIO_AF14, },
    .vsync_pin  = { .pin = GPIO_PIN(PORT_I, 9), .af = GPIO_AF14, },
    .r_pin      = {
        { .pin = GPIO_PIN(PORT_I, 15), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 0), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 1), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 2), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 3), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 4), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 5), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 6), .af = GPIO_AF14, },
    },
    .g_pin      = {
        { .pin = GPIO_PIN(PORT_J, 7), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 8), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 9), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 10), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 11), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 0), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 1), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 2), .af = GPIO_AF14, },
    },
    .b_pin      = {
        { .pin = GPIO_PIN(PORT_E, 4), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 13), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 14), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 15), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_G, 12), .af = GPIO_AF9, },
        { .pin = GPIO_PIN(PORT_K, 4), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 5), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 6), .af = GPIO_AF14, },
    },
    /* values below come from STM32CubeF7 code and differ from the typical
     * values mentioned in the RK043FN48H datasheet. Both sets of values work
     * with the display.
     * See the discussion in
     * https://community.st.com/s/question/0D50X0000BOvdWP/how-to-set-displays-parameters-
     */
    .hsync      = 41,
    .vsync      = 10,
    .hbp        = 13,
    .hfp        = 32,
    .vbp        = 2,
    .vfp        = 2,
};
/** @} */

#if defined(MODULE_PERIPH_USBDEV_HS_ULPI) || DOXYGEN
/**
 * @name USB OTG FS configuration using ULPI HS PHY
 *
 * The USB OTG HS peripheral uses a ULPI HS PHY. The configuration of the
 * ULPI HS PHY interface is board-specific.
 *
 * @{
 */

/**
 * @brief Enable the high speed USB OTG peripheral
 */
#define DWC2_USB_OTG_HS_ENABLED

/**
 * @brief Common USB OTG HS configuration with ULPI HS PHY
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
        .ulpi_dir = GPIO_PIN(PORT_C, 2),
        .ulpi_stp = GPIO_PIN(PORT_C, 0),
        .ulpi_nxt = GPIO_PIN(PORT_H, 4),
    }
};

/**
 * @brief Number of available USB OTG peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(dwc2_usb_otg_fshs_config)

/** @} */
#endif /* defined(MODULE_PERIPH_USBDEV_HS_ULPI) || DOXYGEN */

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
    .sdcke0_pin = { .pin = GPIO_PIN(PORT_C, 3), .af = GPIO_AF12, },  /* FMC_SDCKE0 signal */
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
 * The board has a SDRAM IS42S32400F-6BL with 128 MBit on-board.
 * It is organized in 4 banks of 1M x 32 bits each and connected to bank 5
 * at address 0xc0000000.
 *
 * @note Since only D0 to D15 are connected and D16 to D31 are unused, 4 banks
 *       with only 1M x 16 bits and thus half the capacity (8 MByte) can be
 *       used.
 */
static const fmc_bank_conf_t fmc_bank_config[] = {
    /* bank 5 is used for SDRAM */
    {
        .bank = FMC_BANK_5,
        .mem_type = FMC_SDRAM,
        .data_width = FMC_BUS_WIDTH_16BIT,
        .address = 0xc0000000,               /* Bank 5 is mapped to 0xc0000000 */
        .size = MiB(8),                      /* Size in MByte, 4M x 16 Bit */
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

/**
 * @name SDIO/SDMMC configuration
 * @{
 */

/**
 * @brief SDIO/SDMMC static configuration struct
 */
static const sdmmc_conf_t sdmmc_config[] = {
    {
        .dev = SDMMC1,
        .bus = APB2,
        .rcc_mask = RCC_APB2ENR_SDMMC1EN,
        .cd = GPIO_PIN(PORT_C, 13),
        .cd_active = 0,             /* CD pin is LOW active */
        .cd_mode = GPIO_IN_PU,      /* Pull-up R12 not soldered by default */
        .clk = { GPIO_PIN(PORT_C, 12), GPIO_AF12 },
        .cmd = { GPIO_PIN(PORT_D,  2), GPIO_AF12 },
        .dat0 = { GPIO_PIN(PORT_C,  8), GPIO_AF12 },
        .dat1 = { GPIO_PIN(PORT_C,  9), GPIO_AF12 },
        .dat2 = { GPIO_PIN(PORT_C, 10), GPIO_AF12 },
        .dat3 = { GPIO_PIN(PORT_C, 11), GPIO_AF12 },
#ifdef MODULE_PERIPH_DMA
        .dma = 5,
        .dma_chan = 4,
#endif
        .irqn = SDMMC1_IRQn
    },
};

/**
 * @brief Number of configured SDIO/SDMMC peripherals
 */
#define SDMMC_CONFIG_NUMOF  1

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
