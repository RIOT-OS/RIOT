/*******************************************************************************
* File Name: cyPm.h
* Version 4.20
*
* Description:
*  Provides the function definitions for the power management API.
*
* Note:
*  Documentation of the API's in this file is located in the
*  System Reference Guide provided with PSoC Creator.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYPM_H)
#define CY_BOOT_CYPM_H

#include "cytypes.h"        /* Register access API      */
#include "cydevice_trm.h"   /* Registers addresses      */
#include "cyfitter.h"       /* Comparators placement    */
#include "CyLib.h"          /* Clock API                */
#include "CyFlash.h"        /* Flash API - CyFlash_SetWaitCycles()  */


/***************************************
*    Function Prototypes
***************************************/
void CyPmSaveClocks(void) ;
void CyPmRestoreClocks(void) ;
void CyPmAltAct(uint16 wakeupTime, uint16 wakeupSource) ;
void CyPmSleep(uint8 wakeupTime, uint16 wakeupSource) ;
void CyPmHibernate(void) ;

uint8 CyPmReadStatus(uint8 mask) ;

/* Internal APIs and are not meant to be called directly by the user */
void CyPmCtwSetInterval(uint8 ctwInterval) ;
void CyPmFtwSetInterval(uint8 ftwInterval) ;
void CyPmOppsSet(void) ;


/***************************************
*    API Constants
***************************************/

#define PM_SLEEP_SRC_NONE               (0x0000u)
#define PM_SLEEP_TIME_NONE              (0x00u)
#define PM_ALT_ACT_SRC_NONE             (0x0000u)
#define PM_ALT_ACT_TIME_NONE            (0x0000u)

#if(CY_PSOC3)

    /* Wake up time for Sleep mode */
    #define PM_SLEEP_TIME_ONE_PPS           (0x01u)
    #define PM_SLEEP_TIME_CTW_2MS           (0x02u)
    #define PM_SLEEP_TIME_CTW_4MS           (0x03u)
    #define PM_SLEEP_TIME_CTW_8MS           (0x04u)
    #define PM_SLEEP_TIME_CTW_16MS          (0x05u)
    #define PM_SLEEP_TIME_CTW_32MS          (0x06u)
    #define PM_SLEEP_TIME_CTW_64MS          (0x07u)
    #define PM_SLEEP_TIME_CTW_128MS         (0x08u)
    #define PM_SLEEP_TIME_CTW_256MS         (0x09u)
    #define PM_SLEEP_TIME_CTW_512MS         (0x0Au)
    #define PM_SLEEP_TIME_CTW_1024MS        (0x0Bu)
    #define PM_SLEEP_TIME_CTW_2048MS        (0x0Cu)
    #define PM_SLEEP_TIME_CTW_4096MS        (0x0Du)

    /* Difference between parameter's value and register's one */
    #define CY_PM_FTW_INTERVAL_SHIFT        (0x000Eu)

    /* Wake up time for Alternate Active mode */
    #define PM_ALT_ACT_TIME_ONE_PPS         (0x0001u)
    #define PM_ALT_ACT_TIME_CTW_2MS         (0x0002u)
    #define PM_ALT_ACT_TIME_CTW_4MS         (0x0003u)
    #define PM_ALT_ACT_TIME_CTW_8MS         (0x0004u)
    #define PM_ALT_ACT_TIME_CTW_16MS        (0x0005u)
    #define PM_ALT_ACT_TIME_CTW_32MS        (0x0006u)
    #define PM_ALT_ACT_TIME_CTW_64MS        (0x0007u)
    #define PM_ALT_ACT_TIME_CTW_128MS       (0x0008u)
    #define PM_ALT_ACT_TIME_CTW_256MS       (0x0009u)
    #define PM_ALT_ACT_TIME_CTW_512MS       (0x000Au)
    #define PM_ALT_ACT_TIME_CTW_1024MS      (0x000Bu)
    #define PM_ALT_ACT_TIME_CTW_2048MS      (0x000Cu)
    #define PM_ALT_ACT_TIME_CTW_4096MS      (0x000Du)
    #define PM_ALT_ACT_TIME_FTW(x)          ((x) + CY_PM_FTW_INTERVAL_SHIFT)

#endif  /* (CY_PSOC3) */


