/*******************************************************************************
* File Name: CyLib.h
* Version 4.20
*
* Description:
*  Provides the function definitions for the system, clocking, interrupts and
*  watchdog timer API.
*
* Note:
*  Documentation of the API's in this file is located in the System Reference
*  Guide provided with PSoC Creator.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYLIB_H)
#define CY_BOOT_CYLIB_H

#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "cytypes.h"
#include "cyfitter.h"
#include "cydevice_trm.h"
#include "cyPm.h"

#if(CY_PSOC3)
    #include <PSoC3_8051.h>
#endif  /* (CY_PSOC3) */


#if(CYDEV_VARIABLE_VDDA == 1)

    #include "CyScBoostClk.h"

#endif  /* (CYDEV_VARIABLE_VDDA == 1) */


/* Global variable with preserved reset status */
extern uint8 CYXDATA CyResetStatus;


/* Variable Vdda */
#if(CYDEV_VARIABLE_VDDA == 1)

    extern uint8 CyScPumpEnabled;

#endif  /* (CYDEV_VARIABLE_VDDA == 1) */


/* Do not use these definitions directly in your application */
extern uint32 cydelay_freq_hz;
extern uint32 cydelay_freq_khz;
extern uint8  cydelay_freq_mhz;
extern uint32 cydelay_32k_ms;


/***************************************
*    Function Prototypes
***************************************/
cystatus CyPLL_OUT_Start(uint8 wait) ;
void  CyPLL_OUT_Stop(void) ;
void  CyPLL_OUT_SetPQ(uint8 pDiv, uint8 qDiv, uint8 current) ;
void  CyPLL_OUT_SetSource(uint8 source) ;

void  CyIMO_Start(uint8 wait) ;
void  CyIMO_Stop(void) ;
void  CyIMO_SetFreq(uint8 freq) ;
void  CyIMO_SetSource(uint8 source) ;
void  CyIMO_EnableDoubler(void) ;
void  CyIMO_DisableDoubler(void) ;

void  CyMasterClk_SetSource(uint8 source) ;
void  CyMasterClk_SetDivider(uint8 divider) ;
void  CyBusClk_SetDivider(uint16 divider) ;

#if(CY_PSOC3)
    void  CyCpuClk_SetDivider(uint8 divider) ;
#endif  /* (CY_PSOC3) */

void  CyUsbClk_SetSource(uint8 source) ;

void  CyILO_Start1K(void) ;
void  CyILO_Stop1K(void) ;
void  CyILO_Start100K(void) ;
void  CyILO_Stop100K(void) ;
void  CyILO_Enable33K(void) ;
void  CyILO_Disable33K(void) ;
void  CyILO_SetSource(uint8 source) ;
uint8 CyILO_SetPowerMode(uint8 mode) ;

uint8 CyXTAL_32KHZ_ReadStatus(void) ;
uint8 CyXTAL_32KHZ_SetPowerMode(uint8 mode) ;
void  CyXTAL_32KHZ_Start(void) ;
void  CyXTAL_32KHZ_Stop(void) ;

cystatus CyXTAL_Start(uint8 wait) ;
void  CyXTAL_Stop(void) ;
void  CyXTAL_SetStartup(uint8 setting) ;

void  CyXTAL_EnableErrStatus(void) ;
void  CyXTAL_DisableErrStatus(void) ;
uint8 CyXTAL_ReadStatus(void) ;
void  CyXTAL_EnableFaultRecovery(void) ;
void  CyXTAL_DisableFaultRecovery(void) ;

void CyXTAL_SetFbVoltage(uint8 setting) ;
void CyXTAL_SetWdVoltage(uint8 setting) ;

void CyWdtStart(uint8 ticks, uint8 lpMode) ;
void CyWdtClear(void) ;

/* System Function Prototypes */
void CyDelay(uint32 milliseconds) CYREENTRANT;
void CyDelayUs(uint16 microseconds);
void CyDelayFreq(uint32 freq) CYREENTRANT;
void CyDelayCycles(uint32 cycles);

void CySoftwareReset(void) ;

uint8 CyEnterCriticalSection(void);
void CyExitCriticalSection(uint8 savedIntrStatus);
void CyHalt(uint8 reason) CYREENTRANT;


/* Interrupt Function Prototypes */
#if(CY_PSOC5)
    cyisraddress CyIntSetSysVector(uint8 number, cyisraddress address)  ;
    cyisraddress CyIntGetSysVector(uint8 number) ;
#endif  /* (CY_PSOC5) */

cyisraddress CyIntSetVector(uint8 number, cyisraddress address) ;
cyisraddress CyIntGetVector(uint8 number) ;

void  CyIntSetPriority(uint8 number, uint8 priority) ;
uint8 CyIntGetPriority(uint8 number) ;

uint8 CyIntGetState(uint8 number) ;

uint32 CyDisableInts(void) ;
void CyEnableInts(uint32 mask) ;


#if(CY_PSOC5)
    void CyFlushCache(void);
#endif  /* (CY_PSOC5) */


/* Voltage Detection Function Prototypes */
void CyVdLvDigitEnable(uint8 reset, uint8 threshold) ;
void CyVdLvAnalogEnable(uint8 reset, uint8 threshold) ;
void CyVdLvDigitDisable(void) ;
void CyVdLvAnalogDisable(void) ;
void CyVdHvAnalogEnable(void) ;
void CyVdHvAnalogDisable(void) ;
uint8 CyVdStickyStatus(uint8 mask) ;
uint8 CyVdRealTimeStatus(void) ;

void CySetScPumps(uint8 enable) ;

#if(CY_PSOC5)
    /* Default interrupt handler */
    CY_ISR_PROTO(IntDefaultHandler);
#endif  /* (CY_PSOC5) */

#if(CY_PSOC5)
    /* System tick timer APIs */
    typedef void (*cySysTickCallback)(void);

    void CySysTickStart(void);
    void CySysTickInit(void);
    void CySysTickEnable(void);
    void CySysTickStop(void);
    void CySysTickEnableInterrupt(void);
    void CySysTickDisableInterrupt(void);
    void CySysTickSetReload(uint32 value);
    uint32 CySysTickGetReload(void);
    uint32 CySysTickGetValue(void);
    cySysTickCallback CySysTickSetCallback(uint32 number, cySysTickCallback function);
    cySysTickCallback CySysTickGetCallback(uint32 number);
    void CySysTickSetClockSource(uint32 clockSource);
    uint32 CySysTickGetCountFlag(void);
    void CySysTickClear(void);
#endif  /* (CY_PSOC5) */

/***************************************
* API Constants
***************************************/


/*******************************************************************************
* PLL API Constants
*******************************************************************************/
#define CY_CLK_PLL_ENABLE               (0x01u)
#define CY_CLK_PLL_LOCK_STATUS          (0x01u)

#define CY_CLK_PLL_FTW_INTERVAL         (24u)

#define CY_CLK_PLL_MAX_Q_VALUE          (16u)
#define CY_CLK_PLL_MIN_Q_VALUE          (1u)
#define CY_CLK_PLL_MIN_P_VALUE          (8u)
#define CY_CLK_PLL_MIN_CUR_VALUE        (1u)
#define CY_CLK_PLL_MAX_CUR_VALUE        (7u)

#define CY_CLK_PLL_CURRENT_POSITION     (4u)
#define CY_CLK_PLL_CURRENT_MASK         (0x8Fu)


/*******************************************************************************
* External 32kHz Crystal Oscillator API Constants
*******************************************************************************/
#define CY_XTAL32K_ANA_STAT             (0x20u)

#define CY_CLK_XTAL32_CR_LPM            (0x02u)
#define CY_CLK_XTAL32_CR_EN             (0x01u)
#if(CY_PSOC3)
    #define CY_CLK_XTAL32_CR_PDBEN      (0x04u)
#endif  /* (CY_PSOC3) */

#define CY_CLK_XTAL32_TR_MASK           (0x07u)
#define CY_CLK_XTAL32_TR_STARTUP        (0x03u)
#define CY_CLK_XTAL32_TR_HIGH_POWER     (0x06u)
#define CY_CLK_XTAL32_TR_LOW_POWER      (0x01u)
#define CY_CLK_XTAL32_TR_POWERDOWN      (0x00u)

#define CY_CLK_XTAL32_TST_DEFAULT       (0xF3u)

#define CY_CLK_XTAL32_CFG_LP_DEFAULT    (0x04u)
#define CY_CLK_XTAL32_CFG_LP_LOWPOWER   (0x08u)
#define CY_CLK_XTAL32_CFG_LP_MASK       (0x0Cu)

#define CY_CLK_XTAL32_CFG_LP_ALLOW      (0x80u)


/*******************************************************************************
* External MHz Crystal Oscillator API Constants
*******************************************************************************/
#define CY_CLK_XMHZ_FTW_INTERVAL        (24u)
#define CY_CLK_XMHZ_MIN_TIMEOUT         (130u)

