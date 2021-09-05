/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_cc13x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0/CC13x0 PRCM register definitions
 */

#ifndef CC26X0_CC13X0_PRCM_H
#define CC26X0_CC13X0_PRCM_H

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DDI_0_OSC registers
 */
typedef struct {
    reg32_t CTL0; /**< control 0 */
    reg32_t CTL1; /**< control 1 */
    reg32_t RADCEXTCFG; /**< RADC external config */
    reg32_t AMPCOMPCTL; /**< amplitude compensation control */
    reg32_t AMPCOMPTH1; /**< amplitude compensation threshold 1 */
    reg32_t AMPCOMPTH2; /**< amplitude compensation threshold 2 */
    reg32_t ANABYPASSVAL1; /**< analog bypass values 1 */
    reg32_t ANABYPASSVAL2; /**< analog bypass values 2 */
    reg32_t ATESTCTL; /**< analog test control */
    reg32_t ADCDOUBLERNANOAMPCTL; /**< ADC doubler nanoamp control */
    reg32_t XOSCHFCTL; /**< XOSCHF control */
    reg32_t LFOSCCTL; /**< low frequency oscillator control */
    reg32_t RCOSCHFCTL; /**< RCOSCHF control */
    reg32_t STAT0; /**< status 0 */
    reg32_t STAT1; /**< status 1 */
    reg32_t STAT2; /**< status 2 */
} ddi0_osc_regs_t;

/**
 * @brief   DDI_0_OSC register values
 * @{
 */
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_mask         0x6
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_HF_RCOSC     0x0
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_HF_XOSC      0x4
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_LF_RCOSC     0x8
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_LF_XOSC      0xC
#define DDI_0_OSC_CTL0_ACLK_REF_SRC_SEL_RCOSC_mask  0x60
#define DDI_0_OSC_CTL0_ACLK_REF_SRC_SEL_RCOSC_HF    0x00 /* 31.25kHz */
#define DDI_0_OSC_CTL0_ACLK_REF_SRC_SEL_XOSC_HF     0x20 /* 31.25kHz */
#define DDI_0_OSC_CTL0_ACLK_REF_SRC_SEL_RCOSC_LF    0x40 /* 32kHz */
#define DDI_0_OSC_CTL0_ACLK_REF_SRC_SEL_XOSC_LF     0x60 /* 32.768kHz */
#define DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL_RCOSC_mask  0x180
#define DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL_RCOSC_HF    0x000 /* 48MHz */
#define DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL_RCOSC_LF    0x080 /* 48MHz */
#define DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL_XOSC_HF     0x100 /* 24MHz */
#define DDI_0_OSC_CTL0_DOUBLER_START_DURATION_mask  0x6000000
#define DDI_0_OSC_CTL0_BYPASS_RCOSC_LF_CLK_QUAL     0x10000000
#define DDI_0_OSC_CTL0_BYPASS_XOSC_LF_CLK_QUAL      0x20000000
#define DDI_0_OSC_CTL0_XTAL_IS_24M                  0x80000000
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define DDI0_OSC_BASE       0x400CA000 /**< DDI0_OSC base address */
/** @} */

/**
 * @brief DDI_0_OSC register bank
 */
#define DDI_0_OSC ((ddi0_osc_regs_t *) (DDI0_OSC_BASE))

/**
 * @brief AON_SYSCTL registers
 */
typedef struct {
    reg32_t PWRCTL; /**< power management */
    reg32_t RESETCTL; /**< reset management */
    reg32_t SLEEPCTL; /**< sleep mode */
} aon_sysctl_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AON_SYSCTL_BASE       0x40090000 /**< AON_SYSCTL base address */
/** @} */

#define AON_SYSCTL ((aon_sysctl_regs_t *) (AON_SYSCTL_BASE)) /**< AON_SYSCTL register bank */

/**
 * @brief AON_WUC registers
 */
typedef struct {
    reg32_t MCUCLK; /**< MCU clock management */
    reg32_t AUXCLK; /**< AUX clock management */
    reg32_t MCUCFG; /**< MCU config */
    reg32_t AUXCFG; /**< AUX config */
    reg32_t AUXCTL; /**< AUX control */
    reg32_t PWRSTAT; /**< power status */
    reg32_t __reserved1; /**< Reserved */
    reg32_t SHUTDOWN; /**< shutdown control */
    reg32_t CTL0; /**< control 0 */
    reg32_t CTL1; /**< control 1 */
    reg32_t __reserved2[2]; /**< Reserved */
    reg32_t RECHARGECFG; /**< recharge controller config */
    reg32_t RECHARGESTAT; /**< recharge controller status */
    reg32_t __reserved3; /**< Reserved */
    reg32_t OSCCFG; /**< oscillator config */
    reg32_t JTAGCFG; /**< JTAG config */
    reg32_t JTAGUSERCODE; /**< JTAG USERCODE */
} aon_wuc_regs_t;

