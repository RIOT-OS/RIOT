/*
 * Copyright (C) 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

//------------------------------------------------------------------------------
//  Module      : S5P4418 CHIP
//  File        : s5p4418_chip.h
//  Description : S5P4418 peripheral base address
//  Author      : Dang Minh Phuong
//  History     :
//------------------------------------------------------------------------------

#ifndef __S5P4418_CHIP_H__
#define __S5P4418_CHIP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * S5P4418 clock generator
 */
#define CLK_ID_TIMER_1          0
#define CLK_ID_TIMER_2          1
#define CLK_ID_TIMER_3          2
#define CLK_ID_PWM_1            3
#define CLK_ID_PWM_2            4
#define CLK_ID_PWM_3            5
#define CLK_ID_I2C_0            6
#define CLK_ID_I2C_1            7
#define CLK_ID_I2C_2            8
#define CLK_ID_MIPI             9
#define CLK_ID_GMAC             10  /* External Clock 1 */
#define CLK_ID_SPDIF_TX         11
#define CLK_ID_MPEGTSI          12
#define CLK_ID_PWM_0            13
#define CLK_ID_TIMER_0          14
#define CLK_ID_I2S_0            15  /* External Clock 1 */
#define CLK_ID_I2S_1            16  /* External Clock 1 */
#define CLK_ID_I2S_2            17  /* External Clock 1 */
#define CLK_ID_SDHC_0           18
#define CLK_ID_SDHC_1           19
#define CLK_ID_SDHC_2           20
#define CLK_ID_MALI             21
#define CLK_ID_UART_0           22  /* UART0_MODULE */
#define CLK_ID_UART_2           23  /* UART1_MODULE */
#define CLK_ID_UART_1           24  /* pl01115_Uart_modem_MODULE  */
#define CLK_ID_UART_3           25  /* pl01115_Uart_nodma0_MODULE */
#define CLK_ID_UART_4           26  /* pl01115_Uart_nodma1_MODULE */
#define CLK_ID_UART_5           27  /* pl01115_Uart_nodma2_MODULE */
#define CLK_ID_DIT              28
#define CLK_ID_PPM              29
#define CLK_ID_VIP_0            30  /* External Clock 1 */
#define CLK_ID_VIP_1            31  /* External Clock 1, 2 */
#define CLK_ID_USB2HOST         32  /* External Clock 2 */
#define CLK_ID_CODA             33
#define CLK_ID_CRYPTO           34
#define CLK_ID_SCALER           35
#define CLK_ID_PDM              36
#define CLK_ID_SPI_0            37
#define CLK_ID_SPI_1            38
#define CLK_ID_SPI_2            39
#define CLK_ID_MAX              39

/*
 * device name
 */
#define DEV_NAME_UART           "s5p4418-uart"
#define DEV_NAME_FB             "s5p4418-fb"
#define DEV_NAME_DISP           "s5p4418-disp"
#define DEV_NAME_LCD            "s5p4418-lcd"
#define DEV_NAME_LVDS           "s5p4418-lvds"
#define DEV_NAME_HDMI           "s5p4418-hdmi"
#define DEV_NAME_RESCONV        "s5p4418-resconv"
#define DEV_NAME_MIPI           "s5p4418-mipi"
#define DEV_NAME_PCM            "s5p4418-pcm"
#define DEV_NAME_I2S            "s5p4418-i2s"
#define DEV_NAME_SPDIF_TX       "s5p4418-spdif-tx"
#define DEV_NAME_SPDIF_RX       "s5p4418-spdif-rx"
#define DEV_NAME_I2C            "s5p4418-i2c"
#define DEV_NAME_NAND           "s5p4418-nand"
#define DEV_NAME_KEYPAD         "s5p4418-keypad"
#define DEV_NAME_SDHC           "s5p4418-sdhc"
#define DEV_NAME_PWM            "s5p4418-pwm"
#define DEV_NAME_TIMER          "s5p4418-timer"
#define DEV_NAME_SOC_PWM        "s5p4418-soc-pwm"
#define DEV_NAME_GPIO           "s5p4418-gpio"
#define DEV_NAME_RTC            "s5p4418-rtc"
#define DEV_NAME_GMAC           "s5p4418-gmac"
#define DEV_NAME_MPEGTSI        "s5p4418-mpegtsi"
#define DEV_NAME_MALI           "s5p4418-mali"
#define DEV_NAME_DIT            "s5p4418-deinterlace"
#define DEV_NAME_PPM            "s5p4418-ppm"
#define DEV_NAME_VIP            "s5p4418-vip"
#define DEV_NAME_CODA           "s5p4418-coda"
#define DEV_NAME_USB2HOST       "s5p4418-usb2h"
#define DEV_NAME_CRYPTO         "s5p4418-crypto"
#define DEV_NAME_SCALER         "s5p4418-scaler"
#define DEV_NAME_PDM            "s5p4418-pdm"
#define DEV_NAME_SPI            "s5p4418-spi"
#define DEV_NAME_CPUFREQ        "s5p4418-cpufreq"

