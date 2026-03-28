/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_imx8mq-cm4
 * @brief           Implementation specific clocks configuration options
 * @{
 *
 * @file
 * @brief           Implementation specific clocks configuration options
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef CLK_CONF_H
#define CLK_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu.h"

#define CLK_ENABLE_ALL_DOMAINS 0x3333

/* table 5-1 from datasheet */
typedef enum {
    CCM_ROOT_CLK_CA53 = 0,
    CCM_ROOT_CLK_CM4 = 1,
    CCM_ROOT_CLK_CA53_VPU = 2,
    CCM_ROOT_CLK_GPU_CORE = 3,
    CCM_ROOT_CLK_SHADER = 4,
    CCM_ROOT_CLK_MAIN_AXI = 16,
    CCM_ROOT_CLK_ENET_AXI = 17,
    CCM_ROOT_CLK_NAND_USDHC_BUS = 18,
    CCM_ROOT_CLK_VPU_BUS = 19,
    CCM_ROOT_CLK_DISPLAY_AXI = 20,
    CCM_ROOT_CLK_APB = 21,
    CCM_ROOT_CLK_RTRM = 22,
    CCM_ROOT_CLK_USB_BUS = 23,
    CCM_ROOT_CLK_GPU_AXU = 24,
    CCM_ROOT_CLK_GPU_AHB = 25,
    CCM_ROOT_CLK_NOC = 26,
    CCM_ROOT_CLK_NOC_APB = 27,
    CCM_ROOT_CLK_AHB = 32,
    CCM_ROOT_CLK_RDC = 33, /* NOT IN DATASHEET */
    CCM_ROOT_CLK_AUDIO_AHB = 34,
    CCM_ROOT_CLK_DSI_RX = 36,
    CCM_ROOT_CLK_DRAM_ALT = 64,
    CCM_ROOT_CLK_DRAM_APB = 65,
    CCM_ROOT_CLK_VPU_G1 = 66,
    CCM_ROOT_CLK_VPU_G2 = 67,
    CCM_ROOT_CLK_DISPLAY_DTRC = 68,
    CCM_ROOT_CLK_DC8000 = 69,
    CCM_ROOT_CLK_PCIE_CTRL = 70,
    CCM_ROOT_CLK_PCIE1_PHY = 71,
    CCM_ROOT_CLK_PCIE_AUX = 72,
    CCM_ROOT_CLK_DC_PIXEL = 73,
    CCM_ROOT_CLK_LCDIF_PIXEL = 74,
    CCM_ROOT_CLK_SAI1 = 75,
    CCM_ROOT_CLK_SAI2 = 76,
    CCM_ROOT_CLK_SAI3 = 77,
    CCM_ROOT_CLK_SAI4 = 78,
    CCM_ROOT_CLK_SAI5 = 79,
    CCM_ROOT_CLK_SAI6 = 80,
    CCM_ROOT_CLK_SPDIF1 = 81,
    CCM_ROOT_CLK_SPDIF2 = 82,
    CCM_ROOT_CLK_ENET_REF = 83,
    CCM_ROOT_CLK_ENET_TIMER = 84,
    CCM_ROOT_CLK_ENET_PHY = 85,
    CCM_ROOT_CLK_NAND = 86,
    CCM_ROOT_CLK_QSPI = 87,
    CCM_ROOT_CLK_USDHC1 = 88,
    CCM_ROOT_CLK_USDHC2 = 89,
    CCM_ROOT_CLK_I2C1 = 90,
    CCM_ROOT_CLK_I2C2 = 91,
    CCM_ROOT_CLK_I2C3 = 92,
    CCM_ROOT_CLK_I2C4 = 93,
    CCM_ROOT_CLK_UART1 = 94,
    CCM_ROOT_CLK_UART2 = 95,
    CCM_ROOT_CLK_UART3 = 96,
    CCM_ROOT_CLK_UART4 = 97,
    CCM_ROOT_CLK_USB_CORE = 98,
    CCM_ROOT_CLK_USB_PHY = 99,
    CCM_ROOT_CLK_GIC = 100,
    CCM_ROOT_CLK_ECSPI1 = 101,
    CCM_ROOT_CLK_ECSPI2 = 102,
    CCM_ROOT_CLK_PWM1 = 103,
    CCM_ROOT_CLK_PWM2 = 104,
    CCM_ROOT_CLK_PWM3 = 105,
    CCM_ROOT_CLK_PWM4 = 106,
    CCM_ROOT_CLK_GPT1 = 107,
    CCM_ROOT_CLK_GPT2 = 108,
    CCM_ROOT_CLK_GPT3 = 109,
    CCM_ROOT_CLK_GPT4 = 110,
    CCM_ROOT_CLK_GPT5 = 111,
    CCM_ROOT_CLK_GPT6 = 112,
    CCM_ROOT_CLK_TRACE = 113,
    CCM_ROOT_CLK_WDOG = 114,
    CCM_ROOT_CLK_WRCLK = 115,
    CCM_ROOT_CLK_IPP_CLKO1 = 116,
    CCM_ROOT_CLK_IPP_CLKO2 = 117,
    CCM_ROOT_CLK_DSI_CORE = 118,
    CCM_ROOT_CLK_DSI_PHY = 119,
    CCM_ROOT_CLK_DSI_DBI = 120,
    CCM_ROOT_CLK_DSI_ESC = 121,
    CCM_ROOT_CLK_CSI1_CORE = 122,
    CCM_ROOT_CLK_CSI1_PHY = 123,
    CCM_ROOT_CLK_CSI1_ESC = 124,
    CCM_ROOT_CLK_CSI2_CORE = 125,
    CCM_ROOT_CLK_CSI2_PHY = 126,
    CCM_ROOT_CLK_CSI2_ESC = 127,
    CCM_ROOT_CLK_PCIE2_CTRL = 128,
    CCM_ROOT_CLK_PCIE2_PHY = 129,
    CCM_ROOT_CLK_PCIE2_AUX = 130,
    CCM_ROOT_CLK_ECSPI3 = 131,
    CCM_ROOT_CLK_DSI_ESC_RX = 132,
    CCM_ROOT_CLK_HDMI = 133
} ccm_root_clk_t;

/* From table 5-9 datasheet */
typedef enum {

    CCM_CCGR_ECSPI1 = 7,
    CCM_CCGR_ECSPI2 = 8,
    CCM_CCGR_ECSPI8 = 9,
    CCM_CCGR_GPIO1 = 11,
    CCM_CCGR_GPIO2 = 12,
    CCM_CCGR_GPIO3 = 13,
    CCM_CCGR_GPIO4 = 14,
    CCM_CCGR_GPIO5 = 15,
    CCM_CCGR_GPT1 = 16,
    CCM_CCGR_GPT2 = 17,
    CCM_CCGR_GPT3 = 18,
    CCM_CCGR_GPT4 = 19,
    CCM_CCGR_GPT5 = 20,
    CCM_CCGR_GPT6 = 21,

    CCM_CCGR_I2C1 = 23,
    CCM_CCGR_I2C2 = 24,
    CCM_CCGR_I2C3 = 25,
    CCM_CCGR_I2C4 = 26,

    CCM_CCGR_IOMUX = 27,
    CCM_CCGR_IOMUX1 = 28,
    CCM_CCGR_IOMUX2 = 29,
    CCM_CCGR_IOMUX3 = 30,
    CCM_CCGR_IOMUX4 = 31,

    CCM_CCGR_M4 = 32,
    CCM_CCGR_MU = 33,

    CCM_CCGR_RDC = 49,

    CCM_CCGR_UART1 = 73,
    CCM_CCGR_UART2 = 74,
    CCM_CCGR_UART3 = 75,
    CCM_CCGR_UART4 = 76,

    CCM_CCGR_XTAL = 96,
    CCM_CCGR_PLL = 97,
    CCM_CCGR_TSENSOR = 98,
}ccm_ccgr_t;

static inline void ccm_set_root_clk(ccm_root_clk_t clk, uint32_t pre,
                                        uint32_t post, uint32_t mux) {
                                   
    CCM->ROOT[clk].TARGET_ROOT_SET = CCM_TARGET_ROOT_PRE_PODF(pre - 1) |
                                     CCM_TARGET_ROOT_POST_PODF(post - 1) |
                                     CCM_TARGET_ROOT_MUX(mux);
}

static inline void ccm_en_root_clk(ccm_root_clk_t clk, uint8_t mode) {

    CCM->ROOT[clk].TARGET_ROOT_SET = CCM_TARGET_ROOT_ENABLE(mode);
}

static inline void ccm_set_ccgr(ccm_ccgr_t ccgr, uint32_t val) {
        CCM->CCGR[ccgr].CCGR_SET = val;
}
#ifdef __cplusplus
}
#endif

#endif /* CLK_CONF_H */
/** @} */