/**
 * @brief   AON_WUC register values
 * @{
 */
#define MCUCLK_PWR_DWN_SRC          0x1 /* SCLK_LF in powerdown (no clock elsewise) */
#define MCUCLK_PWR_DWN_SRC_mask     0x3
#define MCUCLK_RCOSC_HF_CAL_DONE    0x4 /* set by MCU bootcode. RCOSC_HF is calibrated to 48 MHz, allowing FLASH to power up */

#define AUXCLK_SRC_HF               0x1 /* SCLK for AUX */
#define AUXCLK_SRC_LF               0x4
#define AUXCLK_SRC_mask             0x7 /* guaranteed to be glitchless */
#define AUXCLK_SCLK_HF_DIV_pos      8 /* don't set while SCLK_HF active for AUX */
#define AUXCLK_SCLK_HF_DIV_mask     0x700 /* divisor will be 2^(value+1) */
#define AUXCLK_PWR_DWN_SRC_pos      11 /* SCLK_LF in powerdown when SCLK_HF is source (no clock elsewise?!)  */
#define AUXCLK_PWR_DWN_SRC_mask     0x1800 /* datasheet is confusing.. */

#define MCUCFG_SRAM_RET_OFF         0x0 /* no retention for any SRAM-bank */
#define MCUCFG_SRAM_RET_B0          0x1
#define MCUCFG_SRAM_RET_B01         0x3
#define MCUCFG_SRAM_RET_B012        0x7
#define MCUCFG_SRAM_RET_B0124       0xF /* retention for banks 0, 1, 2, and 3 */
#define MCUCFG_SRAM_FIXED_WU_EN     0x100
#define MCUCFG_SRAM_VIRT_OFF        0x200

#define AUXCFG_RAM_RET_EN           0x1 /* retention for AUX_RAM bank 0. is off when otherwise in retention mode */

#define AUXCTL_AUX_FORCE_ON     0x1
#define AUXCTL_SWEV             0x2
#define AUXCTL_SCE_RUN_EN       0x3
#define AUXCTL_RESET_REQ        0x80000000

#define PWRSTAT_AUX_RESET_DONE      0x2
#define PWRSTAT_AUX_BUS_CONNECTED   0x4
#define PWRSTAT_MCU_PD_ON           0x10
#define PWRSTAT_AUX_PD_ON           0x20
#define PWRSTAT_JTAG_PD_ON          0x40
#define PWRSTAT_AUX_PWR_DNW         0x200

#define SHUTDOWN_EN     0x1 /* register/cancel shutdown request */

#define AONWUC_CTL0_MCU_SRAM_ERASE      0x4
#define AONWUC_CTL0_AUX_SRAM_ERASE      0x8
#define AONWUC_CTL0_PWR_DWN_DIS         0x10 /* disable powerdown on request */

#define AONWUC_CTL1_MCU_WARM_RESET      0x1 /* last MCU reset was a warm reset */
#define AONWUC_CTL1_MCU_RESET_SRC       0x2 /* JTAG was source of last reset (MCU SW elsewise) */

#define RECHARGECFG_PER_E_mask          0x00000007 /* number of 32KHz clocks between activation of recharge controller: */
#define RECHARGECFG_PER_M_mask          0x000000F8 /* computed as follows: PERIOD = (PER_M*16+15) * 2^(PER_E) */
#define RECHARGECFG_MAX_PER_E_mask      0x00000700 /* maximum period the recharge algorithm can take */
#define RECHARGECFG_MAX_PER_M_mask      0x0000F800 /* computed as follows: MAXCYCLES = (MAX_PER_M*16+15) * 2^(MAX_PER_E) */
#define RECHARGECFG_C1_mask             0x000F0000 /* i resign */
#define RECHARGECFG_C2_mask             0x000F0000
#define RECHARGECFG_ADAPTIVE_EN         0x80000000

#define RECHARGESTAT_MAX_USED_PER_mask  0x0FFFF
#define RECHARGESTAT_VDDR_SMPLS_mask    0xF0000

#define OSCCFG_PER_E_mask               0x07 /* number of 32KHz clocks between oscillator amplitude callibrations */
#define OSCCFG_PER_M_mask               0xF8 /* computed as follows: PERIOD = (PER_M*16+15) * 2^(PER_E) */