#define CY_CLK_XMHZ_CSR_ENABLE          (0x01u)
#define CY_CLK_XMHZ_CSR_XERR            (0x80u)
#define CY_CLK_XMHZ_CSR_XFB             (0x04u)
#define CY_CLK_XMHZ_CSR_XPROT           (0x40u)

#define CY_CLK_XMHZ_CFG0_XCFG_MASK      (0x1Fu)
#define CY_CLK_XMHZ_CFG1_VREF_FB_MASK   (0x0Fu)
#define CY_CLK_XMHZ_CFG1_VREF_WD_MASK   (0x70u)


/*******************************************************************************
* Watchdog Timer API Constants
*******************************************************************************/
#define CYWDT_2_TICKS               (0x0u)     /*    4 -    6 ms */
#define CYWDT_16_TICKS              (0x1u)     /*   32 -   48 ms */
#define CYWDT_128_TICKS             (0x2u)     /*  256 -  384 ms */
#define CYWDT_1024_TICKS            (0x3u)     /* 2048 - 3072 ms */

#define CYWDT_LPMODE_NOCHANGE       (0x00u)
#define CYWDT_LPMODE_MAXINTER       (0x01u)
#define CYWDT_LPMODE_DISABLED       (0x03u)

#define CY_WDT_CFG_INTERVAL_MASK    (0x03u)
#define CY_WDT_CFG_CTW_RESET        (0x80u)
#define CY_WDT_CFG_LPMODE_SHIFT     (5u)
#define CY_WDT_CFG_LPMODE_MASK      (0x60u)
#define CY_WDT_CFG_WDR_EN           (0x10u)
#define CY_WDT_CFG_CLEAR_ALL        (0x00u)
#define CY_WDT_CR_FEED              (0x01u)


/*******************************************************************************
*    Voltage Detection API Constants
*******************************************************************************/

#define CY_VD_LVID_EN                (0x01u)
#define CY_VD_LVIA_EN                (0x02u)
#define CY_VD_HVIA_EN                (0x04u)

#define CY_VD_PRESD_EN               (0x40u)
#define CY_VD_PRESA_EN               (0x80u)

#define CY_VD_LVID                   (0x01u)
#define CY_VD_LVIA                   (0x02u)
#define CY_VD_HVIA                   (0x04u)

#define CY_VD_LVI_TRIP_LVID_MASK     (0x0Fu)


/*******************************************************************************
*    Variable VDDA API Constants
*******************************************************************************/
#if(CYDEV_VARIABLE_VDDA == 1)

    /* Active Power Mode Configuration Register 9 */
    #define CY_LIB_ACT_CFG9_SWCAP0_EN        (0x01u)
    #define CY_LIB_ACT_CFG9_SWCAP1_EN        (0x02u)
    #define CY_LIB_ACT_CFG9_SWCAP2_EN        (0x04u)
    #define CY_LIB_ACT_CFG9_SWCAP3_EN        (0x08u)
    #define CY_LIB_ACT_CFG9_SWCAPS_MASK      (0x0Fu)

    /* Switched Cap Miscellaneous Control Register */
    #define CY_LIB_SC_MISC_PUMP_FORCE        (0x20u)

    /* Switched Capacitor 0 Boost Clock Selection Register */
    #define CY_LIB_SC_BST_CLK_EN             (0x08u)
    #define CY_LIB_SC_BST_CLK_INDEX_MASK     (0xF8u)

#endif  /* (CYDEV_VARIABLE_VDDA == 1) */


/*******************************************************************************
* Clock Distribution API Constants
*******************************************************************************/
#define CY_LIB_CLKDIST_AMASK_MASK       (0xF0u)
#define CY_LIB_CLKDIST_DMASK_MASK       (0x00u)
#define CY_LIB_CLKDIST_LD_LOAD          (0x01u)
#define CY_LIB_CLKDIST_BCFG2_MASK       (0x80u)
#define CY_LIB_CLKDIST_MASTERCLK_DIV    (7u)
#define CY_LIB_CLKDIST_BCFG2_SSS        (0x40u)
#define CY_LIB_CLKDIST_MSTR1_SRC_MASK   (0xFCu)
#define CY_LIB_FASTCLK_IMO_DOUBLER      (0x10u)
#define CY_LIB_FASTCLK_IMO_IMO          (0x20u)
#define CY_LIB_CLKDIST_CR_IMO2X         (0x40u)
#define CY_LIB_FASTCLK_IMO_CR_RANGE_MASK (0xF8u)

#define CY_LIB_CLKDIST_CR_PLL_SCR_MASK  (0xFCu)


/* CyILO_SetPowerMode() */
#define CY_ILO_CONTROL_PD_MODE          (0x10u)
#define CY_ILO_CONTROL_PD_POSITION      (4u)

#define CY_ILO_SOURCE_100K              (0u)
#define CY_ILO_SOURCE_33K               (1u)
#define CY_ILO_SOURCE_1K                (2u)

#define CY_ILO_FAST_START               (0u)
#define CY_ILO_SLOW_START               (1u)

#define CY_ILO_SOURCE_BITS_CLEAR        (0xF3u)
#define CY_ILO_SOURCE_1K_SET            (0x08u)
#define CY_ILO_SOURCE_33K_SET           (0x04u)
#define CY_ILO_SOURCE_100K_SET          (0x00u)

#define CY_MASTER_SOURCE_IMO            (0u)
#define CY_MASTER_SOURCE_PLL            (1u)
#define CY_MASTER_SOURCE_XTAL           (2u)
#define CY_MASTER_SOURCE_DSI            (3u)

#define CY_IMO_SOURCE_IMO               (0u)
#define CY_IMO_SOURCE_XTAL              (1u)
#define CY_IMO_SOURCE_DSI               (2u)


/* CyIMO_Start() */
#define CY_LIB_PM_ACT_CFG0_IMO_EN       (0x10u)
#define CY_LIB_PM_STBY_CFG0_IMO_EN      (0x10u)
#define CY_LIB_CLK_IMO_FTW_TIMEOUT      (0x00u)

#define CY_LIB_IMO_3MHZ_VALUE           (0x03u)
#define CY_LIB_IMO_6MHZ_VALUE           (0x01u)
#define CY_LIB_IMO_12MHZ_VALUE          (0x00u)
#define CY_LIB_IMO_24MHZ_VALUE          (0x02u)
#define CY_LIB_IMO_48MHZ_VALUE          (0x04u)
#define CY_LIB_IMO_62MHZ_VALUE          (0x05u)
#define CY_LIB_IMO_74MHZ_VALUE          (0x06u)


/* CyIMO_SetFreq() */
#define CY_IMO_FREQ_3MHZ                (0u)
#define CY_IMO_FREQ_6MHZ                (1u)
#define CY_IMO_FREQ_12MHZ               (2u)
#define CY_IMO_FREQ_24MHZ               (3u)
#define CY_IMO_FREQ_48MHZ               (4u)
#define CY_IMO_FREQ_62MHZ               (5u)
#if(CY_PSOC5)
    #define CY_IMO_FREQ_74MHZ           (6u)
#endif  /* (CY_PSOC5) */
#define CY_IMO_FREQ_USB                 (8u)

#define CY_LIB_IMO_USBCLK_ON_SET        (0x40u)


/* CyCpuClk_SetDivider() */
#define CY_LIB_CLKDIST_DIV_POSITION     (4u)
#define CY_LIB_CLKDIST_MSTR1_DIV_MASK   (0x0Fu)


/* CyIMO_SetTrimValue() */
#define CY_LIB_USB_CLK_EN               (0x02u)


/* CyPLL_OUT_SetSource() - parameters */
#define CY_PLL_SOURCE_IMO               (0u)
#define CY_PLL_SOURCE_XTAL              (1u)
#define CY_PLL_SOURCE_DSI               (2u)


/* CyILO_[Start|Stop][1|100K](), CyILO_[Enable|Disable]33K() */
#define CY_LIB_SLOWCLK_ILO_CR0_EN_1KHZ   (0x02u)
#define CY_LIB_SLOWCLK_ILO_CR0_EN_33KHZ  (0x20u)
#define CY_LIB_SLOWCLK_ILO_CR0_EN_100KHZ (0x04u)


/* CyUsbClk_SetSource() */
#define CY_LIB_CLKDIST_UCFG_SRC_SEL_MASK (0x03u)


/* CyUsbClk_SetSource() - parameters */
#define CY_LIB_USB_CLK_IMO2X            (0x00u)
#define CY_LIB_USB_CLK_IMO              (0x01u)
#define CY_LIB_USB_CLK_PLL              (0x02u)
#define CY_LIB_USB_CLK_DSI              (0x03u)


/* CyUSB_PowerOnCheck() */
#define CY_ACT_USB_ENABLED              (0x01u)
#define CY_ALT_ACT_USB_ENABLED          (0x01u)


