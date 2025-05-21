/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26x2_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 PRCM register definitions
 */


#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DDI_0_OSC registers
 */
typedef struct {
    reg32_t CTL0; /**< Control 0 */
    reg32_t CTL1; /**< Control 1 */
    reg32_t RADCEXTCFG; /**< RADC External Configuration */
    reg32_t AMPCOMPCTL; /**< Amplitude Compensation Control */
    reg32_t AMPCOMPTH1; /**< Amplitude Compensation Threshold 1 */
    reg32_t AMPCOMPTH2; /**< Amplitude Compensation Threshold 2 */
    reg32_t ANABYPASSVAL1; /**< Analog Bypass Values 1 */
    reg32_t ANABYPASSVAL2; /**< Internal */
    reg32_t ATESTCTL; /**< Analog Test Control */
    reg32_t ADCDOUBLERNANOAMPCTL; /**< ADC Doubler Nanoamp Control */
    reg32_t XOSCHFCTL; /**< XOSCHF Control */
    reg32_t LFOSCCTL; /**< Low Frequency Oscillator Control */
    reg32_t RCOSCHFCTL; /**< RCOSCHF Control */
    reg32_t RCOSCMFCTL; /**< RCOSC_MF Control */
    reg32_t __reserved1; /**< Reserved */
    reg32_t STAT0; /**< Status 0 */
    reg32_t STAT1; /**< Status 1 */
    reg32_t STAT2; /**< Status 2 */
} ddi0_osc_regs_t;

/**
 * @brief   DDI_0_OSC registers with masked 16-bit access
 */
typedef struct {
    reg32_m16_t CTL0; /**< Control 0 */
    reg32_m16_t CTL1; /**< Control 1 */
    reg32_m16_t RADCEXTCFG; /**< RADC External Configuration */
    reg32_m16_t AMPCOMPCTL; /**< Amplitude Compensation Control */
    reg32_m16_t AMPCOMPTH1; /**< Amplitude Compensation Threshold 1 */
    reg32_m16_t AMPCOMPTH2; /**< Amplitude Compensation Threshold 2 */
    reg32_m16_t ANABYPASSVAL1; /**< Analog Bypass Values 1 */
    reg32_m16_t ANABYPASSVAL2; /**< Internal */
    reg32_m16_t ATESTCTL; /**< Analog Test Control */
    reg32_m16_t ADCDOUBLERNANOAMPCTL; /**< ADC Doubler Nanoamp Control */
    reg32_m16_t XOSCHFCTL; /**< XOSCHF Control */
    reg32_m16_t LFOSCCTL; /**< Low Frequency Oscillator Control */
    reg32_m16_t RCOSCHFCTL; /**< RCOSCHF Control */
    reg32_m16_t RCOSCMFCTL; /**< RCOSC_MF Control */
    reg32_m16_t __reserved1; /**< Reserved */
    reg32_m16_t STAT0; /**< Status 0 */
    reg32_m16_t STAT1; /**< Status 1 */
    reg32_m16_t STAT2; /**< Status 2 */
} ddi0_osc_regs_m16_t;

/**
 * @brief   DDI_0_OSC register values
 * @{
 */
#define DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_m        0x00000001
#define DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_s        0
#define DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_XOSC     0x00000001
#define DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_RCOSC    0x00000000
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_m        0x0000000C
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_s        2
#define DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL         0x00000180
#define DDI_0_OSC_CTL0_CLK_LOSS_EN              0x00000200
#define DDI_0_OSC_CTL0_XOSC_LF_DIG_BYPASS       0x00000400
#define DDI_0_OSC_CTL0_XOSC_HF_POWER_MODE       0x00000800
#define DDI_0_OSC_CTL0_RCOSC_LF_TRIMMED         0x00001000
#define DDI_0_OSC_CTL0_HPOSC_MODE_EN            0x00004000
#define DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m       0x01000000
#define DDI_0_OSC_CTL0_DOUBLER_RESET_DURATION   0x02000000
#define DDI_0_OSC_CTL0_DOUBLER_START_DURATION   0x0C000000
#define DDI_0_OSC_CTL0_BYPASS_RCOSC_LF_CLK_QUAL 0x10000000
#define DDI_0_OSC_CTL0_BYPASS_XOSC_LF_CLK_QUAL  0x20000000
#define DDI_0_OSC_CTL0_XTAL_IS_24M              0x80000000
#define DDI_0_OSC_STAT0_PENDINGSCLKHFSWITCHING  0x00000001
#define DDI_0_OSC_STAT0_SCLK_HF_SRC_m           0x10000000
#define DDI_0_OSC_STAT0_SCLK_HF_SRC_s           28
#define DDI_0_OSC_STAT0_SCLK_LF_SRC_m           0x60000000
#define DDI_0_OSC_STAT0_SCLK_LF_SRC_s           29
/** @} */

