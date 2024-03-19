/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f723e-disco
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F723E-DISCO board
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

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#if defined(MODULE_PERIPH_USBDEV_HS_UTMI)
#include "cfg_usb_otg_hs_phy_utmi.h"
#else
#include "cfg_usb_otg_fs.h"
#endif
#include "lcd_fmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_C, 6),
        .tx_pin     = GPIO_PIN(PORT_C, 7),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
    { /* Arduino connector (D0/D1) */
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
    { /* PMOD connector */
        .dev        = UART7,
        .rcc_mask   = RCC_APB1ENR_UART7EN,
        .rx_pin     = GPIO_PIN(PORT_F, 6),
        .tx_pin     = GPIO_PIN(PORT_F, 7),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB1,
        .irqn       = UART7_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_PIN(PORT_F, 9),
        .rts_pin    = GPIO_PIN(PORT_F, 8),
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
    { /* ESP-01 connector */
        .dev        = UART5,
        .rcc_mask   = RCC_APB1ENR_UART5EN,
        .rx_pin     = GPIO_PIN(PORT_D, 2),
        .tx_pin     = GPIO_PIN(PORT_C, 12),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB1,
        .irqn       = UART5_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
};

#define UART_0_ISR          (isr_usart6)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_uart4)
#define UART_3_ISR          (isr_uart7)
#define UART_4_ISR          (isr_uart5)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    { /* Shared between Arduino D14/D15 and STMOD+ connector */
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_H, 4),
        .sda_pin        = GPIO_PIN(PORT_H, 5),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .rcc_sw_mask    = RCC_DCKCFGR2_I2C2SEL_1,
        .irqn           = I2C2_ER_IRQn,
    },
    {   /* Connected to touchscreen controller */
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 8),
        .sda_pin        = GPIO_PIN(PORT_H, 8),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .rcc_sw_mask    = RCC_DCKCFGR2_I2C3SEL_1,
        .irqn           = I2C3_ER_IRQn,
    },
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
};

#define I2C_0_ISR           isr_i2c2_er
#define I2C_1_ISR           isr_i2c3_er
#define I2C_2_ISR           isr_i2c1_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    { /* Arduino connector */
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    { /* PMOD connector */
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_I, 3),
        .miso_pin = GPIO_PIN(PORT_I, 2),
        .sclk_pin = GPIO_PIN(PORT_I, 1),
        .cs_pin   = GPIO_PIN(PORT_I, 0),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
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
#if MODULE_PERIPH_FMC_NOR_SRAM
    .ne1_pin = { .pin = GPIO_PIN(PORT_D, 7), .af = GPIO_AF12, }, /* PSRAM_NE1 signal, subbank 1 */
    .ne2_pin = { .pin = GPIO_PIN(PORT_G, 9), .af = GPIO_AF12, }, /* LCD_NE signal, subbank 2 */
    .noe_pin = { .pin = GPIO_PIN(PORT_D, 4), .af = GPIO_AF12, }, /* LCD_PSRAM_NOE */
    .nwe_pin = { .pin = GPIO_PIN(PORT_D, 5), .af = GPIO_AF12, }, /* LCD_PSRAM_NWE signal */
    .addr = {
        { .pin = GPIO_PIN(PORT_F, 0), .af = GPIO_AF12, },  /* PSRAM_A0 / LCD_RS signal */
        { .pin = GPIO_PIN(PORT_F, 1), .af = GPIO_AF12, },  /* PSRAM_A1 signal */
        { .pin = GPIO_PIN(PORT_F, 2), .af = GPIO_AF12, },  /* PSRAM_A2 signal */
        { .pin = GPIO_PIN(PORT_F, 3), .af = GPIO_AF12, },  /* PSRAM_A3 signal */
        { .pin = GPIO_PIN(PORT_F, 4), .af = GPIO_AF12, },  /* PSRAM_A4 signal */
        { .pin = GPIO_PIN(PORT_F, 5), .af = GPIO_AF12, },  /* PSRAM_A5 signal */
        { .pin = GPIO_PIN(PORT_F, 12), .af = GPIO_AF12, }, /* PSRAM_A6 signal */
        { .pin = GPIO_PIN(PORT_F, 13), .af = GPIO_AF12, }, /* PSRAM_A7 signal */
        { .pin = GPIO_PIN(PORT_F, 14), .af = GPIO_AF12, }, /* PSRAM_A8 signal */
        { .pin = GPIO_PIN(PORT_F, 15), .af = GPIO_AF12, }, /* PSRAM_A9 signal */
        { .pin = GPIO_PIN(PORT_G, 0), .af = GPIO_AF12, },  /* PSRAM_A10 signal */
        { .pin = GPIO_PIN(PORT_G, 1), .af = GPIO_AF12, },  /* PSRAM_A11 signal */
        { .pin = GPIO_PIN(PORT_G, 2), .af = GPIO_AF12, },  /* PSRAM_A12 signal */
        { .pin = GPIO_PIN(PORT_G, 3), .af = GPIO_AF12, },  /* PSRAM_A13 signal */
        { .pin = GPIO_PIN(PORT_G, 4), .af = GPIO_AF12, },  /* PSRAM_A14 signal */
        { .pin = GPIO_PIN(PORT_G, 5), .af = GPIO_AF12, },  /* PSRAM_A15 signal */
        { .pin = GPIO_PIN(PORT_D, 11), .af = GPIO_AF12, }, /* PSRAM_A16 signal */
        { .pin = GPIO_PIN(PORT_D, 12), .af = GPIO_AF12, }, /* PSRAM_A17 signal */
    },