#if(CY_PSOC5)

    /***************************************************************************
    * Instruction Synchronization Barrier flushes the pipeline in the processor,
    * so that all instructions following the ISB are fetched from cache or
    * memory, after the instruction has been completed.
    ***************************************************************************/

    #if defined(__ARMCC_VERSION)
        #define CY_SYS_ISB       __isb(0x0f)
    #else   /* ASM for GCC & IAR */
        #define CY_SYS_ISB       asm volatile ("isb \n")
    #endif /* (__ARMCC_VERSION) */

#endif /* (CY_PSOC5) */


/***************************************
* Registers
***************************************/


/*******************************************************************************
* System Registers
*******************************************************************************/

/* Software Reset Control Register */
#define CY_LIB_RESET_CR2_REG         (* (reg8 *) CYREG_RESET_CR2)
#define CY_LIB_RESET_CR2_PTR         (  (reg8 *) CYREG_RESET_CR2)

/* Timewheel Configuration Register 0 */
#define CY_LIB_PM_TW_CFG0_REG           (*(reg8 *) CYREG_PM_TW_CFG0)
#define CY_LIB_PM_TW_CFG0_PTR           ( (reg8 *) CYREG_PM_TW_CFG0)

/* Timewheel Configuration Register 2 */
#define CY_LIB_PM_TW_CFG2_REG           (*(reg8 *) CYREG_PM_TW_CFG2)
#define CY_LIB_PM_TW_CFG2_PTR           ( (reg8 *) CYREG_PM_TW_CFG2)

/* USB Configuration Register */
#define CY_LIB_CLKDIST_UCFG_REG         (*(reg8 *) CYREG_CLKDIST_UCFG)
#define CY_LIB_CLKDIST_UCFG_PTR         ( (reg8 *) CYREG_CLKDIST_UCFG)

/* Internal Main Oscillator Trim Register 1 */
#define CY_LIB_IMO_TR1_REG              (*(reg8 *) CYREG_IMO_TR1)
#define CY_LIB_IMO_TR1_PTR              ( (reg8 *) CYREG_IMO_TR1)

/* USB control 1 Register */
#define CY_LIB_USB_CR1_REG              (*(reg8 *) CYREG_USB_CR1 )
#define CY_LIB_USB_CR1_PTR              ( (reg8 *) CYREG_USB_CR1 )

/* Active Power Mode Configuration Register 0 */
#define CY_LIB_PM_ACT_CFG0_REG          (*(reg8 *) CYREG_PM_ACT_CFG0)
#define CY_LIB_PM_ACT_CFG0_PTR          ( (reg8 *) CYREG_PM_ACT_CFG0)

/* Standby Power Mode Configuration Register 0 */
#define CY_LIB_PM_STBY_CFG0_REG          (*(reg8 *) CYREG_PM_STBY_CFG0)
#define CY_LIB_PM_STBY_CFG0_PTR          ( (reg8 *) CYREG_PM_STBY_CFG0)

/* Active Power Mode Configuration Register 5 */
#define CY_LIB_PM_ACT_CFG5_REG              (* (reg8 *) CYREG_PM_ACT_CFG5 )
#define CY_LIB_PM_ACT_CFG5_PTR              (  (reg8 *) CYREG_PM_ACT_CFG5 )

/* Standby Power Mode Configuration Register 5 */
#define CY_LIB_PM_STBY_CFG5_REG             (* (reg8 *) CYREG_PM_STBY_CFG5 )
#define CY_LIB_PM_STBY_CFG5_PTR             (  (reg8 *) CYREG_PM_STBY_CFG5 )

/* CyIMO_SetTrimValue() */
#if(CY_PSOC3)
    #define CY_LIB_TRIM_IMO_3MHZ_PTR         ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_3MHZ)
    #define CY_LIB_TRIM_IMO_6MHZ_PTR         ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_6MHZ)
    #define CY_LIB_TRIM_IMO_12MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_12MHZ)
    #define CY_LIB_TRIM_IMO_24MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_24MHZ)
    #define CY_LIB_TRIM_IMO_67MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_67MHZ)
    #define CY_LIB_TRIM_IMO_80MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_80MHZ)
    #define CY_LIB_TRIM_IMO_USB_PTR          ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_USB)
    #define CY_LIB_TRIM_IMO_TR1_PTR          ((void far *) (CYREG_FLSHID_MFG_CFG_IMO_TR1 + 1u))
 #else
    #define CY_LIB_TRIM_IMO_3MHZ_PTR         ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_3MHZ)
    #define CY_LIB_TRIM_IMO_6MHZ_PTR         ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_6MHZ)
    #define CY_LIB_TRIM_IMO_12MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_12MHZ)
    #define CY_LIB_TRIM_IMO_24MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_24MHZ)
    #define CY_LIB_TRIM_IMO_67MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_67MHZ)
    #define CY_LIB_TRIM_IMO_80MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_80MHZ)
    #define CY_LIB_TRIM_IMO_USB_PTR          ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_USB)
    #define CY_LIB_TRIM_IMO_TR1_PTR          ((reg8 *) (CYREG_FLSHID_MFG_CFG_IMO_TR1 + 1u))
#endif  /* (CY_PSOC3) */


/*******************************************************************************
* PLL Registers
*******************************************************************************/

/* PLL Configuration Register 0 */
#define CY_CLK_PLL_CFG0_REG         (*(reg8 *) CYREG_FASTCLK_PLL_CFG0)
#define CY_CLK_PLL_CFG0_PTR         ( (reg8 *) CYREG_FASTCLK_PLL_CFG0)

/* PLL Configuration Register 1 */
#define CY_CLK_PLL_CFG1_REG         (*(reg8 *) CYREG_FASTCLK_PLL_CFG1)
#define CY_CLK_PLL_CFG1_PTR         ( (reg8 *) CYREG_FASTCLK_PLL_CFG1)

/* PLL Status Register */
#define CY_CLK_PLL_SR_REG           (*(reg8 *) CYREG_FASTCLK_PLL_SR)
#define CY_CLK_PLL_SR_PTR           ( (reg8 *) CYREG_FASTCLK_PLL_SR)

/* PLL Q-Counter Configuration Register */
#define CY_CLK_PLL_Q_REG            (*(reg8 *) CYREG_FASTCLK_PLL_Q)
#define CY_CLK_PLL_Q_PTR            ( (reg8 *) CYREG_FASTCLK_PLL_Q)

/* PLL P-Counter Configuration Register */
#define CY_CLK_PLL_P_REG            (*(reg8 *) CYREG_FASTCLK_PLL_P)
#define CY_CLK_PLL_P_PTR            ( (reg8 *) CYREG_FASTCLK_PLL_P)


/*******************************************************************************
* External MHz Crystal Oscillator Registers
*******************************************************************************/

/* External MHz Crystal Oscillator Status and Control Register */
#define CY_CLK_XMHZ_CSR_REG         (*(reg8 *) CYREG_FASTCLK_XMHZ_CSR)
#define CY_CLK_XMHZ_CSR_PTR         ( (reg8 *) CYREG_FASTCLK_XMHZ_CSR)

/* External MHz Crystal Oscillator Configuration Register 0 */
#define CY_CLK_XMHZ_CFG0_REG        (*(reg8 *) CYREG_FASTCLK_XMHZ_CFG0)
#define CY_CLK_XMHZ_CFG0_PTR        ( (reg8 *) CYREG_FASTCLK_XMHZ_CFG0)

/* External MHz Crystal Oscillator Configuration Register 1 */
#define CY_CLK_XMHZ_CFG1_REG        (*(reg8 *) CYREG_FASTCLK_XMHZ_CFG1)
#define CY_CLK_XMHZ_CFG1_PTR        ( (reg8 *) CYREG_FASTCLK_XMHZ_CFG1)


/*******************************************************************************
* External 32kHz Crystal Oscillator Registers
*******************************************************************************/

/* 32 kHz Watch Crystal Oscillator Trim Register */
#define CY_CLK_XTAL32_TR_REG        (*(reg8 *) CYREG_X32_TR)
#define CY_CLK_XTAL32_TR_PTR        ( (reg8 *) CYREG_X32_TR)

/* External 32kHz Crystal Oscillator Test Register */
#define CY_CLK_XTAL32_TST_REG       (*(reg8 *) CYREG_SLOWCLK_X32_TST)
#define CY_CLK_XTAL32_TST_PTR       ( (reg8 *) CYREG_SLOWCLK_X32_TST)

/* External 32kHz Crystal Oscillator Control Register */
#define CY_CLK_XTAL32_CR_REG        (*(reg8 *) CYREG_SLOWCLK_X32_CR)
#define CY_CLK_XTAL32_CR_PTR        ( (reg8 *) CYREG_SLOWCLK_X32_CR)

/* External 32kHz Crystal Oscillator Configuration Register */
#define CY_CLK_XTAL32_CFG_REG       (*(reg8 *) CYREG_SLOWCLK_X32_CFG)
#define CY_CLK_XTAL32_CFG_PTR       ( (reg8 *) CYREG_SLOWCLK_X32_CFG)


