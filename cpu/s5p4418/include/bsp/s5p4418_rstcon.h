/*
 * Copyright (C) 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Phuong Dang <kamejoko80@yahoo.com>
 */

#ifndef __S5P4418_RSTCON_H__
#define __S5P4418_RSTCON_H__

#include <stdint.h>
#include <stdbool.h>
#include "s5p4418_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief S5P4418 Reset control IDs
 */
#define RESET_ID_AC97           0
#define RESET_ID_CPU1           1
#define RESET_ID_CPU2           2
#define RESET_ID_CPU3           3
#define RESET_ID_WD1            4
#define RESET_ID_WD2            5
#define RESET_ID_WD3            6
#define RESET_ID_CRYPTO         7
#define RESET_ID_DEINTERLACE    8
#define RESET_ID_DISP_TOP       9
#define RESET_ID_DISPLAY        10
#define RESET_ID_RESCONV        11
#define RESET_ID_LCDIF          12
#define RESET_ID_HDMI           13
#define RESET_ID_HDMI_VIDEO     14
#define RESET_ID_HDMI_SPDIF     15
#define RESET_ID_HDMI_TMDS      16
#define RESET_ID_HDMI_PHY       17
#define RESET_ID_LVDS           18
#define RESET_ID_ECID           19
#define RESET_ID_I2C0           20
#define RESET_ID_I2C1           21
#define RESET_ID_I2C2           22
#define RESET_ID_I2S0           23
#define RESET_ID_I2S1           24
#define RESET_ID_I2S2           25
#define RESET_ID_DREX_C         26
#define RESET_ID_DREX_A         27
#define RESET_ID_DREX           28
#define RESET_ID_MIPI           29
#define RESET_ID_MIPI_DSI       30
#define RESET_ID_MIPI_CSI       31
#define RESET_ID_MIPI_PHY_S     32
#define RESET_ID_MIPI_PHY_M     33
#define RESET_ID_MPEGTSI        34
#define RESET_ID_PDM            35
#define RESET_ID_TIMER          36
#define RESET_ID_PWM            37
#define RESET_ID_SCALER         38
#define RESET_ID_SDMMC0         39
#define RESET_ID_SDMMC1         40
#define RESET_ID_SDMMC2         41
#define RESET_ID_SPDIFRX        42
#define RESET_ID_SPDIFTX        43
#define RESET_ID_SSP0_P         44
#define RESET_ID_SSP0           45
#define RESET_ID_SSP1_P         46
#define RESET_ID_SSP1           47
#define RESET_ID_SSP2_P         48
#define RESET_ID_SSP2           49
#define RESET_ID_UART0          50
#define RESET_ID_UART1          51
#define RESET_ID_UART2          52
#define RESET_ID_UART3          53
#define RESET_ID_UART4          54
#define RESET_ID_UART5          55
#define RESET_ID_USB20HOST      56
#define RESET_ID_USB20OTG       57
#define RESET_ID_WDT            58
#define RESET_ID_WDT_POR        59
#define RESET_ID_ADC            60
#define RESET_ID_CODA_A         61
#define RESET_ID_CODA_P         62
#define RESET_ID_CODA_C         63
#define RESET_ID_DWC_GMAC       64
#define RESET_ID_MALI400        65
#define RESET_ID_PPM            66
#define RESET_ID_VIP1           67
#define RESET_ID_VIP0           68

/**
 * @brief Reset control register set
 */
typedef struct {
    volatile uint32_t IP_RESET_REGISTER0;   /**< 0x000 IP RESET REGISTER 0 */
    volatile uint32_t IP_RESET_REGISTER1;   /**< 0x004 IP RESET REGISTER 1 */
    volatile uint32_t IP_RESET_REGISTER2;   /**< 0x008 IP RESET REGISTER 2 */
}rstcon_typedef;

/**
 * @brief GPIO bank register base definition
 */
#define RSTCON ((rstcon_typedef *) S5P4418_RSTCON_BASE)

void s5p4418_ip_reset(int id);

#ifdef __cplusplus
}
#endif

#endif