#define JTAGCFG_JTAG_PD_FORCE_ON        0x10
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AON_WUC_BASE       0x40091000 /**< AON_WUC base address */
/** @} */

#define AON_WUC ((aon_wuc_regs_t *) (AON_WUC_BASE)) /**< AON_WUC register bank */

/**
 * @brief AON_RTC registers
 */
typedef struct {
    reg32_t CTL; /**< Control */
    reg32_t EVFLAGS; /**< Event Flags, RTC Status */
    reg32_t SEC; /**< Second Counter Value, Integer Part */
    reg32_t SUBSEC; /**< Second Counter Value, Fractional Part */
    reg32_t SUBSECINC; /**< Subseconds Increment */
    reg32_t CHCTL; /**< Channel Configuration */
    reg32_t CH0CMP; /**< Channel 0 Compare Value */
    reg32_t CH1CMP; /**< Channel 1 Compare Value */
    reg32_t CH2CMP; /**< Channel 2 Compare Value */
    reg32_t CH2CMPINC; /**< Channel 2 Compare Value Auto-increment */
    reg32_t CH1CAPT; /**< Channel 1 Capture Value */
    reg32_t SYNC; /**< AON Synchronization */
} aon_rtc_regs_t;

/**
 * @brief   RTC_UPD is a 16 KHz signal used to sync up the radio timer. The
 *          16 Khz is SCLK_LF divided by 2
 * @details 0h = RTC_UPD signal is forced to 0
 *          1h = RTC_UPD signal is toggling @16 kHz
 */
#define AON_RTC_CTL_RTC_UPD_EN 0x00000002

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AON_RTC_BASE (PERIPH_BASE + 0x92000) /**< AON_RTC base address */
/** @} */

#define AON_RTC ((aon_rtc_regs_t *) (AON_RTC_BASE)) /**< AON_RTC register bank */

/**
 * @brief PRCM registers
 */
typedef struct {
    reg32_t INFRCLKDIVR; /**< infrastructure clock division factor for run mode */
    reg32_t INFRCLKDIVS; /**< infrastructure clock division factor for sleep mode */
    reg32_t INFRCLKDIVDS; /**< infrastructure clock division factor for deep sleep mode */
    reg32_t VDCTL; /**< MCU voltage domain control */
    reg32_t __reserved1[6]; /**< Reserved */
    reg32_t CLKLOADCTL; /**< clock load control */
    reg32_t RFCCLKG; /**< RFC clock gate */
    reg32_t VIMSCLKG; /**< VIMS clock gate */
    reg32_t __reserved2[2]; /**< Reserved */
    reg32_t SECDMACLKGR; /**< TRNG, CRYPTO, and UDMA clock gate for run mode */
    reg32_t SECDMACLKGS; /**< TRNG, CRYPTO, and UDMA clock gate for sleep mode */
    reg32_t SECDMACLKGDS; /**< TRNG, CRYPTO, and UDMA clock gate for deep sleep mode */
    reg32_t GPIOCLKGR; /**< GPIO clock gate for run mode */
    reg32_t GPIOCLKGS; /**< GPIO clock gate for sleep mode */
    reg32_t GPIOCLKGDS; /**< GPIO clock gate for deep sleep mode */
    reg32_t GPTCLKGR; /**< GPT clock gate for run mode */
    reg32_t GPTCLKGS; /**< GPT clock gate for sleep mode */
    reg32_t GPTCLKGDS; /**< GPT clock gate for deep sleep mode */
    reg32_t I2CCLKGR; /**< I2C clock gate for run mode */
    reg32_t I2CCLKGS; /**< I2C clock gate for sleep mode */
    reg32_t I2CCLKGDS; /**< I2C clock gate for deep sleep mode */
    reg32_t UARTCLKGR; /**< UART clock gate for run mode */
    reg32_t UARTCLKGS; /**< UART clock gate for sleep mode */
    reg32_t UARTCLKGDS; /**< UART clock gate for deep sleep mode */
    reg32_t SSICLKGR; /**< SSI clock gate for run mode */
    reg32_t SSICLKGS; /**< SSI clock gate for sleep mode */
    reg32_t SSICLKGDS; /**< SSI clock gate for deep sleep mode */
    reg32_t I2SCLKGR; /**< I2S clock gate for run mode */
    reg32_t I2SCLKGS; /**< I2S clock gate for sleep mode */
    reg32_t I2SCLKGDS; /**< I2S clock gate for deep sleep mode */
    reg32_t __reserved3[10]; /**< Reserved */
    reg32_t CPUCLKDIV; /**< CPU clock division factor */
    reg32_t __reserved4[3]; /**< Reserved */
    reg32_t I2SBCLKSEL; /**< I2S clock select */
    reg32_t GPTCLKDIV; /**< GPT scalar */
    reg32_t I2SCLKCTL; /**< I2S clock control */
    reg32_t I2SMCLKDIV; /**< MCLK division ratio */
    reg32_t I2SBCLKDIV; /**< BCLK division ratio */
    reg32_t I2SWCLKDIV; /**< WCLK division ratio */
    reg32_t __reserved5[11]; /**< Reserved */
    reg32_t SWRESET; /**< SW initiated resets */
    reg32_t WARMRESET; /**< WARM reset control and status */
    reg32_t __reserved6[6]; /**< Reserved */
    reg32_t PDCTL0; /**< power domain control */
    reg32_t PDCTL0RFC; /**< RFC power domain control */
    reg32_t PDCTL0SERIAL; /**< SERIAL power domain control */
    reg32_t PDCTL0PERIPH; /**< PERIPH power domain control */
    reg32_t __reserved7; /**< Reserved */
    reg32_t PDSTAT0; /**< power domain status */
    reg32_t PDSTAT0RFC; /**< RFC power domain status */
    reg32_t PDSTAT0SERIAL; /**< SERIAL power domain status */
    reg32_t PDSTAT0PERIPH; /**< PERIPH power domain status */
    reg32_t __reserved8[11]; /**< Reserved */
    reg32_t PDCTL1; /**< power domain control */
    reg32_t __reserved9; /**< power domain control */
    reg32_t PDCTL1CPU; /**< CPU power domain control */
    reg32_t PDCTL1RFC; /**< RFC power domain control */
    reg32_t PDCTL1VIMS; /**< VIMS power domain control */
    reg32_t __reserved10; /**< Reserved */
    reg32_t PDSTAT1; /**< power domain status */
    reg32_t PDSTAT1BUS; /**< BUS power domain status */
    reg32_t PDSTAT1RFC; /**< RFC power domain status */
    reg32_t PDSTAT1CPU; /**< CPU power domain status */
    reg32_t PDSTAT1VIMS; /**< VIMS power domain status */
    reg32_t __reserved11[10]; /**< Reserved */
    reg32_t RFCMODESEL; /**< selected RFC mode */
    reg32_t __reserved12[20]; /**< Reserved */
    reg32_t RAMRETEN; /**< memory retention control */
    reg32_t __reserved13; /**< Reserved */
    reg32_t PDRETEN; /**< power domain retention (undocumented) */
    reg32_t __reserved14[8]; /**< Reserved */
    reg32_t RAMHWOPT; /**< undocumented */
} prcm_regs_t;