/*******************************************************************************
* Watchdog Timer Registers
*******************************************************************************/

/* Watchdog Timer Configuration Register */
#define CY_WDT_CFG_REG              (*(reg8 *) CYREG_PM_WDT_CFG)
#define CY_WDT_CFG_PTR              ( (reg8 *) CYREG_PM_WDT_CFG)

/* Watchdog Timer Control Register */
#define CY_WDT_CR_REG               (*(reg8 *) CYREG_PM_WDT_CR)
#define CY_WDT_CR_PTR               ( (reg8 *) CYREG_PM_WDT_CR)


/*******************************************************************************
*    LVI/HVI Registers
*******************************************************************************/

#define CY_VD_LVI_TRIP_REG          (* (reg8 *) CYREG_RESET_CR0)
#define CY_VD_LVI_TRIP_PTR          (  (reg8 *) CYREG_RESET_CR0)

#define CY_VD_LVI_HVI_CONTROL_REG   (* (reg8 *) CYREG_RESET_CR1)
#define CY_VD_LVI_HVI_CONTROL_PTR   (  (reg8 *) CYREG_RESET_CR1)

#define CY_VD_PRES_CONTROL_REG      (* (reg8 *) CYREG_RESET_CR3)
#define CY_VD_PRES_CONTROL_PTR      (  (reg8 *) CYREG_RESET_CR3)

#define CY_VD_PERSISTENT_STATUS_REG (* (reg8 *) CYREG_RESET_SR0)
#define CY_VD_PERSISTENT_STATUS_PTR (  (reg8 *) CYREG_RESET_SR0)

#define CY_VD_RT_STATUS_REG         (* (reg8 *) CYREG_RESET_SR2)
#define CY_VD_RT_STATUS_PTR         (  (reg8 *) CYREG_RESET_SR2)


/*******************************************************************************
*    Variable VDDA
*******************************************************************************/
#if(CYDEV_VARIABLE_VDDA == 1)

    /* Active Power Mode Configuration Register 9 */
    #define CY_LIB_ACT_CFG9_REG            (* (reg8 *) CYREG_PM_ACT_CFG9 )
    #define CY_LIB_ACT_CFG9_PTR            (  (reg8 *) CYREG_PM_ACT_CFG9 )

    /* Switched Capacitor 0 Boost Clock Selection Register */
    #define CY_LIB_SC0_BST_REG             (* (reg8 *) CYREG_SC0_BST )
    #define CY_LIB_SC0_BST_PTR             (  (reg8 *) CYREG_SC0_BST )

    /* Switched Capacitor 1 Boost Clock Selection Register */
    #define CY_LIB_SC1_BST_REG             (* (reg8 *) CYREG_SC1_BST )
    #define CY_LIB_SC1_BST_PTR             (  (reg8 *) CYREG_SC1_BST )

    /* Switched Capacitor 2 Boost Clock Selection Register */
    #define CY_LIB_SC2_BST_REG             (* (reg8 *) CYREG_SC2_BST )
    #define CY_LIB_SC2_BST_PTR             (  (reg8 *) CYREG_SC2_BST )

    /* Switched Capacitor 3 Boost Clock Selection Register */
    #define CY_LIB_SC3_BST_REG             (* (reg8 *) CYREG_SC3_BST )
    #define CY_LIB_SC3_BST_PTR             (  (reg8 *) CYREG_SC3_BST )

    /* Switched Cap Miscellaneous Control Register */
    #define CY_LIB_SC_MISC_REG             (* (reg8 *) CYREG_SC_MISC )
    #define CY_LIB_SC_MISC_PTR             (  (reg8 *) CYREG_SC_MISC )

#endif  /* (CYDEV_VARIABLE_VDDA == 1) */


/*******************************************************************************
*    Clock Distribution Registers
*******************************************************************************/

/* Analog Clock Mask Register */
#define CY_LIB_CLKDIST_AMASK_REG       (* (reg8 *) CYREG_CLKDIST_AMASK )
#define CY_LIB_CLKDIST_AMASK_PTR       (  (reg8 *) CYREG_CLKDIST_AMASK )

/* Digital Clock Mask Register */
#define CY_LIB_CLKDIST_DMASK_REG        (*(reg8 *) CYREG_CLKDIST_DMASK)
#define CY_LIB_CLKDIST_DMASK_PTR        ( (reg8 *) CYREG_CLKDIST_DMASK)

/* CLK_BUS Configuration Register */
#define CY_LIB_CLKDIST_BCFG2_REG        (*(reg8 *) CYREG_CLKDIST_BCFG2)
#define CY_LIB_CLKDIST_BCFG2_PTR        ( (reg8 *) CYREG_CLKDIST_BCFG2)

/* LSB Shadow Divider Value Register */
#define CY_LIB_CLKDIST_WRK_LSB_REG      (*(reg8 *) CYREG_CLKDIST_WRK0)
#define CY_LIB_CLKDIST_WRK_LSB_PTR      ( (reg8 *) CYREG_CLKDIST_WRK0)

/* MSB Shadow Divider Value Register */
#define CY_LIB_CLKDIST_WRK_MSB_REG      (*(reg8 *) CYREG_CLKDIST_WRK1)
#define CY_LIB_CLKDIST_WRK_MSB_PTR      ( (reg8 *) CYREG_CLKDIST_WRK1)

/* LOAD Register */
#define CY_LIB_CLKDIST_LD_REG           (*(reg8 *) CYREG_CLKDIST_LD)
#define CY_LIB_CLKDIST_LD_PTR           ( (reg8 *) CYREG_CLKDIST_LD)

/* CLK_BUS LSB Divider Value Register */
#define CY_LIB_CLKDIST_BCFG_LSB_REG     (*(reg8 *) CYREG_CLKDIST_BCFG0)
#define CY_LIB_CLKDIST_BCFG_LSB_PTR     ( (reg8 *) CYREG_CLKDIST_BCFG0)

/* CLK_BUS MSB Divider Value Register */
#define CY_LIB_CLKDIST_BCFG_MSB_REG     (*(reg8 *) CYREG_CLKDIST_BCFG1)
#define CY_LIB_CLKDIST_BCFG_MSB_PTR     ( (reg8 *) CYREG_CLKDIST_BCFG1)

/* Master clock (clk_sync_d) Divider Value Register */
#define CY_LIB_CLKDIST_MSTR0_REG        (*(reg8 *) CYREG_CLKDIST_MSTR0)
#define CY_LIB_CLKDIST_MSTR0_PTR        ( (reg8 *) CYREG_CLKDIST_MSTR0)

/* Master (clk_sync_d) Configuration Register/CPU Divider Value */
#define CY_LIB_CLKDIST_MSTR1_REG        (*(reg8 *) CYREG_CLKDIST_MSTR1)
#define CY_LIB_CLKDIST_MSTR1_PTR        ( (reg8 *) CYREG_CLKDIST_MSTR1)

/* Internal Main Oscillator Control Register */
#define CY_LIB_FASTCLK_IMO_CR_REG       (*(reg8 *) CYREG_FASTCLK_IMO_CR)
#define CY_LIB_FASTCLK_IMO_CR_PTR       ( (reg8 *) CYREG_FASTCLK_IMO_CR)

/* Configuration Register CR */
#define CY_LIB_CLKDIST_CR_REG           (*(reg8 *) CYREG_CLKDIST_CR)
#define CY_LIB_CLKDIST_CR_PTR           ( (reg8 *) CYREG_CLKDIST_CR)

/* Internal Low-speed Oscillator Control Register 0 */
#define CY_LIB_SLOWCLK_ILO_CR0_REG      (*(reg8 *) CYREG_SLOWCLK_ILO_CR0)
#define CY_LIB_SLOWCLK_ILO_CR0_PTR      ( (reg8 *) CYREG_SLOWCLK_ILO_CR0)


/*******************************************************************************
* Interrupt Registers
*******************************************************************************/