/* Wake up sources for Sleep mode */
#define PM_SLEEP_SRC_COMPARATOR0        (0x0001u)
#define PM_SLEEP_SRC_COMPARATOR1        (0x0002u)
#define PM_SLEEP_SRC_COMPARATOR2        (0x0004u)
#define PM_SLEEP_SRC_COMPARATOR3        (0x0008u)
#define PM_SLEEP_SRC_PICU               (0x0040u)
#define PM_SLEEP_SRC_I2C                (0x0080u)
#define PM_SLEEP_SRC_BOOSTCONVERTER     (0x0200u)
#define PM_SLEEP_SRC_VD                 (0x0400u)
#define PM_SLEEP_SRC_CTW                (0x0800u)
#define PM_SLEEP_SRC_ONE_PPS            (0x0800u)
#define PM_SLEEP_SRC_LCD                (0x1000u)

/* Wake up sources for Alternate Active mode */
#define PM_ALT_ACT_SRC_COMPARATOR0      (0x0001u)
#define PM_ALT_ACT_SRC_COMPARATOR1      (0x0002u)
#define PM_ALT_ACT_SRC_COMPARATOR2      (0x0004u)
#define PM_ALT_ACT_SRC_COMPARATOR3      (0x0008u)
#define PM_ALT_ACT_SRC_INTERRUPT        (0x0010u)
#define PM_ALT_ACT_SRC_PICU             (0x0040u)
#define PM_ALT_ACT_SRC_I2C              (0x0080u)
#define PM_ALT_ACT_SRC_BOOSTCONVERTER   (0x0200u)
#define PM_ALT_ACT_SRC_FTW              (0x0400u)
#define PM_ALT_ACT_SRC_VD               (0x0400u)
#define PM_ALT_ACT_SRC_CTW              (0x0800u)
#define PM_ALT_ACT_SRC_ONE_PPS          (0x0800u)
#define PM_ALT_ACT_SRC_LCD              (0x1000u)


#define CY_PM_WAKEUP_PICU               (0x04u)
#define CY_PM_IMO_NO_WAIT_TO_SETTLE     (0x00u)
#define CY_PM_POWERDOWN_MODE            (0x01u)
#define CY_PM_HIGHPOWER_MODE            (0x00u)     /* Deprecated */
#define CY_PM_ENABLED                   (0x01u)
#define CY_PM_DISABLED                  (0x00u)

/* No wait for PLL to stabilize, used in CyPLL_OUT_Start() */
#define CY_PM_PLL_OUT_NO_WAIT           (0u)

/* No wait for MHZ XTAL to stabilize, used in CyXTAL_Start() */
#define CY_PM_XTAL_MHZ_NO_WAIT          (0u)

#define CY_PM_WAIT_200_US               (200u)
#define CY_PM_WAIT_250_US               (250u)
#define CY_PM_WAIT_20_US                (20u)

#define CY_PM_FREQ_3MHZ                 (3u)
#define CY_PM_FREQ_12MHZ                (12u)
#define CY_PM_FREQ_48MHZ                (48u)


#define     CY_PM_MHZ_XTAL_WAIT_NUM_OF_200_US   (5u)


/* Delay line bandgap current settling time starting from wakeup event */
#define     CY_PM_CLK_DELAY_BANDGAP_SETTLE_US       (50u)

/* Delay line internal bias settling */
#define     CY_PM_CLK_DELAY_BIAS_SETTLE_US          (25u)


/* Max flash wait cycles for each device */
#if(CY_PSOC3)
    #define     CY_PM_MAX_FLASH_WAIT_CYCLES        (45u)
#endif  /* (CY_PSOC3) */

#if(CY_PSOC5)
    #define     CY_PM_MAX_FLASH_WAIT_CYCLES        (55u)
#endif  /* (CY_PSOC5) */


/*******************************************************************************
* This marco is used to obtain the CPU frequency in MHz. It should be only used
* when the clock distribution system is prepared for the low power mode entry.
* This macro is silicon dependent as PSoC 5 devices have no CPU clock divider
* and PSoC 3 devices have different placement of the CPU clock divider register
* bitfield.
*******************************************************************************/
#if(CY_PSOC3)
    #define CY_PM_GET_CPU_FREQ_MHZ \
                            ((uint32)(cyPmImoFreqReg2Mhz[CY_PM_FASTCLK_IMO_CR_REG & CY_PM_FASTCLK_IMO_CR_FREQ_MASK]) / \
                            ((uint8)(((CY_PM_CLKDIST_MSTR1_REG & CY_PM_CLKDIST_CPU_DIV_MASK) >> 4u) + 1u)))