#endif
    .data = {
        { .pin = GPIO_PIN(PORT_D, 14), .af = GPIO_AF12, }, /* LCD_PSRAM_D0 signal */
        { .pin = GPIO_PIN(PORT_D, 15), .af = GPIO_AF12, }, /* LCD_PSRAM_D1 signal */
        { .pin = GPIO_PIN(PORT_D, 0), .af = GPIO_AF12, },  /* LCD_PSRAM_D2 signal */
        { .pin = GPIO_PIN(PORT_D, 1), .af = GPIO_AF12, },  /* LCD_PSRAM_D3 signal */
        { .pin = GPIO_PIN(PORT_E, 7), .af = GPIO_AF12, },  /* LCD_PSRAM_D4 signal */
        { .pin = GPIO_PIN(PORT_E, 8), .af = GPIO_AF12, },  /* LCD_PSRAM_D5 signal */
        { .pin = GPIO_PIN(PORT_E, 9), .af = GPIO_AF12, },  /* LCD_PSRAM_D6 signal */
        { .pin = GPIO_PIN(PORT_E, 10), .af = GPIO_AF12, }, /* LCD_PSRAM_D7 signal */
#if MODULE_PERIPH_FMC_16BIT
        { .pin = GPIO_PIN(PORT_E, 11), .af = GPIO_AF12, }, /* LCD_PSRAM_D8 signal */
        { .pin = GPIO_PIN(PORT_E, 12), .af = GPIO_AF12, }, /* LCD_PSRAM_D9 signal */
        { .pin = GPIO_PIN(PORT_E, 13), .af = GPIO_AF12, }, /* LCD_PSRAM_D10 signal */
        { .pin = GPIO_PIN(PORT_E, 14), .af = GPIO_AF12, }, /* LCD_PSRAM_D11 signal */
        { .pin = GPIO_PIN(PORT_E, 15), .af = GPIO_AF12, }, /* LCD_PSRAM_D12 signal */
        { .pin = GPIO_PIN(PORT_D, 8), .af = GPIO_AF12, },  /* LCD_PSRAM_D13 signal */
        { .pin = GPIO_PIN(PORT_D, 9), .af = GPIO_AF12, },  /* LCD_PSRAM_D14 signal */
        { .pin = GPIO_PIN(PORT_D, 10), .af = GPIO_AF12, }, /* LCD_PSRAM_D15 signal */
#endif
    },
    .nbl0_pin = { .pin = GPIO_PIN(PORT_E, 0), .af = GPIO_AF12, }, /* PSRAM_NBL0 signal (LB) */
    .nbl1_pin = { .pin = GPIO_PIN(PORT_E, 1), .af = GPIO_AF12, }, /* PSRAM_NBL1 signal (UB) */
};