#if(CY_PSOC5)

    /* Interrupt Vector Table Offset */
    #define CY_INT_VECT_TABLE           ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)

    /* Interrupt Priority 0-31 */
    #define CY_INT_PRIORITY_REG         (* (reg8 *) CYREG_NVIC_PRI_0)
    #define CY_INT_PRIORITY_PTR         (  (reg8 *) CYREG_NVIC_PRI_0)

    /* Interrupt Enable Set 0-31 */
    #define CY_INT_ENABLE_REG           (* (reg32 *) CYREG_NVIC_SETENA0)
    #define CY_INT_ENABLE_PTR           (  (reg32 *) CYREG_NVIC_SETENA0)

    /* Interrupt Enable Clear 0-31 */
    #define CY_INT_CLEAR_REG            (* (reg32 *) CYREG_NVIC_CLRENA0)
    #define CY_INT_CLEAR_PTR            (  (reg32 *) CYREG_NVIC_CLRENA0)

    /* Interrupt Pending Set 0-31 */
    #define CY_INT_SET_PEND_REG         (* (reg32 *) CYREG_NVIC_SETPEND0)
    #define CY_INT_SET_PEND_PTR         (  (reg32 *) CYREG_NVIC_SETPEND0)

    /* Interrupt Pending Clear 0-31 */
    #define CY_INT_CLR_PEND_REG         (* (reg32 *) CYREG_NVIC_CLRPEND0)
    #define CY_INT_CLR_PEND_PTR         (  (reg32 *) CYREG_NVIC_CLRPEND0)

    /* Cache Control Register */
    #define CY_CACHE_CONTROL_REG        (* (reg16 *) CYREG_CACHE_CC_CTL )
    #define CY_CACHE_CONTROL_PTR        (  (reg16 *) CYREG_CACHE_CC_CTL )

    /* System tick registers */
    #define CY_SYS_SYST_CSR_REG         (*(reg32 *) CYREG_NVIC_SYSTICK_CTL)
    #define CY_SYS_SYST_CSR_PTR         ( (reg32 *) CYREG_NVIC_SYSTICK_CTL)

    #define CY_SYS_SYST_RVR_REG         (*(reg32 *) CYREG_NVIC_SYSTICK_RELOAD)
    #define CY_SYS_SYST_RVR_PTR         ( (reg32 *) CYREG_NVIC_SYSTICK_RELOAD)

    #define CY_SYS_SYST_CVR_REG         (*(reg32 *) CYREG_NVIC_SYSTICK_CURRENT)
    #define CY_SYS_SYST_CVR_PTR         ( (reg32 *) CYREG_NVIC_SYSTICK_CURRENT)

    #define CY_SYS_SYST_CALIB_REG       (*(reg32 *) CYREG_NVIC_SYSTICK_CAL)
    #define CY_SYS_SYST_CALIB_PTR       ( (reg32 *) CYREG_NVIC_SYSTICK_CAL)

#elif (CY_PSOC3)

    /* Interrupt Address Vector registers */
    #define CY_INT_VECT_TABLE           ((cyisraddress CYXDATA *) CYREG_INTC_VECT_MBASE)

    /* Interrupt Controller Priority Registers */
    #define CY_INT_PRIORITY_REG         (* (reg8 *) CYREG_INTC_PRIOR0)
    #define CY_INT_PRIORITY_PTR         (  (reg8 *) CYREG_INTC_PRIOR0)

    /* Interrupt Controller Set Enable Registers */
    #define CY_INT_ENABLE_REG           (* (reg8 *) CYREG_INTC_SET_EN0)
    #define CY_INT_ENABLE_PTR           (  (reg8 *) CYREG_INTC_SET_EN0)

    #define CY_INT_SET_EN0_REG          (* (reg8 *) CYREG_INTC_SET_EN0)
    #define CY_INT_SET_EN0_PTR          (  (reg8 *) CYREG_INTC_SET_EN0)

    #define CY_INT_SET_EN1_REG          (* (reg8 *) CYREG_INTC_SET_EN1)
    #define CY_INT_SET_EN1_PTR          (  (reg8 *) CYREG_INTC_SET_EN1)

    #define CY_INT_SET_EN2_REG          (* (reg8 *) CYREG_INTC_SET_EN2)
    #define CY_INT_SET_EN2_PTR          (  (reg8 *) CYREG_INTC_SET_EN2)

    #define CY_INT_SET_EN3_REG          (* (reg8 *) CYREG_INTC_SET_EN3)
    #define CY_INT_SET_EN3_PTR          (  (reg8 *) CYREG_INTC_SET_EN3)

    /* Interrupt Controller Clear Enable Registers */
    #define CY_INT_CLEAR_REG            (* (reg8 *) CYREG_INTC_CLR_EN0)
    #define CY_INT_CLEAR_PTR            (  (reg8 *) CYREG_INTC_CLR_EN0)

    #define CY_INT_CLR_EN0_REG          (* (reg8 *) CYREG_INTC_CLR_EN0)
    #define CY_INT_CLR_EN0_PTR          (  (reg8 *) CYREG_INTC_CLR_EN0)

    #define CY_INT_CLR_EN1_REG          (* (reg8 *) CYREG_INTC_CLR_EN1)
    #define CY_INT_CLR_EN1_PTR          (  (reg8 *) CYREG_INTC_CLR_EN1)

    #define CY_INT_CLR_EN2_REG          (* (reg8 *) CYREG_INTC_CLR_EN2)
    #define CY_INT_CLR_EN2_PTR          (  (reg8 *) CYREG_INTC_CLR_EN2)

    #define CY_INT_CLR_EN3_REG          (* (reg8 *) CYREG_INTC_CLR_EN3)
    #define CY_INT_CLR_EN3_PTR          (  (reg8 *) CYREG_INTC_CLR_EN3)


    /* Interrupt Controller Set Pend Registers */
    #define CY_INT_SET_PEND_REG         (* (reg8 *) CYREG_INTC_SET_PD0)
    #define CY_INT_SET_PEND_PTR         (  (reg8 *) CYREG_INTC_SET_PD0)

    /* Interrupt Controller Clear Pend Registers */
    #define CY_INT_CLR_PEND_REG         (* (reg8 *) CYREG_INTC_CLR_PD0)
    #define CY_INT_CLR_PEND_PTR         (  (reg8 *) CYREG_INTC_CLR_PD0)


    /* Access Interrupt Controller Registers based on interrupt number */
    #define CY_INT_SET_EN_INDX_PTR(number)    ((reg8 *) (CYREG_INTC_SET_EN0 + (((number) & CY_INT_NUMBER_MASK) >> 3u)))
    #define CY_INT_CLR_EN_INDX_PTR(number)    ((reg8 *) (CYREG_INTC_CLR_EN0 + (((number) & CY_INT_NUMBER_MASK) >> 3u)))
    #define CY_INT_CLR_PEND_INDX_PTR(number)  ((reg8 *) (CYREG_INTC_CLR_PD0 + (((number) & CY_INT_NUMBER_MASK) >> 3u)))
    #define CY_INT_SET_PEND_INDX_PTR(number)  ((reg8 *) (CYREG_INTC_SET_PD0 + (((number) & CY_INT_NUMBER_MASK) >> 3u)))

#endif  /* (CY_PSOC5) */


/*******************************************************************************
* Macro Name: CyAssert
********************************************************************************
* Summary:
*  The macro that evaluates the expression and if it is false (evaluates to 0)
*  then the processor is halted.
*
*  This macro is evaluated unless NDEBUG is defined.
*
*  If NDEBUG is defined, then no code is generated for this macro. NDEBUG is
*  defined by default for a Release build setting and not defined for a Debug
*  build setting.
*
* Parameters:
*  expr: Logical expression.  Asserts if false.
*
* Return:
*  None
*
*******************************************************************************/
#if !defined(NDEBUG)
    #define CYASSERT(x)     { \
                                if(!(x)) \
                                { \
                                    CyHalt((uint8) 0u); \
                                } \
                            }
#else
    #define CYASSERT(x)
#endif /* !defined(NDEBUG) */


/* Reset register fields of RESET_SR0 (CyResetStatus) */
#define CY_RESET_LVID               (0x01u)
#define CY_RESET_LVIA               (0x02u)
#define CY_RESET_HVIA               (0x04u)
#define CY_RESET_WD                 (0x08u)
#define CY_RESET_SW                 (0x20u)
#define CY_RESET_GPIO0              (0x40u)
#define CY_RESET_GPIO1              (0x80u)


/* Interrupt Controller Configuration and Status Register */
#if(CY_PSOC3)
    #define INTERRUPT_CSR               ((reg8 *) CYREG_INTC_CSR_EN)
    #define DISABLE_IRQ_SET             ((uint8)(0x01u << 1u))    /* INTC_CSR_EN */
    #define INTERRUPT_DISABLE_IRQ       {*INTERRUPT_CSR |= DISABLE_IRQ_SET;}
    #define INTERRUPT_ENABLE_IRQ        {*INTERRUPT_CSR = (uint8)(~DISABLE_IRQ_SET);}
#endif  /* (CY_PSOC3) */


#if defined(__ARMCC_VERSION)
    #define CyGlobalIntEnable           {__enable_irq();}
    #define CyGlobalIntDisable          {__disable_irq();}
#elif defined(__GNUC__) || defined (__ICCARM__)
    #define CyGlobalIntEnable           {__asm("CPSIE   i");}
    #define CyGlobalIntDisable          {__asm("CPSID   i");}
#elif defined(__C51__)
    #define CyGlobalIntEnable           {\
                                            EA = 1u; \
                                            INTERRUPT_ENABLE_IRQ\
                                        }

    #define CyGlobalIntDisable          {\
                                            INTERRUPT_DISABLE_IRQ; \
                                            CY_NOP; \
                                            EA = 0u;\
                                        }
#else
    #error No compiler toolchain defined
    #define CyGlobalIntEnable
    #define CyGlobalIntDisable