/**
 * @ingroup cpu_cc26x2_cc13x2_peripheral_memory_map
 * @{
 */
#define DDI_DIR              0x00000000
#define DDI_SET              0x00000080
#define DDI_CLR              0x00000100
#define DDI_MASK4B           0x00000200
#define DDI_MASK8B           0x00000300
#define DDI_MASK16B          0x00000400
/**
 * @brief   DDI0_OSC base address
 */
#define DDI0_OSC_BASE        (PERIPH_BASE + 0xCA000)
/**
 * @brief   DDI0_OSC 16-bit masked access base address
 */
#define DDI0_OSC_BASE_M16    (DDI0_OSC_BASE + DDI_MASK16B)
/** @} */

/**
 * @brief   DDI_0_OSC register bank
 */
#define DDI_0_OSC            ((ddi0_osc_regs_t *) (DDI0_OSC_BASE))
/**
 * @brief   DDI_0_OSC 16-bit masked access register bank
 */
#define DDI_0_OSC_M16        ((ddi0_osc_regs_m16_t *) (DDI0_OSC_BASE_M16))

/**
 * @brief   SCLK_HF oscillators.
 * @{
 */
#define OSC_RCOSC_HF         0x00000000 /**< Internal HF RC oscillator */
#define OSC_XOSC_HF          0x00000001 /**< External HF crystal oscillator */
/** @} */

/**
 * @brief    DDI_0_OSC functions
 * @{
 */
/**
 * @brief   Switch the high frequency clock.
 *
 * @note This function will not return until the clock source has been switched.
 *
 * @param[in] osc The oscillator to use.
 */
void osc_hf_source_switch(uint32_t osc);
/** @} */

/**
 * @brief    AON_PMCTL registers
 */
typedef struct {
   reg32_t __reserved1; /**< Reserved */
   reg32_t AUXSCECLK; /**< AUX SCE management */
   reg32_t RAMCFG; /**< RAM configuration */
   reg32_t __reserved2; /**< Reserved */
   reg32_t PWRCTL; /**< Power management control */
   reg32_t PWRSTAT; /**< Power status */
   reg32_t SHUTDOWN; /**< Shutdown control */
   reg32_t RECHARGECFG; /**< Recharge controller configuration */
   reg32_t RECHARGESTAT; /**< Recharge controller status */
   reg32_t OSCCFG; /**< Oscillator configuration */
   reg32_t RESETCTL; /**< Reset control */
   reg32_t SLEEPCTL; /**< Reset control */
   reg32_t __reserved3; /**< Reserved */
   reg32_t JTAGCFG; /**< JTAG configuration */
   reg32_t __reserved4; /**< Reserved */
   reg32_t JTAGUSERCODE; /**< JTAG USERCODE */
} aon_pmctl_regs_t;

/**
 * @brief   AON_PMTCTL register values
 * @{
 */
#define AON_PMCTL_SLEEPCTL_IO_PAD_SLEEP_DIS 0x00000001
#define AON_PMCTL_RESETCTL_BOOT_DET_1_CLR_m 0x02000000
#define AON_PMCTL_RESETCTL_BOOT_DET_0_CLR_m 0x01000000
#define AON_PMCTL_RESETCTL_BOOT_DET_1_SET_m 0x00020000
#define AON_PMCTL_RESETCTL_BOOT_DET_0_SET_m 0x00010000
#define AON_PMCTL_RESETCTL_BOOT_DET_1_m     0x00002000
#define AON_PMCTL_RESETCTL_BOOT_DET_0_m     0x00001000
#define AON_PMCTL_RESETCTL_BOOT_DET_0_s     12
#define AON_PMCTL_RESETCTL_MCU_WARM_RESET_m 0x00000010
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AON_PMCTL base address
 */