/*
 * clock generator
 */
#define CORECLK_NAME_PLL0       "pll0"  /* cpu clock */
#define CORECLK_NAME_PLL1       "pll1"
#define CORECLK_NAME_PLL2       "pll2"
#define CORECLK_NAME_PLL3       "pll3"
#define CORECLK_NAME_FCLK       "fclk"
#define CORECLK_NAME_MCLK       "mclk"
#define CORECLK_NAME_BCLK       "bclk"
#define CORECLK_NAME_PCLK       "pclk"
#define CORECLK_NAME_HCLK       "hclk"

#define CORECLK_ID_PLL0         (0)
#define CORECLK_ID_PLL1         (1)
#define CORECLK_ID_PLL2         (2)
#define CORECLK_ID_PLL3         (3)
#define CORECLK_ID_FCLK         (4)
#define CORECLK_ID_MCLK         (5)
#define CORECLK_ID_BCLK         (6)
#define CORECLK_ID_PCLK         (7)
#define CORECLK_ID_HCLK         (8)

/*
 * PHY base address
 */
#define PHY_BASEADDR_DMA0           (0xC0000000)
#define PHY_BASEADDR_DMA1           (0xC0001000)
#define PHY_BASEADDR_INTC0          (0xC0002000)
#define PHY_BASEADDR_INTC1          (0xC0003000)
#define PHY_BASEADDR_ALIVE          (0xC0010800)
#define PHY_BASEADDR_RSTCON         (0xC0012000)
#define PHY_BASEADDR_TIEOFF         (0xC0011000)
#define PHY_BASEADDR_PDM            (0xC0014000)
#define PHY_BASEADDR_CRYPTO         (0xC0015000)
#define PHY_BASEADDR_TIMER          (0xC0017000)
#define PHY_BASEADDR_PWM            (0xC0018000)
#define PHY_BASEADDR_WDT            (0xC0019000)
#define PHY_BASEADDR_GPIOA          (0xC001A000)
#define PHY_BASEADDR_GPIOB          (0xC001B000)
#define PHY_BASEADDR_GPIOC          (0xC001C000)
#define PHY_BASEADDR_GPIOD          (0xC001D000)
#define PHY_BASEADDR_GPIOE          (0xC001E000)
#define PHY_BASEADDR_ADC            (0xC0053000)
#define PHY_BASEADDR_PPM            (0xC0054000)
#define PHY_BASEADDR_I2S0           (0xC0055000)
#define PHY_BASEADDR_I2S1           (0xC0056000)
#define PHY_BASEADDR_I2S2           (0xC0057000)
#define PHY_BASEADDR_AC97           (0xC0058000)
#define PHY_BASEADDR_SPDIF_TX       (0xC0059000)
#define PHY_BASEADDR_SPDIF_RX       (0xC005A000)
#define PHY_BASEADDR_SSP0           (0xC005B000)
#define PHY_BASEADDR_SSP1           (0xC005C000)
#define PHY_BASEADDR_SSP2           (0xC005F000)
#define PHY_BASEADDR_MPEGTSI        (0xC005D000)
#define PHY_BASEADDR_GMAC           (0xC0060000)
#define PHY_BASEADDR_VIP0           (0xC0063000)
#define PHY_BASEADDR_VIP1           (0xC0064000)
#define PHY_BASEADDR_DEINTERLACE    (0xC0065000)
#define PHY_BASEADDR_SCALER         (0xC0066000)
#define PHY_BASEADDR_ECID           (0xC0067000)
#define PHY_BASEADDR_SDMMC0         (0xC0062000)
#define PHY_BASEADDR_SDMMC1         (0xC0068000)
#define PHY_BASEADDR_SDMMC2         (0xC0069000)
#define PHY_BASEADDR_MALI400        (0xC0070000)
#define PHY_BASEADDR_CODA_APB0      (0xC0080000)
#define PHY_BASEADDR_CODA_APB1      (0xC0081000)
#define PHY_BASEADDR_CODA_APB2      (0xC0082000)
#define PHY_BASEADDR_CODA_APB3      (0xC0083000)
#define PHY_BASEADDR_UART0          (0xC00A1000)    // dma (O), modem(X), UART0_MODULE
#define PHY_BASEADDR_UART1          (0xC00A0000)    // dma (O), modem(O), pl01115_Uart_modem_MODULE
#define PHY_BASEADDR_UART2          (0xC00A2000)    // dma (O), modem(X), UART1_MODULE
#define PHY_BASEADDR_UART3          (0xC00A3000)    // dma (X), modem(X), pl01115_Uart_nodma0_MODULE
#define PHY_BASEADDR_UART4          (0xC006D000)    // dma (X), modem(X), pl01115_Uart_nodma1_MODULE
#define PHY_BASEADDR_UART5          (0xC006F000)    // dma (X), modem(X), pl01115_Uart_nodma2_MODULE
#define PHY_BASEADDR_I2C0           (0xC00A4000)
#define PHY_BASEADDR_I2C1           (0xC00A5000)
#define PHY_BASEADDR_I2C2           (0xC00A6000)
#define PHY_BASEADDR_CAN0           (0xC00CE000)
#define PHY_BASEADDR_CAN1           (0xC00CF000)
#define PHY_BASEADDR_MIPI           (0xC00D0000)
#define PHY_BASEADDR_DISPLAYTOP     (0xC0100000)
#define PHY_BASEADDR_CLKPWR_MODULE  (0xC0010000)