#endif  /* (CY_PSOC3) */

#if(CY_PSOC5)

    /* CPU clock is directly derived from bus clock */
    #define     CY_PM_GET_CPU_FREQ_MHZ (cyPmImoFreqReg2Mhz[CY_PM_FASTCLK_IMO_CR_REG & CY_PM_FASTCLK_IMO_CR_FREQ_MASK])

#endif  /* (CY_PSOC5) */


/*******************************************************************************
* The low power mode entry is different for PSoC 3 and PSoC 5 devices. The low
* power modes in PSoC 5 devices are invoked by Wait-For-Interrupt (WFI)
* instruction. The ARM compilers has __wfi() intrinsic that inserts a WFI
* instruction into the instruction stream generated by the compiler. The GCC
* compiler has to execute assembly language instruction.
*******************************************************************************/
#if(CY_PSOC5)

    #if defined(__ARMCC_VERSION)    /* Instristic for Keil compilers */
        #define CY_PM_WFI       __wfi()
    #else   /* ASM for GCC & IAR */
        #define CY_PM_WFI       asm volatile ("WFI \n")
    #endif /* (__ARMCC_VERSION) */

#else

    #define CY_PM_WFI           CY_NOP

#endif /* (CY_PSOC5) */


/*******************************************************************************
* Macro for the wakeupTime argument of the CyPmAltAct() function. The FTW should
* be programmed manually for non PSoC 3 devices.
*******************************************************************************/
#if(CY_PSOC3)

    #define PM_ALT_ACT_FTW_INTERVAL(x)  ((uint8)((x) - CY_PM_FTW_INTERVAL_SHIFT))

#endif  /* (CY_PSOC3) */


/*******************************************************************************
* This macro defines the IMO frequency that will be set by CyPmSaveClocks()
* function based on Enable Fast IMO during Startup option from the DWR file.
* The CyPmSleep()/CyPmHibernate() will set IMO 12 MHz just before entering the
* low power mode and restore IMO back to the value set by CyPmSaveClocks()
* immediately on wakeup.
*******************************************************************************/

/* Enable Fast IMO during Startup - enabled */
#if(1u == CYDEV_CONFIGURATION_IMOENABLED)

    /* IMO will be configured to 48 MHz */
    #define CY_PM_IMO_FREQ_LPM      (CY_IMO_FREQ_48MHZ)

#else

    /* IMO will be configured to 12 MHz */
    #define CY_PM_IMO_FREQ_LPM      (CY_IMO_FREQ_12MHZ)

#endif  /* (1u == CYDEV_CONFIGURATION_IMOENABLED) */


typedef struct cyPmClockBackupStruct
{
    /* CyPmSaveClocks()/CyPmRestoreClocks() */
    uint8  enClkA;              /* Analog clocks enable         */
    uint8  enClkD;              /* Digital clocks enable        */
    uint8  masterClkSrc;        /* Master clock source          */
    uint8  imoFreq;             /* IMO frequency (reg's value)  */
    uint8  imoUsbClk;           /* IMO USB CLK (reg's value)    */
    uint8  flashWaitCycles;     /* Flash wait cycles            */
    uint8  imoEnable;           /* IMO enable in Active mode    */
    uint8  imoClkSrc;           /* The IMO output               */
    uint8  clkImoSrc;
    uint8  imo2x;               /* IMO doubler enable state     */
    uint8  clkSyncDiv;          /* Master clk divider           */
    uint16 clkBusDiv;           /* clk_bus divider              */
    uint8  pllEnableState;      /* PLL enable state             */
    uint8  xmhzEnableState;     /* XM HZ enable state           */
    uint8  clkDistDelay;        /* Delay for clk_bus and ACLKs  */

} CY_PM_CLOCK_BACKUP_STRUCT;