/**
 * @brief   PRCM register values
 * @{
 */
#define CLKLOADCTL_LOAD        0x1
#define CLKLOADCTL_LOADDONE    0x2

#define PDCTL0_RFC_ON       0x1
#define PDCTL0_SERIAL_ON    0x2
#define PDCTL0_PERIPH_ON    0x4

#define PDSTAT0_RFC_ON       0x1
#define PDSTAT0_SERIAL_ON    0x2
#define PDSTAT0_PERIPH_ON    0x4

#define PDCTL1_CPU_ON       0x2
#define PDCTL1_RFC_ON       0x4
#define PDCTL1_VIMS_ON      0x8

#define PDSTAT1_CPU_ON      0x2
#define PDSTAT1_RFC_ON      0x4
#define PDSTAT1_VIMS_ON     0x8

#define GPIOCLKGR_CLK_EN       0x1
#define I2CCLKGR_CLK_EN        0x1
#define UARTCLKGR_CLK_EN_UART0 0x1

#define GPIOCLKGS_CLK_EN       0x1
#define I2CCLKGS_CLK_EN        0x1
#define UARTCLKGS_CLK_EN_UART0 0x1

#define GPIOCLKGDS_CLK_EN       0x1
#define I2CCLKGDS_CLK_EN        0x1
#define UARTCLKGDS_CLK_EN_UART0 0x1
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define PRCM_BASE        (PERIPH_BASE + 0x82000) /**< PRCM base address */
#define PRCM_BASE_NONBUF (PERIPH_BASE_NONBUF + 0x82000) /**< PRCM base address (nonbuf) */
/** @} */

#define PRCM        ((prcm_regs_t *) (PRCM_BASE)) /**< PRCM register bank */
#define PRCM_NONBUF ((prcm_regs_t *) (PRCM_BASE_NONBUF)) /**< PRCM register bank (nonbuf) */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X0_CC13X0_PRCM_H */
/** @} */