#define PHY_BASEADDR_CLKGEN0        (0xC00BB000)    // TIMER_1
#define PHY_BASEADDR_CLKGEN1        (0xC00BC000)    // TIMER_2
#define PHY_BASEADDR_CLKGEN2        (0xC00BD000)    // TIMER_3
#define PHY_BASEADDR_CLKGEN3        (0xC00BE000)    // PWM_1
#define PHY_BASEADDR_CLKGEN4        (0xC00BF000)    // PWM_2
#define PHY_BASEADDR_CLKGEN5        (0xC00C0000)    // PWM_3
#define PHY_BASEADDR_CLKGEN6        (0xC00AE000)    // I2C_0
#define PHY_BASEADDR_CLKGEN7        (0xC00AF000)    // I2C_1
#define PHY_BASEADDR_CLKGEN8        (0xC00B0000)    // I2C_2
#define PHY_BASEADDR_CLKGEN9        (0xC00CA000)    // MIPI
#define PHY_BASEADDR_CLKGEN10       (0xC00C8000)    // GMAC
#define PHY_BASEADDR_CLKGEN11       (0xC00B8000)    // SPDIF_TX
#define PHY_BASEADDR_CLKGEN12       (0xC00B7000)    // MPEGTSI
#define PHY_BASEADDR_CLKGEN13       (0xC00BA000)    // PWM_0
#define PHY_BASEADDR_CLKGEN14       (0xC00B9000)    // TIMER_0
#define PHY_BASEADDR_CLKGEN15       (0xC00B2000)    // I2S_0
#define PHY_BASEADDR_CLKGEN16       (0xC00B3000)    // I2S_1
#define PHY_BASEADDR_CLKGEN17       (0xC00B4000)    // I2S_2
#define PHY_BASEADDR_CLKGEN18       (0xC00C5000)    // SDHC_0
#define PHY_BASEADDR_CLKGEN19       (0xC00CC000)    // SDHC_1
#define PHY_BASEADDR_CLKGEN20       (0xC00CD000)    // SDHC_2
#define PHY_BASEADDR_CLKGEN21       (0xC00C3000)    // MALI
#define PHY_BASEADDR_CLKGEN22       (0xC00A9000)    // UART_0
#define PHY_BASEADDR_CLKGEN23       (0xC00AA000)    // UART_2
#define PHY_BASEADDR_CLKGEN24       (0xC00A8000)    // UART_1
#define PHY_BASEADDR_CLKGEN25       (0xC00AB000)    // UART_3
#define PHY_BASEADDR_CLKGEN26       (0xC006E000)    // UART_4
#define PHY_BASEADDR_CLKGEN27       (0xC0084000)    // UART_5
#define PHY_BASEADDR_CLKGEN28       (0xC00B5000)    // DEINTERLACE
#define PHY_BASEADDR_CLKGEN29       (0xC00C4000)    // PPM
#define PHY_BASEADDR_CLKGEN30       (0xC00C1000)    // VIP_0
#define PHY_BASEADDR_CLKGEN31       (0xC00C2000)    // VIP_1
#define PHY_BASEADDR_CLKGEN32       (0xC006B000)    // USB2HOST
#define PHY_BASEADDR_CLKGEN33       (0xC00C7000)    // CODA
#define PHY_BASEADDR_CLKGEN34       (0xC00C6000)    // CRYPTO
#define PHY_BASEADDR_CLKGEN35       (0xC00B6000)    // SCALER
#define PHY_BASEADDR_CLKGEN36       (0xC00CB000)    // PDM
#define PHY_BASEADDR_CLKGEN37       (0xC00AC000)    // SPI0
#define PHY_BASEADDR_CLKGEN38       (0xC00AD000)    // SPI1
#define PHY_BASEADDR_CLKGEN39       (0xC00A7000)    // SPI2
#define PHY_BASEADDR_CS_NAND        (0x2C000000)    // ????

// VIC base address
#define  S5P4418_VIC0_BASE          (0xC0002000)
#define  S5P4418_VIC1_BASE          (0xC0003000)

// UART base address
#define S5P4418_UART0_BASE          (0xC00A1000)
#define S5P4418_UART1_BASE          (0xC00A0000)
#define S5P4418_UART2_BASE          (0xC00A2000)
#define S5P4418_UART3_BASE          (0xC00A3000)

// TIMER base address
#define S5P4418_TIMER_BASE          (0xC0017000)

// GPIO base address
#define S5P4418_GPIOA_BASE          (0xC001A000)
#define S5P4418_GPIOB_BASE          (0xC001B000)
#define S5P4418_GPIOC_BASE          (0xC001C000)
#define S5P4418_GPIOD_BASE          (0xC001D000)
#define S5P4418_GPIOE_BASE          (0xC001E000)

// SPI/SSP base address
#define S5P4418_SSP0_BASE           (0xC005B000)
#define S5P4418_SSP1_BASE           (0xC005C000)
#define S5P4418_SSP2_BASE           (0xC005F000)

// IP Reset base address
#define S5P4418_RSTCON_BASE         (0xC0012000)

#ifdef __cplusplus
}
#endif

#endif // __S5P4418_CHIP_H__