typedef struct cyPmBackupStruct
{
    uint8 iloPowerMode;         /* ILO power mode           */
    uint8 ilo1kEnable;          /* ILO 1K enable state      */
    uint8 ilo100kEnable;        /* ILO 100K enable state    */

    uint8 slpTrBypass;          /* Sleep Trim Bypass        */

    #if(CY_PSOC3)

        uint8 swvClkEnabled;    /* SWV clock enable state   */
        uint8 prt1Dm;           /* Ports drive mode configuration */
        uint8 hardwareBuzz;

    #endif  /* (CY_PSOC3)  */

    uint8 wakeupCfg0;       /* Wake up configuration 0  */
    uint8 wakeupCfg1;       /* Wake up configuration 1  */
    uint8 wakeupCfg2;       /* Wake up configuration 2  */

    uint8 wakeupTrim0;
    uint8 wakeupTrim1;

    uint8 scctData[28u];   /* SC/CT routing registers  */

    /* CyPmHviLviSaveDisable()/CyPmHviLviRestore() */
    uint8 lvidEn;
    uint8 lvidTrip;
    uint8 lviaEn;
    uint8 lviaTrip;
    uint8 hviaEn;
    uint8 lvidRst;
    uint8 lviaRst;

    uint8 imoActFreq;       /* Last moment IMO change   */
    uint8 imoActFreq12Mhz;  /* 12 MHz or not            */

    uint8 boostRefExt;      /* Boost reference selection */

} CY_PM_BACKUP_STRUCT;


/***************************************
* Registers
***************************************/

/* Power Mode Wakeup Trim Register 1 */
#define CY_PM_PWRSYS_WAKE_TR1_REG           (* (reg8 *) CYREG_PWRSYS_WAKE_TR1 )
#define CY_PM_PWRSYS_WAKE_TR1_PTR           (  (reg8 *) CYREG_PWRSYS_WAKE_TR1 )

/* Master clock Divider Value Register */
#define CY_PM_CLKDIST_MSTR0_REG             (* (reg8 *) CYREG_CLKDIST_MSTR0 )
#define CY_PM_CLKDIST_MSTR0_PTR             (  (reg8 *) CYREG_CLKDIST_MSTR0 )

/* Master Clock Configuration Register/CPU Divider Value */
#define CY_PM_CLKDIST_MSTR1_REG             (* (reg8 *) CYREG_CLKDIST_MSTR1 )
#define CY_PM_CLKDIST_MSTR1_PTR             (  (reg8 *) CYREG_CLKDIST_MSTR1 )

/* Clock distribution configuration Register */
#define CY_PM_CLKDIST_CR_REG                (* (reg8 *) CYREG_CLKDIST_CR )
#define CY_PM_CLKDIST_CR_PTR                (  (reg8 *) CYREG_CLKDIST_CR )

/* CLK_BUS LSB Divider Value Register */
#define CY_PM_CLK_BUS_LSB_DIV_REG           (* (reg8 *) CYREG_CLKDIST_BCFG0 )
#define CY_PM_CLK_BUS_LSB_DIV_PTR           (  (reg8 *) CYREG_CLKDIST_BCFG0 )

/* CLK_BUS MSB Divider Value Register */
#define CY_PM_CLK_BUS_MSB_DIV_REG           (* (reg8 *) CYREG_CLKDIST_BCFG1 )
#define CY_PM_CLK_BUS_MSB_DIV_PTR           (  (reg8 *) CYREG_CLKDIST_BCFG1 )

/* CLK_BUS Configuration Register */
#define CLK_BUS_CFG_REG                     (* (reg8 *) CYREG_CLKDIST_BCFG2 )
#define CLK_BUS_CFG_PTR                     (  (reg8 *) CYREG_CLKDIST_BCFG2 )

/* Power Mode Control/Status Register */
#define CY_PM_MODE_CSR_REG                  (* (reg8 *) CYREG_PM_MODE_CSR )
#define CY_PM_MODE_CSR_PTR                  (  (reg8 *) CYREG_PM_MODE_CSR )

/* Power System Control Register 1 */
#define CY_PM_PWRSYS_CR1_REG                (* (reg8 *) CYREG_PWRSYS_CR1 )
#define CY_PM_PWRSYS_CR1_PTR                (  (reg8 *) CYREG_PWRSYS_CR1 )

/* Power System Control Register 0 */
#define CY_PM_PWRSYS_CR0_REG                (* (reg8 *) CYREG_PWRSYS_CR0 )
#define CY_PM_PWRSYS_CR0_PTR                (  (reg8 *) CYREG_PWRSYS_CR0 )

/* Internal Low-speed Oscillator Control Register 0 */
#define CY_PM_SLOWCLK_ILO_CR0_REG           (* (reg8 *) CYREG_SLOWCLK_ILO_CR0 )
#define CY_PM_SLOWCLK_ILO_CR0_PTR           (  (reg8 *) CYREG_SLOWCLK_ILO_CR0 )

/* External 32kHz Crystal Oscillator Control Register */
#define CY_PM_SLOWCLK_X32_CR_REG            (* (reg8 *) CYREG_SLOWCLK_X32_CR )
#define CY_PM_SLOWCLK_X32_CR_PTR            (  (reg8 *) CYREG_SLOWCLK_X32_CR )