#endif  /* (__ARMCC_VERSION) */


#ifdef CYREG_MLOGIC_CPU_SCR_CPU_SCR
    #define CYDEV_HALT_CPU      CY_SET_REG8(CYREG_MLOGIC_CPU_SCR_CPU_SCR, 0x01u)
#else
    #define CYDEV_HALT_CPU      CY_SET_REG8(CYREG_MLOGIC_CPU_SCR, 0x01u)
#endif  /* (CYREG_MLOGIC_CPU_SCR_CPU_SCR) */


#ifdef CYREG_MLOGIC_REV_ID_REV_ID
    #define CYDEV_CHIP_REV_ACTUAL       (CY_GET_REG8(CYREG_MLOGIC_REV_ID_REV_ID))
#else
    #define CYDEV_CHIP_REV_ACTUAL       (CY_GET_REG8(CYREG_MLOGIC_REV_ID))
#endif  /* (CYREG_MLOGIC_REV_ID_REV_ID) */


/*******************************************************************************
* System API constants
*******************************************************************************/
#define CY_CACHE_CONTROL_FLUSH          (0x0004u)
#define CY_LIB_RESET_CR2_RESET          (0x01u)

#if(CY_PSOC5)
    /* System tick API constants */
    #define CY_SYS_SYST_CSR_ENABLE              ((uint32) (0x01u))
    #define CY_SYS_SYST_CSR_ENABLE_INT          ((uint32) (0x02u))
    #define CY_SYS_SYST_CSR_CLK_SOURCE_SHIFT    ((uint32) (0x02u))
    #define CY_SYS_SYST_CSR_COUNTFLAG_SHIFT     ((uint32) (16u))
    #define CY_SYS_SYST_CSR_CLK_SRC_SYSCLK      ((uint32) (1u))
    #define CY_SYS_SYST_CSR_CLK_SRC_LFCLK       ((uint32) (0u))
    #define CY_SYS_SYST_RVR_CNT_MASK            ((uint32) (0x00FFFFFFu))
    #define CY_SYS_SYST_NUM_OF_CALLBACKS        ((uint32) (5u))
#endif /* (CY_PSOC5) */



/*******************************************************************************
* Interrupt API constants
*******************************************************************************/
#if(CY_PSOC5)

    #define CY_INT_IRQ_BASE             (16u)

#elif (CY_PSOC3)

    #define CY_INT_IRQ_BASE             (0u)

#endif  /* (CY_PSOC5) */

/* Valid range of interrupt 0-31 */
#define CY_INT_NUMBER_MAX               (31u)

/* Valid range of system interrupt 0-15 */
#define CY_INT_SYS_NUMBER_MAX           (15u)

/* Valid range of system priority 0-7 */
#define CY_INT_PRIORITY_MAX             (7u)

/* Mask to get valid range of interrupt 0-31 */
#define CY_INT_NUMBER_MASK              (0x1Fu)

/* Mask to get valid range of system priority 0-7 */
#define CY_INT_PRIORITY_MASK            (0x7u)

/* Mask to get valid range of system interrupt 0-15 */
#define CY_INT_SYS_NUMBER_MASK          (0xFu)

#if(CY_PSOC5)

    /* CyIntSetSysVector()/CyIntGetSysVector() - parameter definitions */
    #define CY_INT_NMI_IRQN                  ( 2u)      /* Non Maskable Interrupt      */
    #define CY_INT_HARD_FAULT_IRQN           ( 3u)      /* Hard Fault Interrupt        */
    #define CY_INT_MEM_MANAGE_IRQN           ( 4u)      /* Memory Management Interrupt */
    #define CY_INT_BUS_FAULT_IRQN            ( 5u)      /* Bus Fault Interrupt         */
    #define CY_INT_USAGE_FAULT_IRQN          ( 6u)      /* Usage Fault Interrupt       */
    #define CY_INT_SVCALL_IRQN               (11u)      /* SV Call Interrupt           */
    #define CY_INT_DEBUG_MONITOR_IRQN        (12u)      /* Debug Monitor Interrupt     */
    #define CY_INT_PEND_SV_IRQN              (14u)      /* Pend SV Interrupt           */
    #define CY_INT_SYSTICK_IRQN              (15u)      /* System Tick Interrupt       */

#endif  /* (CY_PSOC5) */

/*******************************************************************************
* Interrupt Macros
*******************************************************************************/

#if(CY_PSOC5)

    /*******************************************************************************
    * Macro Name: CyIntEnable
    ********************************************************************************
    *
    * Summary:
    *  Enables the specified interrupt number.
    *
    * Parameters:
    *  number: Valid range [0-31].  Interrupt number
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntEnable(number)     CY_SET_REG32(CY_INT_ENABLE_PTR, ((uint32)((uint32)1u << (0x1Fu & (number)))))

    /*******************************************************************************
    * Macro Name: CyIntDisable
    ********************************************************************************
    *
    * Summary:
    *  Disables the specified interrupt number.
    *
    * Parameters:
    *  number: Valid range [0-31].  Interrupt number.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntDisable(number)     CY_SET_REG32(CY_INT_CLEAR_PTR, ((uint32)((uint32)1u << (0x1Fu & (number)))))


    /*******************************************************************************
    * Macro Name: CyIntSetPending
    ********************************************************************************
    *
    * Summary:
    *   Forces the specified interrupt number to be pending.
    *
    * Parameters:
    *   number: Valid range [0-31].  Interrupt number.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntSetPending(number)     CY_SET_REG32(CY_INT_SET_PEND_PTR, ((uint32)((uint32)1u << (0x1Fu & (number)))))


    /*******************************************************************************
    * Macro Name: CyIntClearPending
    ********************************************************************************
    *
    * Summary:
    *   Clears any pending interrupt for the specified interrupt number.
    *
    * Parameters:
    *   number: Valid range [0-31].  Interrupt number.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntClearPending(number)   CY_SET_REG32(CY_INT_CLR_PEND_PTR, ((uint32)((uint32)1u << (0x1Fu & (number)))))


#else   /* PSoC3 */


    /*******************************************************************************
    * Macro Name: CyIntEnable
    ********************************************************************************
    *
    * Summary:
    *  Enables the specified interrupt number.
    *
    * Parameters:
    *  number: Valid range [0-31].  Interrupt number
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntEnable(number)   CY_SET_REG8(CY_INT_SET_EN_INDX_PTR((number)), \
                                          ((uint8)(1u << (0x07u & (number)))))


    /*******************************************************************************
    * Macro Name: CyIntDisable
    ********************************************************************************
    *
    * Summary:
    *  Disables the specified interrupt number.
    *
    * Parameters:
    *  number: Valid range [0-31].  Interrupt number.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntDisable(number)   CY_SET_REG8(CY_INT_CLR_EN_INDX_PTR((number)), \
                                          ((uint8)(1u << (0x07u & (number)))))


    /*******************************************************************************
    * Macro Name: CyIntSetPending
    ********************************************************************************
    *
    * Summary:
    *  Forces the specified interrupt number to be pending.
    *
    * Parameters:
    *  number: Valid range [0-31].  Interrupt number.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntSetPending(number)   CY_SET_REG8(CY_INT_SET_PEND_INDX_PTR((number)), \
                                                  ((uint8)(1u << (0x07u & (number)))))


    /*******************************************************************************
    * Macro Name: CyIntClearPending
    ********************************************************************************
    * Summary:
    *  Clears any pending interrupt for the specified interrupt number.
    *
    * Parameters:
    *  number: Valid range [0-31].  Interrupt number.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    #define CyIntClearPending(number)   CY_SET_REG8(CY_INT_CLR_PEND_INDX_PTR((number)), \
                                                    ((uint8)(1u << (0x07u & (number)))))

#endif  /* (CY_PSOC5) */