/**
 * @brief   FMC Bank configuration
 *
 * The board has a PSRAM IS66WV51216EBLL-55BLI with  MBit on-board.
 * It is organized in 512K x 16 bits and connected to bank 1, subbank 1
 * at address 0x60000000.
 *
 * @note A18 of the PSRAM is not used. Therefore, only 256K x 16 bits
 *       (512 kByte) of the 1 MByte PSRAM can be used.
 *
 * The LCD display of the board is connected to bank 1, subbank2
 * at address 0x64000000.
 */
static const fmc_bank_conf_t fmc_bank_config[] = {
    /* bank 1, subbank 1 is used for PSRAM with asynchronuous
     * access in Mode 1, i.e. write timings are not used */
    {
        .bank = FMC_BANK_1,
        .mem_type = FMC_SRAM,
        .data_width = FMC_BUS_WIDTH_16BIT,
        .address = 0x60000000,  /* Bank 1, subbank 1 is mapped to 0x60000000 */
        .size = KiB(512),       /* Size in byte, 256K x 16 bit */
        .nor_sram = {
            .sub_bank = 1,
            .ext_mode = false,                      /* Mode 1 used, no separate w_timing */
                                                    /* timings for IS66WV51216EBLL-55BLI
                                                       @216 MHz AHB clock */
            .r_timing = {  .addr_setup = 13,        /* t_AA = max 60 ns (13 HCLKs a 4.63 ns) */
                           .data_setup = 6,         /* t_SD = min 25 ns (6 HCLKs a 4.63 ns) */
                           .bus_turnaround = 3, },  /* 3 HCLKs a 4.63 ns */
        },
    },
    /* bank 1, subbank 2 is used for LCD with asynchronuous
     * access in Mode 1, i.e. write timings are not used */
    {
        .bank = FMC_BANK_1,
        .mem_type = FMC_SRAM,
        .data_width = FMC_BUS_WIDTH_16BIT,
        .address = 0x64000000,  /* Bank 1, subbank 2 is mapped to 0x64000000 */
        .size = 4,              /* 1 word for command @ 0x64000000 and
                                     1 word for data @ 0x64000001 */
        .nor_sram = {
            .sub_bank = 2,
            .ext_mode = false,                      /* Mode 1 used, no separate w_timing */
                                                    /* timing requirements for ST7789H2:
                                                       - t_AST min 0 ns (Address setup time)
                                                       - t_DST min 10 ns (Data setup time)
                                                       - t_WRL min 15 ns (WE LOW time)
                                                       - t_WRH min 15 ns (WE HIGH time)
                                                       - t_WRC min 66 ns (WE cycle time) */
            .r_timing = {  .addr_setup = 2,         /* t_AST = 10 ns (2 HCLKs a 4.63 ns) */
                           .data_setup = 8,         /* t_DST = 37 ns (8 HCLKs a 4.63 ns) */
                           .bus_turnaround = 5, },  /* t_WRH = 23 ns (5 HCLKs a 4.63 ns) */
        },
    },
};

/**
 * @brief   Number of configured FMC banks
 */
#define FMC_BANK_NUMOF  ARRAY_SIZE(fmc_bank_config)
/** @} */

/**
 * @brief   Descriptors of FMC banks used for LCDs
 */
static const lcd_fmc_desc_t lcd_fmc_desc[] = {
    {
        .bank = FMC_BANK_CONFIG(1), /* second bank (fmc_bank_config[1]) is used */
        .cmd_offset = 0x0,          /* address 0x64000000 (offset 0x0) used for commands */
        .data_offset = 0x2,         /* address 0x64000002 (offset 0x2) used for commands */
    }
};

/**
 * @brief   Number of LCDs using FMC banks
 *
 * Because it is used by the preprocessor it has to be a number.
 * The @ref ARRAY_SIZE can't be used here.
 */
#define LCD_FMC_NUMOF  1

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