#if(CY_PSOC3)

    /* MLOGIC Debug Register */
    #define CY_PM_MLOGIC_DBG_REG            (* (reg8 *) CYREG_MLOGIC_DEBUG )
    #define CY_PM_MLOGIC_DBG_PTR            (  (reg8 *) CYREG_MLOGIC_DEBUG )

    /* Port Pin Configuration Register */
    #define CY_PM_PRT1_PC3_REG              (* (reg8 *) CYREG_PRT1_PC3 )
    #define CY_PM_PRT1_PC3_PTR              (  (reg8 *) CYREG_PRT1_PC3 )

#endif /* (CY_PSOC3) */


/* Sleep Regulator Trim Register */
#define CY_PM_PWRSYS_SLP_TR_REG         (* (reg8 *) CYREG_PWRSYS_SLP_TR )
#define CY_PM_PWRSYS_SLP_TR_PTR         (  (reg8 *) CYREG_PWRSYS_SLP_TR )


/* Reset System Control Register */
#define CY_PM_RESET_CR1_REG                 (* (reg8 *) CYREG_RESET_CR1 )
#define CY_PM_RESET_CR1_PTR                 (  (reg8 *) CYREG_RESET_CR1 )

/* Power Mode Wakeup Trim Register 0 */
#define CY_PM_PWRSYS_WAKE_TR0_REG           (* (reg8 *) CYREG_PWRSYS_WAKE_TR0 )
#define CY_PM_PWRSYS_WAKE_TR0_PTR           (  (reg8 *) CYREG_PWRSYS_WAKE_TR0 )

#if(CY_PSOC3)

    /* Power Mode Wakeup Trim Register 2 */
    #define CY_PM_PWRSYS_WAKE_TR2_REG           (* (reg8 *) CYREG_PWRSYS_WAKE_TR2 )
    #define CY_PM_PWRSYS_WAKE_TR2_PTR           (  (reg8 *) CYREG_PWRSYS_WAKE_TR2 )

#endif  /* (CY_PSOC3) */

/* Power Manager Interrupt Status Register */
#define CY_PM_INT_SR_REG                    (* (reg8 *) CYREG_PM_INT_SR )
#define CY_PM_INT_SR_PTR                    (  (reg8 *) CYREG_PM_INT_SR )

/* Active Power Mode Configuration Register 0 */
#define CY_PM_ACT_CFG0_REG                  (* (reg8 *) CYREG_PM_ACT_CFG0 )
#define CY_PM_ACT_CFG0_PTR                  (  (reg8 *) CYREG_PM_ACT_CFG0 )

/* Active Power Mode Configuration Register 1 */
#define CY_PM_ACT_CFG1_REG                  (* (reg8 *) CYREG_PM_ACT_CFG1 )
#define CY_PM_ACT_CFG1_PTR                  (  (reg8 *) CYREG_PM_ACT_CFG1 )

/* Active Power Mode Configuration Register 2 */
#define CY_PM_ACT_CFG2_REG                  (* (reg8 *) CYREG_PM_ACT_CFG2 )
#define CY_PM_ACT_CFG2_PTR                  (  (reg8 *) CYREG_PM_ACT_CFG2 )

/* Boost Control 1 */
#define CY_PM_BOOST_CR1_REG                 (* (reg8 *) CYREG_BOOST_CR1 )
#define CY_PM_BOOST_CR1_PTR                 (  (reg8 *) CYREG_BOOST_CR1 )

/* Timewheel Configuration Register 0 */
#define CY_PM_TW_CFG0_REG                   (* (reg8 *) CYREG_PM_TW_CFG0 )
#define CY_PM_TW_CFG0_PTR                   (  (reg8 *) CYREG_PM_TW_CFG0 )

/* Timewheel Configuration Register 1 */
#define CY_PM_TW_CFG1_REG                   (* (reg8 *) CYREG_PM_TW_CFG1 )
#define CY_PM_TW_CFG1_PTR                   (  (reg8 *) CYREG_PM_TW_CFG1 )

/* Timewheel Configuration Register 2 */
#define CY_PM_TW_CFG2_REG                   (* (reg8 *) CYREG_PM_TW_CFG2 )
#define CY_PM_TW_CFG2_PTR                   (  (reg8 *) CYREG_PM_TW_CFG2 )