/*******************************************************************************
* The following code is OBSOLETE and must not be used.
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

#define CYGlobalIntEnable       CyGlobalIntEnable
#define CYGlobalIntDisable      CyGlobalIntDisable

#define cymemset(s,c,n)         memset((s),(c),(n))
#define cymemcpy(d,s,n)         memcpy((d),(s),(n))

#define MFGCFG_X32_TR_PTR               (CY_CLK_XTAL32_TR_PTR)
#define MFGCFG_X32_TR                   (CY_CLK_XTAL32_TR_REG)
#define SLOWCLK_X32_TST_PTR             (CY_CLK_XTAL32_TST_PTR)
#define SLOWCLK_X32_TST                 (CY_CLK_XTAL32_TST_REG)
#define SLOWCLK_X32_CR_PTR              (CY_CLK_XTAL32_CR_PTR)
#define SLOWCLK_X32_CR                  (CY_CLK_XTAL32_CR_REG)
#define SLOWCLK_X32_CFG_PTR             (CY_CLK_XTAL32_CFG_PTR)
#define SLOWCLK_X32_CFG                 (CY_CLK_XTAL32_CFG_REG)

#define X32_CONTROL_ANA_STAT            (CY_CLK_XTAL32_CR_ANA_STAT)
#define X32_CONTROL_DIG_STAT            (0x10u)
#define X32_CONTROL_LPM                 (CY_CLK_XTAL32_CR_LPM)
#define X32_CONTROL_LPM_POSITION        (1u)
#define X32_CONTROL_X32EN               (CY_CLK_XTAL32_CR_EN)
#define X32_CONTROL_PDBEN           (CY_CLK_XTAL32_CR_PDBEN)
#define X32_TR_DPMODE                   (CY_CLK_XTAL32_TR_STARTUP)
#define X32_TR_CLEAR                    (CY_CLK_XTAL32_TR_POWERDOWN)
#define X32_TR_HPMODE                   (CY_CLK_XTAL32_TR_HIGH_POWER)
#define X32_TR_LPMODE                   (CY_CLK_XTAL32_TR_LOW_POWER)
#define X32_TST_SETALL                  (CY_CLK_XTAL32_TST_DEFAULT)
#define X32_CFG_LP_BITS_MASK            (CY_CLK_XTAL32_CFG_LP_MASK)
#define X32_CFG_LP_DEFAULT              (CY_CLK_XTAL32_CFG_LP_DEFAULT)
#define X32_CFG_LOWPOWERMODE            (0x80u)
#define X32_CFG_LP_LOWPOWER             (0x8u)
#define CY_X32_HIGHPOWER_MODE           (0u)
#define CY_X32_LOWPOWER_MODE            (1u)
#define CY_XTAL32K_DIG_STAT             (0x10u)
#define CY_XTAL32K_STAT_FIELDS          (0x30u)
#define CY_XTAL32K_DIG_STAT_UNSTABLE    (0u)
#define CY_XTAL32K_ANA_STAT_UNSTABLE    (0x0u)
#define CY_XTAL32K_STATUS               (0x20u)

#define FASTCLK_XMHZ_CSR_PTR            (CY_CLK_XMHZ_CSR_PTR)
#define FASTCLK_XMHZ_CSR                (CY_CLK_XMHZ_CSR_REG)
#define FASTCLK_XMHZ_CFG0_PTR           (CY_CLK_XMHZ_CFG0_PTR)
#define FASTCLK_XMHZ_CFG0               (CY_CLK_XMHZ_CFG0_REG)
#define FASTCLK_XMHZ_CFG1_PTR           (CY_CLK_XMHZ_CFG1_PTR)
#define FASTCLK_XMHZ_CFG1               (CY_CLK_XMHZ_CFG1_REG)
#define FASTCLK_XMHZ_GAINMASK           (CY_CLK_XMHZ_CFG0_XCFG_MASK)
#define FASTCLK_XMHZ_VREFMASK           (CY_CLK_XMHZ_CFG1_VREF_FB_MASK)
#define FASTCLK_XMHZ_VREF_WD_MASK       (CY_CLK_XMHZ_CFG1_VREF_WD_MASK)
#define XMHZ_CONTROL_ENABLE             (CY_CLK_XMHZ_CSR_ENABLE)
#define X32_CONTROL_XERR_MASK           (CY_CLK_XMHZ_CSR_XERR)
#define X32_CONTROL_XERR_DIS            (CY_CLK_XMHZ_CSR_XFB)
#define X32_CONTROL_XERR_POSITION       (7u)
#define X32_CONTROL_FAULT_RECOVER       (CY_CLK_XMHZ_CSR_XPROT)

#define CYWDT_CFG                       (CY_WDT_CFG_PTR)
#define CYWDT_CR                        (CY_WDT_CR_PTR)

#define CYWDT_TICKS_MASK                (CY_WDT_CFG_INTERVAL_MASK)
#define CYWDT_RESET                     (CY_WDT_CFG_CTW_RESET)
#define CYWDT_LPMODE_SHIFT              (CY_WDT_CFG_LPMODE_SHIFT)
#define CYWDT_LPMODE_MASK               (CY_WDT_CFG_LPMODE_MASK)
#define CYWDT_ENABLE_BIT                (CY_WDT_CFG_WDR_EN)

#define FASTCLK_PLL_CFG0_PTR            (CY_CLK_PLL_CFG0_PTR)
#define FASTCLK_PLL_CFG0                (CY_CLK_PLL_CFG0_REG)
#define FASTCLK_PLL_SR_PTR              (CY_CLK_PLL_SR_PTR)
#define FASTCLK_PLL_SR                  (CY_CLK_PLL_SR_REG)

#define MAX_FASTCLK_PLL_Q_VALUE         (CY_CLK_PLL_MAX_Q_VALUE)
#define MIN_FASTCLK_PLL_Q_VALUE         (CY_CLK_PLL_MIN_Q_VALUE)
#define MIN_FASTCLK_PLL_P_VALUE         (CY_CLK_PLL_MIN_P_VALUE)
#define MIN_FASTCLK_PLL_CUR_VALUE       (CY_CLK_PLL_MIN_CUR_VALUE)
#define MAX_FASTCLK_PLL_CUR_VALUE       (CY_CLK_PLL_MAX_CUR_VALUE)

#define PLL_CONTROL_ENABLE              (CY_CLK_PLL_ENABLE)
#define PLL_STATUS_LOCK                 (CY_CLK_PLL_LOCK_STATUS)
#define PLL_STATUS_ENABLED              (CY_CLK_PLL_ENABLE)
#define PLL_CURRENT_POSITION            (CY_CLK_PLL_CURRENT_POSITION)
#define PLL_VCO_GAIN_2                  (2u)

#define FASTCLK_PLL_Q_PTR              (CY_CLK_PLL_Q_PTR)
#define FASTCLK_PLL_Q                  (CY_CLK_PLL_Q_REG)
#define FASTCLK_PLL_P_PTR              (CY_CLK_PLL_P_PTR)
#define FASTCLK_PLL_P                  (CY_CLK_PLL_P_REG)
#define FASTCLK_PLL_CFG1_PTR           (CY_CLK_PLL_CFG1_REG)
#define FASTCLK_PLL_CFG1               (CY_CLK_PLL_CFG1_REG)

#define CY_VD_PRESISTENT_STATUS_REG    (CY_VD_PERSISTENT_STATUS_REG)
#define CY_VD_PRESISTENT_STATUS_PTR    (CY_VD_PERSISTENT_STATUS_PTR)


#if(CY_PSOC5)

    #define CYINT_IRQ_BASE      (CY_INT_IRQ_BASE)

    #define CYINT_VECT_TABLE    (CY_INT_VECT_TABLE)
    #define CYINT_PRIORITY      (CY_INT_PRIORITY_PTR)
    #define CYINT_ENABLE        (CY_INT_ENABLE_PTR)
    #define CYINT_CLEAR         (CY_INT_CLEAR_PTR)
    #define CYINT_SET_PEND      (CY_INT_SET_PEND_PTR)
    #define CYINT_CLR_PEND      (CY_INT_CLR_PEND_PTR)
    #define CACHE_CC_CTL        (CY_CACHE_CONTROL_PTR)

#elif (CY_PSOC3)

    #define CYINT_IRQ_BASE      (CY_INT_IRQ_BASE)

    #define CYINT_VECT_TABLE    (CY_INT_VECT_TABLE)
    #define CYINT_PRIORITY      (CY_INT_PRIORITY_PTR)
    #define CYINT_ENABLE        (CY_INT_ENABLE_PTR)
    #define CYINT_CLEAR         (CY_INT_CLEAR_PTR)
    #define CYINT_SET_PEND      (CY_INT_SET_PEND_PTR)
    #define CYINT_CLR_PEND      (CY_INT_CLR_PEND_PTR)

#endif  /* (CY_PSOC5) */



#define BUS_AMASK_CLEAR                 (0xF0u)
#define BUS_DMASK_CLEAR                 (0x00u)
#define CLKDIST_LD_LOAD_SET             (0x01u)
#define CLKDIST_WRK0_MASK_SET           (0x80u) /* Enable shadow loads */
#define MASTERCLK_DIVIDER_VALUE         (7u)
#define CLKDIST_BCFG2_SSS_SET           (0x40u) /* Sync source is same frequency */
#define MASTER_CLK_SRC_CLEAR            (0xFCu)
#define IMO_DOUBLER_ENABLE              (0x10u)
#define CLOCK_IMO_IMO                   (0x20u)
#define CLOCK_IMO2X_XTAL                (0x40u)
#define CLOCK_IMO_RANGE_CLEAR           (0xF8u)
#define CLOCK_CONTROL_DIST_MASK         (0xFCu)