#define AON_PMCTL_BASE       (PERIPH_BASE + 0x90000)
/** @} */

/**
 * @brief   AON_PMCTL register bank
 */
#define AON_PMCTL            ((aon_pmctl_regs_t *) (AON_PMCTL_BASE))

/**
 * AON_RTC registers
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
    reg32_t TIME; /**< Current Counter Value */
    reg32_t SYNCLF; /**< Synchronization to SCLK_LF */
} aon_rtc_regs_t;

/**
 * @brief   RTC_UPD is a 16 KHz signal used to sync up the radio timer. The
 *          16 Khz is SCLK_LF divided by 2
 * @details 0h = RTC_UPD signal is forced to 0
 *          1h = RTC_UPD signal is toggling @16 kHz
 */
#define AON_RTC_CTL_RTC_UPD_EN 0x00000002

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AON_RTC_BASE (PERIPH_BASE + 0x92000) /**< AON_RTC base address */
/** @} */

#define AON_RTC ((aon_rtc_regs_t *) (AON_RTC_BASE)) /**< AON_RTC register bank */

/**
 * PRCM registers
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
    reg32_t __reserved3[9]; /**< Reserved */
    reg32_t SYSBUSCLKDIV; /**< System bus clock division factor */
    reg32_t CPUCLKDIV; /**< CPU clock division factor */
    reg32_t PERBUSCPUCLKDIV; /**< Peripheral bus division factor */
    reg32_t __reserved4; /**< Reserved */
    reg32_t PERDMACLKDIV; /**< DMA clock division factor */
    reg32_t I2SBCLKSEL; /**< I2S clock select */
    reg32_t GPTCLKDIV; /**< GPT scalar */
    reg32_t I2SCLKCTL; /**< I2S clock control */
    reg32_t I2SMCLKDIV; /**< MCLK division ratio */
    reg32_t I2SBCLKDIV; /**< BCLK division ratio */
    reg32_t I2SWCLKDIV; /**< WCLK division ratio */
    reg32_t __reserved5[4]; /**< Reserved */
    reg32_t RESETSECDMA; /**< Reset SEC and UDMA */
    reg32_t RESETGPIO; /**< Reset GPIO */
    reg32_t RESETGPT; /**< Reset GPTs */
    reg32_t RESETI2C; /**< Reset I2C */
    reg32_t RESETUART; /**< Reset UART */
    reg32_t RESETSSI; /**< Reset SSI */
    reg32_t RESETI2S; /**< Reset I2S */
    reg32_t __reserved6[8]; /**< Reserved */
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
    reg32_t __reserved11[9]; /**< Reserved */
    reg32_t RFCBITS; /**< Control to RFC */
    reg32_t RFCMODESEL; /**< selected RFC mode */
    reg32_t RFCMODEHWOPT; /**< allowed RFC modes */
    reg32_t __reserved12[2]; /**< Reserved */
    reg32_t PWRPROFSTAT; /**< power profiler register */
    reg32_t __reserved13[14]; /**< Reserved */
    reg32_t MCUSRAMCFG; /**< MCU SRAM configuration */
    reg32_t __reserved14; /**< Reserved */
    reg32_t RAMRETEN; /**< memory retention control */
    reg32_t __reserved15[27]; /**< Reserved */
    reg32_t OSCIMSC; /**< oscillator interrupt mask */
    reg32_t OSCRIS; /**< oscillator raw interrupt status */
    reg32_t OSCICR; /**< oscillator raw interrupt clear */
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

#define GPIOCLKGR_CLK_EN        0x1
#define I2CCLKGR_CLK_EN         0x1
#define UARTCLKGR_CLK_EN_UART0  0x1
#define UARTCLKGR_CLK_EN_UART1  0x2

#define GPIOCLKGS_CLK_EN        0x1
#define I2CCLKGS_CLK_EN         0x1
#define UARTCLKGS_CLK_EN_UART0  0x1
#define UARTCLKGS_CLK_EN_UART1  0x2

#define GPIOCLKGDS_CLK_EN       0x1
#define I2CCLKGDS_CLK_EN        0x1
#define UARTCLKGDS_CLK_EN_UART0 0x1
#define UARTCLKGDS_CLK_EN_UART1 0x2
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

/** @} */