/* PLL Status Register */
#define CY_PM_FASTCLK_PLL_SR_REG            (*(reg8 *) CYREG_FASTCLK_PLL_SR )
#define CY_PM_FASTCLK_PLL_SR_PTR            ( (reg8 *) CYREG_FASTCLK_PLL_SR )

/* Internal Main Oscillator Control Register */
#define CY_PM_FASTCLK_IMO_CR_REG            (* (reg8 *) CYREG_FASTCLK_IMO_CR )
#define CY_PM_FASTCLK_IMO_CR_PTR            (  (reg8 *) CYREG_FASTCLK_IMO_CR )

/* PLL Configuration Register */
#define CY_PM_FASTCLK_PLL_CFG0_REG          (* (reg8 *) CYREG_FASTCLK_PLL_CFG0 )
#define CY_PM_FASTCLK_PLL_CFG0_PTR          (  (reg8 *) CYREG_FASTCLK_PLL_CFG0 )

/* External 4-33 MHz Crystal Oscillator Status and Control Register */
#define CY_PM_FASTCLK_XMHZ_CSR_REG          (* (reg8 *) CYREG_FASTCLK_XMHZ_CSR )
#define CY_PM_FASTCLK_XMHZ_CSR_PTR          (  (reg8 *) CYREG_FASTCLK_XMHZ_CSR )

/* Delay block Configuration Register */
#define CY_PM_CLKDIST_DELAY_REG             (* (reg8 *) CYREG_CLKDIST_DLY1 )
#define CY_PM_CLKDIST_DELAY_PTR             (  (reg8 *) CYREG_CLKDIST_DLY1 )


#if(CY_PSOC3)

    /* Cache Control Register */
    #define CY_PM_CACHE_CR_REG              (* (reg8 *) CYREG_CACHE_CR )
    #define CY_PM_CACHE_CR_PTR              (  (reg8 *) CYREG_CACHE_CR )

#else   /* Device is PSoC 5 */

    /* Cache Control Register */
    #define CY_PM_CACHE_CR_REG              (* (reg8 *) CYREG_CACHE_CC_CTL )
    #define CY_PM_CACHE_CR_PTR              (  (reg8 *) CYREG_CACHE_CC_CTL )

#endif  /* (CY_PSOC3) */


/* Power Mode Wakeup Mask Configuration Register 0 */
#define CY_PM_WAKEUP_CFG0_REG           (* (reg8 *) CYREG_PM_WAKEUP_CFG0 )
#define CY_PM_WAKEUP_CFG0_PTR           (  (reg8 *) CYREG_PM_WAKEUP_CFG0 )

/* Power Mode Wakeup Mask Configuration Register 1 */
#define CY_PM_WAKEUP_CFG1_REG           (* (reg8 *) CYREG_PM_WAKEUP_CFG1 )
#define CY_PM_WAKEUP_CFG1_PTR           (  (reg8 *) CYREG_PM_WAKEUP_CFG1 )

/* Power Mode Wakeup Mask Configuration Register 2 */
#define CY_PM_WAKEUP_CFG2_REG           (* (reg8 *) CYREG_PM_WAKEUP_CFG2 )
#define CY_PM_WAKEUP_CFG2_PTR           (  (reg8 *) CYREG_PM_WAKEUP_CFG2 )

/* Boost Control 2 */
#define CY_PM_BOOST_CR2_REG           (* (reg8 *) CYREG_BOOST_CR2 )
#define CY_PM_BOOST_CR2_PTR           (  (reg8 *) CYREG_BOOST_CR2 )

#if(CY_PSOC3)

    /* Interrrupt Controller Configuration and Status Register */
    #define CY_PM_INTC_CSR_EN_REG           (* (reg8 *) CYREG_INTC_CSR_EN )
    #define CY_PM_INTC_CSR_EN_PTR           (  (reg8 *) CYREG_INTC_CSR_EN )

#endif  /* (CY_PSOC3) */


/***************************************
* Register Constants
***************************************/

/* Internal Main Oscillator Control Register */

#define CY_PM_FASTCLK_IMO_CR_FREQ_MASK  (0x07u)    /* IMO frequency mask    */
#define CY_PM_FASTCLK_IMO_CR_FREQ_12MHZ (0x00u)    /* IMO frequency 12 MHz  */
#define CY_PM_FASTCLK_IMO_CR_F2XON      (0x10u)    /* IMO doubler enable    */
#define CY_PM_FASTCLK_IMO_CR_USB        (0x40u)    /* IMO is in USB mode    */