#define CLKDIST_AMASK                  (*(reg8 *) CYREG_CLKDIST_AMASK)
#define CLKDIST_AMASK_PTR              ( (reg8 *) CYREG_CLKDIST_AMASK)
#define CLKDIST_DMASK_PTR              ( (reg8 *) CYREG_CLKDIST_DMASK)
#define CLKDIST_DMASK                  (*(reg8 *) CYREG_CLKDIST_DMASK)
#define CLKDIST_BCFG2_PTR              ( (reg8 *) CYREG_CLKDIST_BCFG2)
#define CLKDIST_BCFG2                  (*(reg8 *) CYREG_CLKDIST_BCFG2)
#define CLKDIST_WRK0_PTR               ( (reg8 *) CYREG_CLKDIST_WRK0)
#define CLKDIST_WRK0                   (*(reg8 *) CYREG_CLKDIST_WRK0)
#define CLKDIST_LD_PTR                 ( (reg8 *) CYREG_CLKDIST_LD)
#define CLKDIST_LD                     (*(reg8 *) CYREG_CLKDIST_LD)
#define CLKDIST_BCFG0_PTR              ( (reg8 *) CYREG_CLKDIST_BCFG0)
#define CLKDIST_BCFG0                  (*(reg8 *) CYREG_CLKDIST_BCFG0)
#define CLKDIST_MSTR0_PTR              ( (reg8 *) CYREG_CLKDIST_MSTR0)
#define CLKDIST_MSTR0                  (*(reg8 *) CYREG_CLKDIST_MSTR0)
#define FASTCLK_IMO_CR_PTR             ( (reg8 *) CYREG_FASTCLK_IMO_CR)
#define FASTCLK_IMO_CR                 (*(reg8 *) CYREG_FASTCLK_IMO_CR)
#define CLKDIST_CR_PTR                 ( (reg8 *) CYREG_CLKDIST_CR)
#define CLKDIST_CR                     (*(reg8 *) CYREG_CLKDIST_CR)


#define IMO_PM_ENABLE                   (0x10u)
#define PM_ACT_CFG0_PTR                ( (reg8 *) CYREG_PM_ACT_CFG0)
#define PM_ACT_CFG0                    (*(reg8 *) CYREG_PM_ACT_CFG0)
#define SLOWCLK_ILO_CR0_PTR            ( (reg8 *) CYREG_SLOWCLK_ILO_CR0)
#define SLOWCLK_ILO_CR0                (*(reg8 *) CYREG_SLOWCLK_ILO_CR0)
#define ILO_CONTROL_PD_MODE             (0x10u)
#define ILO_CONTROL_PD_POSITION         (4u)
#define ILO_CONTROL_1KHZ_ON             (0x02u)
#define ILO_CONTROL_100KHZ_ON           (0x04u)
#define ILO_CONTROL_33KHZ_ON            (0x20u)
#define PM_TW_CFG0_PTR                 ( (reg8 *) CYREG_PM_TW_CFG0)
#define PM_TW_CFG0                     (*(reg8 *) CYREG_PM_TW_CFG0)
#define PM_TW_CFG2_PTR                 ( (reg8 *) CYREG_PM_TW_CFG2)
#define PM_TW_CFG2                     (*(reg8 *) CYREG_PM_TW_CFG2)
#define RESET_CR2               ((reg8 *) CYREG_RESET_CR2)
#define FASTCLK_IMO_USBCLK_ON_SET       (0x40u)
#define CLOCK_IMO_3MHZ_VALUE            (0x03u)
#define CLOCK_IMO_6MHZ_VALUE            (0x01u)
#define CLOCK_IMO_12MHZ_VALUE           (0x00u)
#define CLOCK_IMO_24MHZ_VALUE           (0x02u)
#define CLOCK_IMO_48MHZ_VALUE           (0x04u)
#define CLOCK_IMO_62MHZ_VALUE           (0x05u)
#define CLOCK_IMO_74MHZ_VALUE           (0x06u)
#define CLKDIST_DIV_POSITION            (4u)
#define CLKDIST_MSTR1_DIV_CLEAR         (0x0Fu)
#define SFR_USER_CPUCLK_DIV_MASK        (0x0Fu)
#define CLOCK_USB_ENABLE                (0x02u)
#define CLOCK_IMO_OUT_X2                (0x10u)
#define CLOCK_IMO_OUT_X1                ((uint8)(~CLOCK_IMO_OUT_X2))
#define CLOCK_IMO2X_ECO                 ((uint8)(~CLOCK_IMO2X_DSI))
#define USB_CLKDIST_CONFIG_MASK         (0x03u)
#define USB_CLK_IMO2X                   (0x00u)
#define USB_CLK_IMO                     (0x01u)
#define USB_CLK_PLL                     (0x02u)
#define USB_CLK_DSI                     (0x03u)
#define USB_CLK_DIV2_ON                 (0x04u)
#define USB_CLK_STOP_FLAG               (0x00u)
#define USB_CLK_START_FLAG              (0x01u)
#define FTW_CLEAR_ALL_BITS              (0x00u)
#define FTW_CLEAR_FTW_BITS              (0xFCu)
#define FTW_ENABLE                      (0x01u)
#define PM_STBY_CFG0_PTR               ( (reg8 *) CYREG_PM_STBY_CFG0)
#define PM_STBY_CFG0                   (*(reg8 *) CYREG_PM_STBY_CFG0)
#define PM_AVAIL_CR2_PTR               ( (reg8 *) CYREG_PM_AVAIL_CR2)
#define PM_AVAIL_CR2                   (*(reg8 *) CYREG_PM_AVAIL_CR2)
#define CLKDIST_UCFG_PTR               ( (reg8 *) CYREG_CLKDIST_UCFG)
#define CLKDIST_UCFG                   (*(reg8 *) CYREG_CLKDIST_UCFG)
#define CLKDIST_MSTR1_PTR              ( (reg8 *) CYREG_CLKDIST_MSTR1)
#define CLKDIST_MSTR1                  (*(reg8 *) CYREG_CLKDIST_MSTR1)
#define SFR_USER_CPUCLK_DIV_PTR        ((void far *) CYREG_SFR_USER_CPUCLK_DIV)
#define IMO_TR1_PTR                    ( (reg8 *) CYREG_IMO_TR1)
#define IMO_TR1                        (*(reg8 *) CYREG_IMO_TR1)
#define CLOCK_CONTROL                  ( (reg8 *) CYREG_CLKDIST_CR)
#define CY_USB_CR1_PTR                 ( (reg8 *) CYREG_USB_CR1 )
#define CY_USB_CR1                     (*(reg8 *) CYREG_USB_CR1 )
#define USB_CLKDIST_CONFIG_PTR         ( (reg8 *) CYREG_CLKDIST_UCFG)
#define USB_CLKDIST_CONFIG             (*(reg8 *) CYREG_CLKDIST_UCFG)
#define CY_PM_ACT_CFG5_REG              (* (reg8 *) CYREG_PM_ACT_CFG5 )
#define CY_PM_ACT_CFG5_PTR              (  (reg8 *) CYREG_PM_ACT_CFG5 )
#define CY_PM_STBY_CFG5_REG             (* (reg8 *) CYREG_PM_STBY_CFG5 )
#define CY_PM_STBY_CFG5_PTR             (  (reg8 *) CYREG_PM_STBY_CFG5 )
#if(CY_PSOC3)
    #define FLSHID_CUST_TABLES_IMO_3MHZ_PTR         ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_3MHZ)
    #define FLSHID_CUST_TABLES_IMO_6MHZ_PTR         ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_6MHZ)
    #define FLSHID_CUST_TABLES_IMO_12MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_12MHZ)
    #define FLSHID_CUST_TABLES_IMO_24MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_24MHZ)
    #define FLSHID_CUST_TABLES_IMO_67MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_67MHZ)
    #define FLSHID_CUST_TABLES_IMO_80MHZ_PTR        ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_80MHZ)
    #define FLSHID_CUST_TABLES_IMO_USB_PTR          ((void far *) CYREG_FLSHID_CUST_TABLES_IMO_USB)
    #define FLSHID_MFG_CFG_IMO_TR1_PTR              ((void far *) (CYREG_FLSHID_MFG_CFG_IMO_TR1 + 1u))
 #else
    #define FLSHID_CUST_TABLES_IMO_3MHZ_PTR         ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_3MHZ)
    #define FLSHID_CUST_TABLES_IMO_6MHZ_PTR         ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_6MHZ)
    #define FLSHID_CUST_TABLES_IMO_12MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_12MHZ)
    #define FLSHID_CUST_TABLES_IMO_24MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_24MHZ)
    #define FLSHID_CUST_TABLES_IMO_67MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_67MHZ)
    #define FLSHID_CUST_TABLES_IMO_80MHZ_PTR        ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_80MHZ)
    #define FLSHID_CUST_TABLES_IMO_USB_PTR          ((reg8 *) CYREG_FLSHID_CUST_TABLES_IMO_USB)
    #define FLSHID_MFG_CFG_IMO_TR1_PTR              ((reg8 *) (CYREG_FLSHID_MFG_CFG_IMO_TR1 + 1u))
#endif  /* (CY_PSOC3) */


#endif  /* (CY_BOOT_CYLIB_H) */


/* [] END OF FILE */