#define CY_PM_MASTER_CLK_SRC_IMO        (0u)
#define CY_PM_MASTER_CLK_SRC_PLL        (1u)
#define CY_PM_MASTER_CLK_SRC_XTAL       (2u)
#define CY_PM_MASTER_CLK_SRC_DSI        (3u)
#define CY_PM_MASTER_CLK_SRC_MASK       (3u)

#define CY_PM_PLL_CFG0_ENABLE           (0x01u)     /* PLL enable             */
#define CY_PM_PLL_STATUS_LOCK           (0x01u)     /* PLL Lock Status        */
#define CY_PM_XMHZ_CSR_ENABLE           (0x01u)     /* Enable X MHz OSC       */
#define CY_PM_XMHZ_CSR_XERR             (0x80u)     /* High indicates failure */
#define CY_PM_BOOST_ENABLE              (0x08u)     /* Boost enable           */
#define CY_PM_ILO_CR0_EN_1K             (0x02u)     /* Enable 1kHz ILO        */
#define CY_PM_ILO_CR0_EN_100K           (0x04u)     /* Enable 100kHz ILO      */
#define CY_PM_ILO_CR0_PD_MODE           (0x10u)     /* Power down mode for ILO*/
#define CY_PM_X32_CR_X32EN              (0x01u)     /* Enable 32kHz OSC       */

#define CY_PM_CTW_IE                    (0x08u)     /* CTW interrupt enable   */
#define CY_PM_CTW_EN                    (0x04u)     /* CTW enable             */
#define CY_PM_FTW_IE                    (0x02u)     /* FTW interrupt enable   */
#define CY_PM_FTW_EN                    (0x01u)     /* FTW enable             */
#define CY_PM_1PPS_EN                   (0x10u)     /* 1PPS enable            */
#define CY_PM_1PPS_IE                   (0x20u)     /* 1PPS interrupt enable  */


#define CY_PM_ACT_EN_CLK_A_MASK         (0x0Fu)
#define CY_PM_ACT_EN_CLK_D_MASK         (0xFFu)

#define CY_PM_DIV_BY_ONE                (0x00u)

/* Internal Main Oscillator Control Register */
#define CY_PM_FASTCLK_IMO_CR_XCLKEN     (0x20u)

/* Clock distribution configuration Register */
#define CY_PM_CLKDIST_IMO_OUT_MASK      (0x30u)
#define CY_PM_CLKDIST_IMO_OUT_IMO       (0x00u)
#define CY_PM_CLKDIST_IMO2X_SRC         (0x40u)

#define CY_PM_CLKDIST_PLL_SRC_MASK      (0x03u)
#define CY_PM_CLKDIST_PLL_SRC_IMO       (0x00u)
#define CY_PM_CLKDIST_PLL_SRC_XTAL      (0x01u)
#define CY_PM_CLKDIST_PLL_SRC_DSI       (0x02u)

/* Waiting for hibernate/sleep regulator to stabilize */
#define CY_PM_MODE_CSR_PWRUP_PULSE_Q    (0x08u)

#define CY_PM_MODE_CSR_ACTIVE           (0x00u)     /* Active power mode      */
#define CY_PM_MODE_CSR_ALT_ACT          (0x01u)     /* Alternate Active power */
#define CY_PM_MODE_CSR_SLEEP            (0x03u)     /* Sleep power mode       */
#define CY_PM_MODE_CSR_HIBERNATE        (0x04u)     /* Hibernate power mode   */
#define CY_PM_MODE_CSR_MASK             (0x07u)

/* I2C regulator backup enable */
#define CY_PM_PWRSYS_CR1_I2CREG_BACKUP  (0x04u)

/* When set, prepares system to disable LDO-A */
#define CY_PM_PWRSYS_CR1_LDOA_ISO       (0x01u)

/* When set, disables analog LDO regulator */
#define CY_PM_PWRSYS_CR1_LDOA_DIS       (0x02u)

#define CY_PM_PWRSYS_WAKE_TR2_VCCD_CLK_DET  (0x04u)

#define CY_PM_FTW_INT                   (0x01u)     /* FTW event has occured  */
#define CY_PM_CTW_INT                   (0x02u)     /* CTW event has occured  */
#define CY_PM_ONEPPS_INT                (0x04u)     /* 1PPS event has occured */

/* Active Power Mode Configuration Register 0 */
#define CY_PM_ACT_CFG0_IMO              (0x10u)     /* IMO enable in Active */

/* Cache Control Register (same mask for all device revisions) */
#define CY_PM_CACHE_CR_CYCLES_MASK      (0xC0u)

/* Bus Clock divider to divide-by-one */
#define CY_PM_BUS_CLK_DIV_BY_ONE        (0x00u)

/* HVI/LVI feature on external analog and digital supply mask */
#define CY_PM_RESET_CR1_HVI_LVI_EN_MASK (0x07u)

/* High-voltage-interrupt feature on external analog supply */
#define CY_PM_RESET_CR1_HVIA_EN         (0x04u)

/* Low-voltage-interrupt feature on external analog supply */
#define CY_PM_RESET_CR1_LVIA_EN         (0x02u)

/* Low-voltage-interrupt feature on external digital supply */
#define CY_PM_RESET_CR1_LVID_EN         (0x01u)

/* Allows system to program delays on clk_sync_d */
#define CY_PM_CLKDIST_DELAY_EN          (0x04u)


#define CY_PM_WAKEUP_SRC_CMPS_MASK      (0x000Fu)

/* Holdoff mask sleep trim */
#define CY_PM_PWRSYS_SLP_TR_HIBSLP_HOLDOFF_MASK     (0x1Fu)

#if(CY_PSOC3)

    /* CPU clock divider mask */
    #define CY_PM_CLKDIST_CPU_DIV_MASK          (0xF0u)

    /* Serial Wire View (SWV) clock enable */
    #define CY_PM_MLOGIC_DBG_SWV_CLK_EN         (0x04u)

    /* Port drive mode */
    #define CY_PM_PRT1_PC3_DM_MASK              (0xf1u)

    /* Mode 6, stong pull-up, strong pull-down */
    #define CY_PM_PRT1_PC3_DM_STRONG            (0x0Cu)

    /* When set, enables buzz wakeups */
    #define CY_PM_PWRSYS_WAKE_TR2_EN_BUZZ       (0x01u)

#endif  /* (CY_PSOC3) */


/* Disables sleep regulator and shorts vccd to vpwrsleep */
#define CY_PM_PWRSYS_SLP_TR_BYPASS          (0x10u)

/* Boost Control 2: Select external precision reference */
#define CY_PM_BOOST_CR2_EREFSEL_EXT     (0x08u)

#if(CY_PSOC3)

    #define CY_PM_PWRSYS_WAKE_TR0       (0xFFu)
    #define CY_PM_PWRSYS_WAKE_TR1       (0x90u)

#endif  /* (CY_PSOC3) */

#if(CY_PSOC5)

    #define CY_PM_PWRSYS_WAKE_TR0       (0xFFu)
    #define CY_PM_PWRSYS_WAKE_TR1       (0xB0u)

#endif  /* (CY_PSOC5) */

#if(CY_PSOC3)

    /* Interrrupt Controller Configuration and Status Register */
    #define CY_PM_INTC_CSR_EN_CLK       (0x01u)

#endif  /* (CY_PSOC3) */


/*******************************************************************************
* Lock Status Flag. If lock is acquired this flag will stay set (regardless of
* whether lock is subsequently lost) until it is read. Upon reading it will
* clear. If lock is still true then the bit will simply set again. If lock
* happens to be false when the clear on read occurs then the bit will stay
* cleared until the next lock event.
*******************************************************************************/
#define CY_PM_FASTCLK_PLL_LOCKED       (0x01u)


/*******************************************************************************
* The following code is OBSOLETE and must not be used starting with cy_boot 3.30
*
* If the obsoleted macro definitions intended for use in the application use the
* following scheme, redefine your own versions of these definitions:
*    #ifdef <OBSOLETED_DEFINE>
*        #undef  <OBSOLETED_DEFINE>
*        #define <OBSOLETED_DEFINE>      (<New Value>)
*    #endif
*
* Note: Redefine obsoleted macro definitions with caution. They might still be
*       used in the application and their modification might lead to unexpected
*       consequences.
*******************************************************************************/
#if(CY_PSOC3)

    /* Was removed as redundant */
    #define CY_PM_FTW_INTERVAL_MASK    (0xFFu)

#endif  /* (CY_PSOC3) */

/* Was removed as redundant */
#define CY_PM_CTW_INTERVAL_MASK         (0x0Fu)

#endif  /* (CY_BOOT_CYPM_H) */


/* [] END OF FILE */
