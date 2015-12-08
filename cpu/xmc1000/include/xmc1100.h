/****************************************************************************/
/**
Copyright (C) 2011-2015 Infineon Technologies AG. All rights reserved.
*
*
* @par
* Infineon Technologies AG (Infineon) is supplying this software for use with
* Infineon's microcontrollers.  This file can be freely distributed within
* development tools that are supporting such microcontrollers.
*
* @par
* THIS SOFTWARE IS PROVIDED AS IS. NO WARRANTIES, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
* OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
******************************************************************************/


/****************************************************************************************************//**
 * @file     XMC1100.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           XMC1100 from Infineon.
 *
 * @version  V1.2.2 (Reference Manual v1.2)
 * @date     23. January 2015
 *
 * @note     Generated with SVDConv V2.86c
 *           from CMSIS SVD File 'XMC1100_Processed_SVD.xml' Version 1.2.2 (Reference Manual v1.2),
 *******************************************************************************************************/



/** @addtogroup Infineon
  * @{
  */

/** @addtogroup XMC1100
  * @{
  */

#ifndef XMC1100_H
#define XMC1100_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
    /* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
    Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
    NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
    HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
    SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
    PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
    SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
    /* ---------------------  XMC1100 Specific Interrupt Numbers  --------------------- */
    SCU_0_IRQn          =   0,   /*!< SCU SR0 Interrupt                        */
    SCU_1_IRQn          =   1,   /*!< SCU SR1 Interrupt                        */
    SCU_2_IRQn          =   2,   /*!< SCU SR2 Interrupt                        */
    ERU0_0_IRQn         =   3,   /*!< ERU0 SR0 Interrupt                       */
    ERU0_1_IRQn         =   4,   /*!< ERU0 SR1 Interrupt                       */
    ERU0_2_IRQn         =   5,   /*!< ERU0 SR2 Interrupt                       */
    ERU0_3_IRQn         =   6,   /*!< ERU0 SR3 Interrupt                       */

    USIC0_0_IRQn        =   9,   /*!< USIC SR0 Interrupt                       */
    USIC0_1_IRQn        =  10,   /*!< USIC SR1 Interrupt                       */
    USIC0_2_IRQn        =  11,   /*!< USIC SR2 Interrupt                       */
    USIC0_3_IRQn        =  12,   /*!< USIC SR3 Interrupt                       */
    USIC0_4_IRQn        =  13,   /*!< USIC SR4 Interrupt                       */
    USIC0_5_IRQn        =  14,   /*!< USIC SR5 Interrupt                       */

    VADC0_C0_0_IRQn     =  15,   /*!< VADC SR0 Interrupt                       */
    VADC0_C0_1_IRQn     =  16,   /*!< VADC SR1 Interrupt                       */

    CCU40_0_IRQn        =  21,   /*!< CCU40 SR0 Interrupt                      */
    CCU40_1_IRQn        =  22,   /*!< CCU40 SR1 Interrupt                      */
    CCU40_2_IRQn        =  23,   /*!< CCU40 SR2 Interrupt                      */
    CCU40_3_IRQn        =  24,   /*!< CCU40 SR3 Interrupt                      */

} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0000            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm0.h"                               /*!< Cortex-M0 processor and core peripherals                              */
/* #include "system_XMC1100.h"                         /\*!< XMC1100 System                                                     */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */
/* Macro to modify desired bitfields of a register */
#define WR_REG(reg, mask, pos, val) reg = (((uint32_t)val << pos) & \
                                                         ((uint32_t)mask)) | \
                                          (reg & ((uint32_t)~((uint32_t)mask)))

/* Macro to modify desired bitfields of a register */
#define WR_REG_SIZE(reg, mask, pos, val, size) {  \
uint##size##_t VAL1 = (uint##size##_t)((uint##size##_t)val << pos); \
uint##size##_t VAL2 = (uint##size##_t) (VAL1 & (uint##size##_t)mask); \
uint##size##_t VAL3 = (uint##size##_t)~((uint##size##_t)mask); \
uint##size##_t VAL4 = (uint##size##_t) ((uint##size##_t)reg & VAL3); \
reg = (uint##size##_t) (VAL2 | VAL4);\
}

/** Macro to read bitfields from a register */
#define RD_REG(reg, mask, pos) (((uint32_t)reg & (uint32_t)mask) >> pos)

/** Macro to read bitfields from a register */
#define RD_REG_SIZE(reg, mask, pos,size) ((uint##size##_t)(((uint32_t)reg & \
                                                      (uint32_t)mask) >> pos) )

/** Macro to set a bit in register */
#define SET_BIT(reg, pos)     (reg |= ((uint32_t)1<<pos))

/** Macro to clear a bit in register */
#define CLR_BIT(reg, pos)     (reg = reg & (uint32_t)(~((uint32_t)1<<pos)) )
/*
* ==========================================================================
* ---------- Interrupt Handler Definition ----------------------------------
* ==========================================================================
*/
#define IRQ_Hdlr_0   SCU_0_IRQHandler
#define IRQ_Hdlr_1   SCU_1_IRQHandler
#define IRQ_Hdlr_2   SCU_2_IRQHandler
#define IRQ_Hdlr_3   ERU0_0_IRQHandler
#define IRQ_Hdlr_4   ERU0_1_IRQHandler
#define IRQ_Hdlr_5   ERU0_2_IRQHandler
#define IRQ_Hdlr_6   ERU0_3_IRQHandler

#define IRQ_Hdlr_9   USIC0_0_IRQHandler
#define IRQ_Hdlr_10  USIC0_1_IRQHandler
#define IRQ_Hdlr_11  USIC0_2_IRQHandler
#define IRQ_Hdlr_12  USIC0_3_IRQHandler
#define IRQ_Hdlr_13  USIC0_4_IRQHandler
#define IRQ_Hdlr_14  USIC0_5_IRQHandler
#define IRQ_Hdlr_15  VADC0_C0_0_IRQHandler
#define IRQ_Hdlr_16  VADC0_C0_1_IRQHandler
#define IRQ_Hdlr_21  CCU40_0_IRQHandler
#define IRQ_Hdlr_22  CCU40_1_IRQHandler
#define IRQ_Hdlr_23  CCU40_2_IRQHandler
#define IRQ_Hdlr_24  CCU40_3_IRQHandler

/*
* ==========================================================================
* ---------- Interrupt Handler retrieval macro -----------------------------
* ==========================================================================
*/
#define GET_IRQ_HANDLER(N) IRQ_Hdlr_##N


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language=extended
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning 586
#else
#warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                       PPB                      ================ */
/* ================================================================================ */


/**
  * @brief Cortex-M0 Private Peripheral Block (PPB)
  */

typedef struct {                                    /*!< (@ 0xE000E000) PPB Structure                                          */
    __I  uint32_t  RESERVED[4];
    __IO uint32_t
    SYST_CSR;                          /*!< (@ 0xE000E010) SysTick Control and Status Register                    */
    __IO uint32_t
    SYST_RVR;                          /*!< (@ 0xE000E014) SysTick Reload Value Register                          */
    __IO uint32_t
    SYST_CVR;                          /*!< (@ 0xE000E018) SysTick Current Value Register                         */
    __I  uint32_t
    SYST_CALIB;                        /*!< (@ 0xE000E01C) SysTick Calibration Value Register                     */
    __I  uint32_t  RESERVED1[56];
    __IO uint32_t
    NVIC_ISER;                         /*!< (@ 0xE000E100) Interrupt Set-enable Register                          */
    __I  uint32_t  RESERVED2[31];
    __IO uint32_t
    NVIC_ICER;                         /*!< (@ 0xE000E180) IInterrupt Clear-enable Register                       */
    __I  uint32_t  RESERVED3[31];
    __IO uint32_t
    NVIC_ISPR;                         /*!< (@ 0xE000E200) Interrupt Set-pending Register                         */
    __I  uint32_t  RESERVED4[31];
    __IO uint32_t
    NVIC_ICPR;                         /*!< (@ 0xE000E280) Interrupt Clear-pending Register                       */
    __I  uint32_t  RESERVED5[95];
    __IO uint32_t
    NVIC_IPR0;                         /*!< (@ 0xE000E400) Interrupt Priority Register 0                          */
    __IO uint32_t
    NVIC_IPR1;                         /*!< (@ 0xE000E404) Interrupt Priority Register 1                          */
    __IO uint32_t
    NVIC_IPR2;                         /*!< (@ 0xE000E408) Interrupt Priority Register 2                          */
    __IO uint32_t
    NVIC_IPR3;                         /*!< (@ 0xE000E40C) Interrupt Priority Register 3                          */
    __IO uint32_t
    NVIC_IPR4;                         /*!< (@ 0xE000E410) Interrupt Priority Register 4                          */
    __IO uint32_t
    NVIC_IPR5;                         /*!< (@ 0xE000E414) Interrupt Priority Register 5                          */
    __IO uint32_t
    NVIC_IPR6;                         /*!< (@ 0xE000E418) Interrupt Priority Register 6                          */
    __IO uint32_t
    NVIC_IPR7;                         /*!< (@ 0xE000E41C) Interrupt Priority Register 7                          */
    __I  uint32_t  RESERVED6[568];
    __I  uint32_t
    CPUID;                             /*!< (@ 0xE000ED00) CPUID Base Register                                    */
    __IO uint32_t
    ICSR;                              /*!< (@ 0xE000ED04) Interrupt Control and State Register                   */
    __I  uint32_t  RESERVED7;
    __IO uint32_t
    AIRCR;                             /*!< (@ 0xE000ED0C) Application Interrupt and Reset Control Register       */
    __IO uint32_t
    SCR;                               /*!< (@ 0xE000ED10) System Control Register                                */
    __I  uint32_t
    CCR;                               /*!< (@ 0xE000ED14) Configuration and Control Register                     */
    __I  uint32_t  RESERVED8;
    __IO uint32_t
    SHPR2;                             /*!< (@ 0xE000ED1C) System Handler Priority Register 2                     */
    __IO uint32_t
    SHPR3;                             /*!< (@ 0xE000ED20) System Handler Priority Register 3                     */
    __IO uint32_t
    SHCSR;                             /*!< (@ 0xE000ED24) System Handler Control and State Register              */
} PPB_Type;


/* ================================================================================ */
/* ================                   ERU [ERU0]                   ================ */
/* ================================================================================ */


/**
  * @brief Event Request Unit 0 (ERU)
  */

typedef struct {                                    /*!< (@ 0x40010600) ERU Structure                                          */
    __IO uint32_t
    EXISEL;                            /*!< (@ 0x40010600) Event Input Select                                     */
    __I  uint32_t  RESERVED[3];
    __IO uint32_t
    EXICON[4];                         /*!< (@ 0x40010610) Event Input Control                                    */
    __IO uint32_t
    EXOCON[4];                         /*!< (@ 0x40010620) Event Output Trigger Control                           */
} ERU_GLOBAL_TypeDef;


/* ================================================================================ */
/* ================                       PAU                      ================ */
/* ================================================================================ */


/**
  * @brief PAU Unit (PAU)
  */

typedef struct {                                    /*!< (@ 0x40000000) PAU Structure                                          */
    __I  uint32_t  RESERVED[16];
    __I  uint32_t
    AVAIL0;                            /*!< (@ 0x40000040) Peripheral Availability Register 0                     */
    __I  uint32_t
    AVAIL1;                            /*!< (@ 0x40000044) Peripheral Availability Register 1                     */
    __IO uint32_t
    AVAIL2;                            /*!< (@ 0x40000048) Peripheral Availability Register 2                     */
    __I  uint32_t  RESERVED1[13];
    __IO uint32_t
    PRIVDIS0;                          /*!< (@ 0x40000080) Peripheral Privilege Access Register 0                 */
    __IO uint32_t
    PRIVDIS1;                          /*!< (@ 0x40000084) Peripheral Privilege Access Register 1                 */
    __I  uint32_t  RESERVED2[222];
    __I  uint32_t
    ROMSIZE;                           /*!< (@ 0x40000400) ROM Size Register                                      */
    __I  uint32_t
    FLSIZE;                            /*!< (@ 0x40000404) Flash Size Register                                    */
    __I  uint32_t  RESERVED3[2];
    __I  uint32_t
    RAM0SIZE;                          /*!< (@ 0x40000410) RAM0 Size Register                                     */
} PAU_Type;


/* ================================================================================ */
/* ================                       NVM                      ================ */
/* ================================================================================ */


/**
  * @brief NVM Unit (NVM)
  */

typedef struct {                                    /*!< (@ 0x40050000) NVM Structure                                          */
    __I  uint16_t
    NVMSTATUS;                         /*!< (@ 0x40050000) NVM Status Register                                    */
    __I  uint16_t  RESERVED;
    __IO uint16_t
    NVMPROG;                           /*!< (@ 0x40050004) NVM Programming Control Register                       */
    __I  uint16_t  RESERVED1;
    __IO uint16_t
    NVMCONF;                           /*!< (@ 0x40050008) NVM Configuration Register                             */
} NVM_Type;


/* ================================================================================ */
/* ================                       WDT                      ================ */
/* ================================================================================ */


/**
  * @brief Watch Dog Timer (WDT)
  */

typedef struct {                                    /*!< (@ 0x40020000) WDT Structure                                          */
    __I  uint32_t
    ID;                                /*!< (@ 0x40020000) WDT Module ID Register                                 */
    __IO uint32_t
    CTR;                               /*!< (@ 0x40020004) WDT Control Register                                   */
    __O  uint32_t
    SRV;                               /*!< (@ 0x40020008) WDT Service Register                                   */
    __I  uint32_t
    TIM;                               /*!< (@ 0x4002000C) WDT Timer Register                                     */
    __IO uint32_t
    WLB;                               /*!< (@ 0x40020010) WDT Window Lower Bound Register                        */
    __IO uint32_t
    WUB;                               /*!< (@ 0x40020014) WDT Window Upper Bound Register                        */
    __I  uint32_t
    WDTSTS;                            /*!< (@ 0x40020018) WDT Status Register                                    */
    __O  uint32_t
    WDTCLR;                            /*!< (@ 0x4002001C) WDT Clear Register                                     */
} WDT_GLOBAL_TypeDef;


/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real Time Clock (RTC)
  */

typedef struct {                                    /*!< (@ 0x40010A00) RTC Structure                                          */
    __I  uint32_t
    ID;                                /*!< (@ 0x40010A00) RTC Module ID Register                                 */
    __IO uint32_t
    CTR;                               /*!< (@ 0x40010A04) RTC Control Register                                   */
    __I  uint32_t
    RAWSTAT;                           /*!< (@ 0x40010A08) RTC Raw Service Request Register                       */
    __I  uint32_t
    STSSR;                             /*!< (@ 0x40010A0C) RTC Service Request Status Register                    */
    __IO uint32_t
    MSKSR;                             /*!< (@ 0x40010A10) RTC Service Request Mask Register                      */
    __O  uint32_t
    CLRSR;                             /*!< (@ 0x40010A14) RTC Clear Service Request Register                     */
    __IO uint32_t
    ATIM0;                             /*!< (@ 0x40010A18) RTC Alarm Time Register 0                              */
    __IO uint32_t
    ATIM1;                             /*!< (@ 0x40010A1C) RTC Alarm Time Register 1                              */
    __IO uint32_t
    TIM0;                              /*!< (@ 0x40010A20) RTC Time Register 0                                    */
    __IO uint32_t
    TIM1;                              /*!< (@ 0x40010A24) RTC Time Register 1                                    */
} RTC_GLOBAL_TypeDef;


/* ================================================================================ */
/* ================                      PRNG                      ================ */
/* ================================================================================ */


/**
  * @brief PRNG Unit (PRNG)
  */

typedef struct {                                    /*!< (@ 0x48020000) PRNG Structure                                         */
    __IO uint16_t
    WORD;                              /*!< (@ 0x48020000) PRNG Word Register                                     */
    __I  uint16_t  RESERVED;
    __I  uint16_t
    CHK;                               /*!< (@ 0x48020004) PRNG Status Check Register                             */
    __I  uint16_t  RESERVED1[3];
    __IO uint16_t
    CTRL;                              /*!< (@ 0x4802000C) PRNG Control Register                                  */
} PRNG_Type;


/* ================================================================================ */
/* ================                  USIC [USIC0]                  ================ */
/* ================================================================================ */


/**
  * @brief Universal Serial Interface Controller 0 (USIC)
  */

typedef struct {                                    /*!< (@ 0x48000008) USIC Structure                                         */
    __I  uint32_t
    ID;                                /*!< (@ 0x48000008) Module Identification Register                         */
} USIC_GLOBAL_TypeDef;


/* ================================================================================ */
/* ================               USIC_CH [USIC0_CH0]              ================ */
/* ================================================================================ */


/**
  * @brief Universal Serial Interface Controller 0 (USIC_CH)
  */

typedef struct {                                    /*!< (@ 0x48000000) USIC_CH Structure                                      */
    __I  uint32_t  RESERVED;
    __I  uint32_t
    CCFG;                              /*!< (@ 0x48000004) Channel Configuration Register                         */
    __I  uint32_t  RESERVED1;
    __IO uint32_t
    KSCFG;                             /*!< (@ 0x4800000C) Kernel State Configuration Register                    */
    __IO uint32_t
    FDR;                               /*!< (@ 0x48000010) Fractional Divider Register                            */
    __IO uint32_t
    BRG;                               /*!< (@ 0x48000014) Baud Rate Generator Register                           */
    __IO uint32_t
    INPR;                              /*!< (@ 0x48000018) Interrupt Node Pointer Register                        */
    __IO uint32_t
    DX0CR;                             /*!< (@ 0x4800001C) Input Control Register 0                               */
    __IO uint32_t
    DX1CR;                             /*!< (@ 0x48000020) Input Control Register 1                               */
    __IO uint32_t
    DX2CR;                             /*!< (@ 0x48000024) Input Control Register 2                               */
    __IO uint32_t
    DX3CR;                             /*!< (@ 0x48000028) Input Control Register 3                               */
    __IO uint32_t
    DX4CR;                             /*!< (@ 0x4800002C) Input Control Register 4                               */
    __IO uint32_t
    DX5CR;                             /*!< (@ 0x48000030) Input Control Register 5                               */
    __IO uint32_t
    SCTR;                              /*!< (@ 0x48000034) Shift Control Register                                 */
    __IO uint32_t
    TCSR;                              /*!< (@ 0x48000038) Transmit Control/Status Register                       */

    union {
        __IO uint32_t
        PCR_IICMode;                     /*!< (@ 0x4800003C) Protocol Control Register [IIC Mode]                   */
        __IO uint32_t
        PCR_IISMode;                     /*!< (@ 0x4800003C) Protocol Control Register [IIS Mode]                   */
        __IO uint32_t
        PCR_SSCMode;                     /*!< (@ 0x4800003C) Protocol Control Register [SSC Mode]                   */
        __IO uint32_t
        PCR;                             /*!< (@ 0x4800003C) Protocol Control Register                              */
        __IO uint32_t
        PCR_ASCMode;                     /*!< (@ 0x4800003C) Protocol Control Register [ASC Mode]                   */
    };
    __IO uint32_t
    CCR;                               /*!< (@ 0x48000040) Channel Control Register                               */
    __IO uint32_t
    CMTR;                              /*!< (@ 0x48000044) Capture Mode Timer Register                            */

    union {
        __IO uint32_t
        PSR_IICMode;                     /*!< (@ 0x48000048) Protocol Status Register [IIC Mode]                    */
        __IO uint32_t
        PSR_IISMode;                     /*!< (@ 0x48000048) Protocol Status Register [IIS Mode]                    */
        __IO uint32_t
        PSR_SSCMode;                     /*!< (@ 0x48000048) Protocol Status Register [SSC Mode]                    */
        __IO uint32_t
        PSR;                             /*!< (@ 0x48000048) Protocol Status Register                               */
        __IO uint32_t
        PSR_ASCMode;                     /*!< (@ 0x48000048) Protocol Status Register [ASC Mode]                    */
    };
    __O  uint32_t
    PSCR;                              /*!< (@ 0x4800004C) Protocol Status Clear Register                         */
    __I  uint32_t
    RBUFSR;                            /*!< (@ 0x48000050) Receiver Buffer Status Register                        */
    __I  uint32_t
    RBUF;                              /*!< (@ 0x48000054) Receiver Buffer Register                               */
    __I  uint32_t
    RBUFD;                             /*!< (@ 0x48000058) Receiver Buffer Register for Debugger                  */
    __I  uint32_t
    RBUF0;                             /*!< (@ 0x4800005C) Receiver Buffer Register 0                             */
    __I  uint32_t
    RBUF1;                             /*!< (@ 0x48000060) Receiver Buffer Register 1                             */
    __I  uint32_t
    RBUF01SR;                          /*!< (@ 0x48000064) Receiver Buffer 01 Status Register                     */
    __O  uint32_t
    FMR;                               /*!< (@ 0x48000068) Flag Modification Register                             */
    __I  uint32_t  RESERVED2[5];
    __IO uint32_t
    TBUF[32];                          /*!< (@ 0x48000080) Transmit Buffer                                        */
    __IO uint32_t
    BYP;                               /*!< (@ 0x48000100) Bypass Data Register                                   */
    __IO uint32_t
    BYPCR;                             /*!< (@ 0x48000104) Bypass Control Register                                */
    __IO uint32_t
    TBCTR;                             /*!< (@ 0x48000108) Transmitter Buffer Control Register                    */
    __IO uint32_t
    RBCTR;                             /*!< (@ 0x4800010C) Receiver Buffer Control Register                       */
    __I  uint32_t
    TRBPTR;                            /*!< (@ 0x48000110) Transmit/Receive Buffer Pointer Register               */
    __IO uint32_t
    TRBSR;                             /*!< (@ 0x48000114) Transmit/Receive Buffer Status Register                */
    __O  uint32_t
    TRBSCR;                            /*!< (@ 0x48000118) Transmit/Receive Buffer Status Clear Register          */
    __I  uint32_t
    OUTR;                              /*!< (@ 0x4800011C) Receiver Buffer Output Register                        */
    __I  uint32_t
    OUTDR;                             /*!< (@ 0x48000120) Receiver Buffer Output Register L for Debugger         */
    __I  uint32_t  RESERVED3[23];
    __O  uint32_t
    IN[32];                            /*!< (@ 0x48000180) Transmit FIFO Buffer                                   */
} USIC_CH_TypeDef;


/* ================================================================================ */
/* ================                   SCU_GENERAL                  ================ */
/* ================================================================================ */


/**
  * @brief System Control Unit (SCU_GENERAL)
  */

typedef struct {                                    /*!< (@ 0x40010000) SCU_GENERAL Structure                                  */
    __I  uint32_t
    DBGROMID;                          /*!< (@ 0x40010000) Debug System ROM ID Register                           */
    __I  uint32_t
    IDCHIP;                            /*!< (@ 0x40010004) Chip ID Register                                       */
    __I  uint32_t
    ID;                                /*!< (@ 0x40010008) SCU Module ID Register                                 */
    __I  uint32_t  RESERVED[2];
    __IO uint32_t
    SSW0;                              /*!< (@ 0x40010014) SSW Register 0                                         */
    __I  uint32_t  RESERVED1[3];
    __IO uint32_t
    PASSWD;                            /*!< (@ 0x40010024) Password Register                                      */
    __I  uint32_t  RESERVED2[2];
    __IO uint32_t
    CCUCON;                            /*!< (@ 0x40010030) CCU Control Register                                   */
    __I  uint32_t  RESERVED3[5];
    __I  uint32_t
    MIRRSTS;                           /*!< (@ 0x40010048) Mirror Update Status Register                          */
    __I  uint32_t  RESERVED4[2];
    __IO uint32_t
    PMTSR;                             /*!< (@ 0x40010054) Parity Memory Test Select Register                     */
} SCU_GENERAL_Type;


/* ================================================================================ */
/* ================                  SCU_INTERRUPT                 ================ */
/* ================================================================================ */


/**
  * @brief System Control Unit (SCU_INTERRUPT)
  */

typedef struct {                                    /*!< (@ 0x40010038) SCU_INTERRUPT Structure                                */
    __I  uint32_t
    SRRAW;                             /*!< (@ 0x40010038) SCU Raw Service Request Status                         */
    __IO uint32_t
    SRMSK;                             /*!< (@ 0x4001003C) SCU Service Request Mask                               */
    __O  uint32_t
    SRCLR;                             /*!< (@ 0x40010040) SCU Service Request Clear                              */
    __O  uint32_t
    SRSET;                             /*!< (@ 0x40010044) SCU Service Request Set                                */
} SCU_INTERRUPT_TypeDef;


/* ================================================================================ */
/* ================                    SCU_POWER                   ================ */
/* ================================================================================ */


/**
  * @brief System Control Unit (SCU_POWER)
  */

typedef struct {                                    /*!< (@ 0x40010200) SCU_POWER Structure                                    */
    __I  uint32_t
    VDESR;                             /*!< (@ 0x40010200) Voltage Detector Status Register                       */
} SCU_POWER_Type;


/* ================================================================================ */
/* ================                     SCU_CLK                    ================ */
/* ================================================================================ */


/**
  * @brief System Control Unit (SCU_CLK)
  */

typedef struct {                                    /*!< (@ 0x40010300) SCU_CLK Structure                                      */
    __IO uint32_t
    CLKCR;                             /*!< (@ 0x40010300) Clock Control Register                                 */
    __IO uint32_t
    PWRSVCR;                           /*!< (@ 0x40010304) Power Save Control Register                            */
    __I  uint32_t
    CGATSTAT0;                         /*!< (@ 0x40010308) Peripheral 0 Clock Gating Status                       */
    __O  uint32_t
    CGATSET0;                          /*!< (@ 0x4001030C) Peripheral 0 Clock Gating Set                          */
    __O  uint32_t
    CGATCLR0;                          /*!< (@ 0x40010310) Peripheral 0 Clock Gating Clear                        */
    __IO uint32_t
    OSCCSR;                            /*!< (@ 0x40010314) Oscillator Control and Status Register                 */
} SCU_CLK_TypeDef;


/* ================================================================================ */
/* ================                    SCU_RESET                   ================ */
/* ================================================================================ */


/**
  * @brief System Control Unit (SCU_RESET)
  */

typedef struct {                                    /*!< (@ 0x40010400) SCU_RESET Structure                                    */
    __I  uint32_t
    RSTSTAT;                           /*!< (@ 0x40010400) RCU Reset Status                                       */
    __O  uint32_t
    RSTSET;                            /*!< (@ 0x40010404) RCU Reset Set Register                                 */
    __O  uint32_t
    RSTCLR;                            /*!< (@ 0x40010408) RCU Reset Clear Register                               */
    __IO uint32_t
    RSTCON;                            /*!< (@ 0x4001040C) RCU Reset Control Register                             */
} SCU_RESET_Type;


/* ================================================================================ */
/* ================                   SCU_ANALOG                   ================ */
/* ================================================================================ */


/**
  * @brief System Control Unit (SCU_ANALOG)
  */

typedef struct {                                    /*!< (@ 0x40011000) SCU_ANALOG Structure                                   */
    __I  uint32_t  RESERVED[9];
    __IO uint16_t
    ANATSECTRL;                        /*!< (@ 0x40011024) Temperature Sensor Control Register                    */
    __I  uint16_t  RESERVED1[5];
    __IO uint16_t
    ANATSEIH;                          /*!< (@ 0x40011030) Temperature Sensor High Temperature Interrupt
                                                         Register                                                              */
    __I  uint16_t  RESERVED2;
    __IO uint16_t
    ANATSEIL;                          /*!< (@ 0x40011034) Temperature Sensor Low Temperature Interrupt
                                                         Register                                                              */
    __I  uint16_t  RESERVED3[5];
    __I  uint16_t
    ANATSEMON;                         /*!< (@ 0x40011040) Temperature Sensor Counter2 Monitor Register           */
    __I  uint16_t  RESERVED4[7];
    __IO uint16_t
    ANAVDEL;                           /*!< (@ 0x40011050) Voltage Detector Control Register                      */
    __I  uint16_t  RESERVED5[13];
    __IO uint16_t
    ANAOFFSET;                         /*!< (@ 0x4001106C) DCO1 Offset Register                                   */
} SCU_ANALOG_Type;


/* ================================================================================ */
/* ================                  CCU4 [CCU40]                  ================ */
/* ================================================================================ */


/**
  * @brief Capture Compare Unit 4 - Unit 0 (CCU4)
  */

typedef struct {                                    /*!< (@ 0x48040000) CCU4 Structure                                         */
    __IO uint32_t
    GCTRL;                             /*!< (@ 0x48040000) Global Control Register                                */
    __I  uint32_t
    GSTAT;                             /*!< (@ 0x48040004) Global Status Register                                 */
    __O  uint32_t
    GIDLS;                             /*!< (@ 0x48040008) Global Idle Set                                        */
    __O  uint32_t
    GIDLC;                             /*!< (@ 0x4804000C) Global Idle Clear                                      */
    __O  uint32_t
    GCSS;                              /*!< (@ 0x48040010) Global Channel Set                                     */
    __O  uint32_t
    GCSC;                              /*!< (@ 0x48040014) Global Channel Clear                                   */
    __I  uint32_t
    GCST;                              /*!< (@ 0x48040018) Global Channel Status                                  */
    __I  uint32_t  RESERVED[25];
    __I  uint32_t
    MIDR;                              /*!< (@ 0x48040080) Module Identification                                  */
} CCU4_GLOBAL_TypeDef;


/* ================================================================================ */
/* ================              CCU4_CC4 [CCU40_CC40]             ================ */
/* ================================================================================ */


/**
  * @brief Capture Compare Unit 4 - Unit 0 (CCU4_CC4)
  */

typedef struct {                                    /*!< (@ 0x48040100) CCU4_CC4 Structure                                     */
    __IO uint32_t
    INS;                               /*!< (@ 0x48040100) Input Selector Configuration                           */
    __IO uint32_t
    CMC;                               /*!< (@ 0x48040104) Connection Matrix Control                              */
    __I  uint32_t
    TCST;                              /*!< (@ 0x48040108) Slice Timer Status                                     */
    __O  uint32_t
    TCSET;                             /*!< (@ 0x4804010C) Slice Timer Run Set                                    */
    __O  uint32_t
    TCCLR;                             /*!< (@ 0x48040110) Slice Timer Clear                                      */
    __IO uint32_t
    TC;                                /*!< (@ 0x48040114) Slice Timer Control                                    */
    __IO uint32_t
    PSL;                               /*!< (@ 0x48040118) Passive Level Config                                   */
    __I  uint32_t
    DIT;                               /*!< (@ 0x4804011C) Dither Config                                          */
    __IO uint32_t
    DITS;                              /*!< (@ 0x48040120) Dither Shadow Register                                 */
    __IO uint32_t
    PSC;                               /*!< (@ 0x48040124) Prescaler Control                                      */
    __IO uint32_t
    FPC;                               /*!< (@ 0x48040128) Floating Prescaler Control                             */
    __IO uint32_t
    FPCS;                              /*!< (@ 0x4804012C) Floating Prescaler Shadow                              */
    __I  uint32_t
    PR;                                /*!< (@ 0x48040130) Timer Period Value                                     */
    __IO uint32_t
    PRS;                               /*!< (@ 0x48040134) Timer Shadow Period Value                              */
    __I  uint32_t
    CR;                                /*!< (@ 0x48040138) Timer Compare Value                                    */
    __IO uint32_t
    CRS;                               /*!< (@ 0x4804013C) Timer Shadow Compare Value                             */
    __I  uint32_t  RESERVED[12];
    __IO uint32_t
    TIMER;                             /*!< (@ 0x48040170) Timer Value                                            */
    __I  uint32_t
    CV[4];                             /*!< (@ 0x48040174) Capture Register 0                                     */
    __I  uint32_t  RESERVED1[7];
    __I  uint32_t
    INTS;                              /*!< (@ 0x480401A0) Interrupt Status                                       */
    __IO uint32_t
    INTE;                              /*!< (@ 0x480401A4) Interrupt Enable Control                               */
    __IO uint32_t
    SRS;                               /*!< (@ 0x480401A8) Service Request Selector                               */
    __O  uint32_t
    SWS;                               /*!< (@ 0x480401AC) Interrupt Status Set                                   */
    __O  uint32_t
    SWR;                               /*!< (@ 0x480401B0) Interrupt Status Clear                                 */
    __I  uint32_t  RESERVED2;
    __I  uint32_t
    ECRD0;                             /*!< (@ 0x480401B8) Extended Read Back 0                                   */
    __I  uint32_t
    ECRD1;                             /*!< (@ 0x480401BC) Extended Read Back 1                                   */
} CCU4_CC4_TypeDef;


/* ================================================================================ */
/* ================                   VADC [VADC]                  ================ */
/* ================================================================================ */


/**
  * @brief Analog to Digital Converter (VADC)
  */

typedef struct {                                    /*!< (@ 0x48030000) VADC Structure                                         */
    __IO uint32_t
    CLC;                               /*!< (@ 0x48030000) Clock Control Register                                 */
    __I  uint32_t  RESERVED;
    __I  uint32_t
    ID;                                /*!< (@ 0x48030008) Module Identification Register                         */
    __I  uint32_t  RESERVED1[7];
    __IO uint32_t
    OCS;                               /*!< (@ 0x48030028) OCDS Control and Status Register                       */
    __I  uint32_t  RESERVED2[21];
    __IO uint32_t
    GLOBCFG;                           /*!< (@ 0x48030080) Global Configuration Register                          */
    __I  uint32_t  RESERVED3[7];
    __IO uint32_t
    GLOBICLASS[2];                     /*!< (@ 0x480300A0) Input Class Register, Global                           */
    __I  uint32_t  RESERVED4[14];
    __IO uint32_t
    GLOBEFLAG;                         /*!< (@ 0x480300E0) Global Event Flag Register                             */
    __I  uint32_t  RESERVED5[23];
    __IO uint32_t
    GLOBEVNP;                          /*!< (@ 0x48030140) Global Event Node Pointer Register                     */
    __I  uint32_t  RESERVED6[15];
    __IO uint32_t
    BRSSEL[2];                         /*!< (@ 0x48030180) Background Request Source Channel Select Register      */
    __I  uint32_t  RESERVED7[14];
    __IO uint32_t
    BRSPND[2];                         /*!< (@ 0x480301C0) Background Request Source Pending Register             */
    __I  uint32_t  RESERVED8[14];
    __IO uint32_t
    BRSCTRL;                           /*!< (@ 0x48030200) Background Request Source Control Register             */
    __IO uint32_t
    BRSMR;                             /*!< (@ 0x48030204) Background Request Source Mode Register                */
    __I  uint32_t  RESERVED9[30];
    __IO uint32_t
    GLOBRCR;                           /*!< (@ 0x48030280) Global Result Control Register                         */
    __I  uint32_t  RESERVED10[31];
    __IO uint32_t
    GLOBRES;                           /*!< (@ 0x48030300) Global Result Register                                 */
    __I  uint32_t  RESERVED11[31];
    __IO uint32_t
    GLOBRESD;                          /*!< (@ 0x48030380) Global Result Register, Debug                          */
} VADC_GLOBAL_TypeDef;


/* ================================================================================ */
/* ================                   SHS [SHS0]                   ================ */
/* ================================================================================ */


/**
  * @brief Sample and Hold ADC Sequencer (SHS)
  */

typedef struct {                                    /*!< (@ 0x48034000) SHS Structure                                          */
    __I  uint32_t  RESERVED[2];
    __I  uint32_t
    ID;                                /*!< (@ 0x48034008) Module Identification Register                         */
    __I  uint32_t  RESERVED1[13];
    __IO uint32_t
    SHSCFG;                            /*!< (@ 0x48034040) SHS Configuration Register                             */
    __IO uint32_t
    STEPCFG;                           /*!< (@ 0x48034044) Stepper Configuration Register                         */
    __I  uint32_t  RESERVED2[2];
    __IO uint32_t
    LOOP;                              /*!< (@ 0x48034050) Loop Control Register                                  */
    __I  uint32_t  RESERVED3[11];
    __IO uint32_t
    TIMCFG0;                           /*!< (@ 0x48034080) Timing Configuration Register 0                        */
    __IO uint32_t
    TIMCFG1;                           /*!< (@ 0x48034084) Timing Configuration Register 1                        */
    __I  uint32_t  RESERVED4[13];
    __IO uint32_t
    CALCTR;                            /*!< (@ 0x480340BC) Calibration Control Register                           */
    __IO uint32_t
    CALGC0;                            /*!< (@ 0x480340C0) Gain Calibration Control Register 0                    */
    __IO uint32_t
    CALGC1;                            /*!< (@ 0x480340C4) Gain Calibration Control Register 1                    */
    __I  uint32_t  RESERVED5[46];
    __IO uint32_t
    GNCTR00;                           /*!< (@ 0x48034180) Gain Control Register 00                               */
    __I  uint32_t  RESERVED6[3];
    __IO uint32_t
    GNCTR10;                           /*!< (@ 0x48034190) Gain Control Register 10                               */
} SHS_Type;


/* ================================================================================ */
/* ================                      PORT0                     ================ */
/* ================================================================================ */


/**
  * @brief Port 0 (PORT0)
  */

typedef struct {                                    /*!< (@ 0x40040000) PORT0 Structure                                        */
    __IO uint32_t
    OUT;                               /*!< (@ 0x40040000) Port 0 Output Register                                 */
    __O  uint32_t
    OMR;                               /*!< (@ 0x40040004) Port 0 Output Modification Register                    */
    __I  uint32_t  RESERVED[2];
    __IO uint32_t
    IOCR0;                             /*!< (@ 0x40040010) Port 0 Input/Output Control Register 0                 */
    __IO uint32_t
    IOCR4;                             /*!< (@ 0x40040014) Port 0 Input/Output Control Register 4                 */
    __IO uint32_t
    IOCR8;                             /*!< (@ 0x40040018) Port 0 Input/Output Control Register 8                 */
    __IO uint32_t
    IOCR12;                            /*!< (@ 0x4004001C) Port 0 Input/Output Control Register 12                */
    __I  uint32_t  RESERVED1;
    __I  uint32_t
    IN;                                /*!< (@ 0x40040024) Port 0 Input Register                                  */
    __I  uint32_t  RESERVED2[6];
    __IO uint32_t
    PHCR0;                             /*!< (@ 0x40040040) Port 0 Pad Hysteresis Control Register 0               */
    __IO uint32_t
    PHCR1;                             /*!< (@ 0x40040044) Port 0 Pad Hysteresis Control Register 1               */
    __I  uint32_t  RESERVED3[6];
    __I  uint32_t
    PDISC;                             /*!< (@ 0x40040060) Port 0 Pin Function Decision Control Register          */
    __I  uint32_t  RESERVED4[3];
    __IO uint32_t
    PPS;                               /*!< (@ 0x40040070) Port 0 Pin Power Save Register                         */
    __IO uint32_t
    HWSEL;                             /*!< (@ 0x40040074) Port 0 Pin Hardware Select Register                    */
} PORT0_Type;


/* ================================================================================ */
/* ================                      PORT1                     ================ */
/* ================================================================================ */


/**
  * @brief Port 1 (PORT1)
  */

typedef struct {                                    /*!< (@ 0x40040100) PORT1 Structure                                        */
    __IO uint32_t
    OUT;                               /*!< (@ 0x40040100) Port 1 Output Register                                 */
    __O  uint32_t
    OMR;                               /*!< (@ 0x40040104) Port 1 Output Modification Register                    */
    __I  uint32_t  RESERVED[2];
    __IO uint32_t
    IOCR0;                             /*!< (@ 0x40040110) Port 1 Input/Output Control Register 0                 */
    __IO uint32_t
    IOCR4;                             /*!< (@ 0x40040114) Port 1 Input/Output Control Register 4                 */
    __I  uint32_t  RESERVED1[3];
    __I  uint32_t
    IN;                                /*!< (@ 0x40040124) Port 1 Input Register                                  */
    __I  uint32_t  RESERVED2[6];
    __IO uint32_t
    PHCR0;                             /*!< (@ 0x40040140) Port 1 Pad Hysteresis Control Register 0               */
    __I  uint32_t  RESERVED3[7];
    __I  uint32_t
    PDISC;                             /*!< (@ 0x40040160) Port 1 Pin Function Decision Control Register          */
    __I  uint32_t  RESERVED4[3];
    __IO uint32_t
    PPS;                               /*!< (@ 0x40040170) Port 1 Pin Power Save Register                         */
    __IO uint32_t
    HWSEL;                             /*!< (@ 0x40040174) Port 1 Pin Hardware Select Register                    */
} PORT1_Type;


/* ================================================================================ */
/* ================                      PORT2                     ================ */
/* ================================================================================ */


/**
  * @brief Port 2 (PORT2)
  */

typedef struct {                                    /*!< (@ 0x40040200) PORT2 Structure                                        */
    __IO uint32_t
    OUT;                               /*!< (@ 0x40040200) Port 2 Output Register                                 */
    __O  uint32_t
    OMR;                               /*!< (@ 0x40040204) Port 2 Output Modification Register                    */
    __I  uint32_t  RESERVED[2];
    __IO uint32_t
    IOCR0;                             /*!< (@ 0x40040210) Port 2 Input/Output Control Register 0                 */
    __IO uint32_t
    IOCR4;                             /*!< (@ 0x40040214) Port 2 Input/Output Control Register 4                 */
    __IO uint32_t
    IOCR8;                             /*!< (@ 0x40040218) Port 2 Input/Output Control Register 8                 */
    __I  uint32_t  RESERVED1[2];
    __I  uint32_t
    IN;                                /*!< (@ 0x40040224) Port 2 Input Register                                  */
    __I  uint32_t  RESERVED2[6];
    __IO uint32_t
    PHCR0;                             /*!< (@ 0x40040240) Port 2 Pad Hysteresis Control Register 0               */
    __IO uint32_t
    PHCR1;                             /*!< (@ 0x40040244) Port 2 Pad Hysteresis Control Register 1               */
    __I  uint32_t  RESERVED3[6];
    __IO uint32_t
    PDISC;                             /*!< (@ 0x40040260) Port 2 Pin Function Decision Control Register          */
    __I  uint32_t  RESERVED4[3];
    __IO uint32_t
    PPS;                               /*!< (@ 0x40040270) Port 2 Pin Power Save Register                         */
    __IO uint32_t
    HWSEL;                             /*!< (@ 0x40040274) Port 2 Pin Hardware Select Register                    */
} PORT2_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
/* leave anonymous unions enabled */
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning restore
#else
#warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================          struct 'PPB' Position & Mask          ================ */
/* ================================================================================ */


/* --------------------------------  PPB_SYST_CSR  -------------------------------- */
#define PPB_SYST_CSR_ENABLE_Pos               (0UL)                     /*!< PPB SYST_CSR: ENABLE (Bit 0)                                */
#define PPB_SYST_CSR_ENABLE_Msk               (0x1UL)                   /*!< PPB SYST_CSR: ENABLE (Bitfield-Mask: 0x01)                  */
#define PPB_SYST_CSR_TICKINT_Pos              (1UL)                     /*!< PPB SYST_CSR: TICKINT (Bit 1)                               */
#define PPB_SYST_CSR_TICKINT_Msk              (0x2UL)                   /*!< PPB SYST_CSR: TICKINT (Bitfield-Mask: 0x01)                 */
#define PPB_SYST_CSR_CLKSOURCE_Pos            (2UL)                     /*!< PPB SYST_CSR: CLKSOURCE (Bit 2)                             */
#define PPB_SYST_CSR_CLKSOURCE_Msk            (0x4UL)                   /*!< PPB SYST_CSR: CLKSOURCE (Bitfield-Mask: 0x01)               */
#define PPB_SYST_CSR_COUNTFLAG_Pos            (16UL)                    /*!< PPB SYST_CSR: COUNTFLAG (Bit 16)                            */
#define PPB_SYST_CSR_COUNTFLAG_Msk            (0x10000UL)               /*!< PPB SYST_CSR: COUNTFLAG (Bitfield-Mask: 0x01)               */

/* --------------------------------  PPB_SYST_RVR  -------------------------------- */
#define PPB_SYST_RVR_RELOAD_Pos               (0UL)                     /*!< PPB SYST_RVR: RELOAD (Bit 0)                                */
#define PPB_SYST_RVR_RELOAD_Msk               (0xffffffUL)              /*!< PPB SYST_RVR: RELOAD (Bitfield-Mask: 0xffffff)              */

/* --------------------------------  PPB_SYST_CVR  -------------------------------- */
#define PPB_SYST_CVR_CURRENT_Pos              (0UL)                     /*!< PPB SYST_CVR: CURRENT (Bit 0)                               */
#define PPB_SYST_CVR_CURRENT_Msk              (0xffffffUL)              /*!< PPB SYST_CVR: CURRENT (Bitfield-Mask: 0xffffff)             */

/* -------------------------------  PPB_SYST_CALIB  ------------------------------- */
#define PPB_SYST_CALIB_TENMS_Pos              (0UL)                     /*!< PPB SYST_CALIB: TENMS (Bit 0)                               */
#define PPB_SYST_CALIB_TENMS_Msk              (0xffffffUL)              /*!< PPB SYST_CALIB: TENMS (Bitfield-Mask: 0xffffff)             */
#define PPB_SYST_CALIB_SKEW_Pos               (30UL)                    /*!< PPB SYST_CALIB: SKEW (Bit 30)                               */
#define PPB_SYST_CALIB_SKEW_Msk               (0x40000000UL)            /*!< PPB SYST_CALIB: SKEW (Bitfield-Mask: 0x01)                  */
#define PPB_SYST_CALIB_NOREF_Pos              (31UL)                    /*!< PPB SYST_CALIB: NOREF (Bit 31)                              */
#define PPB_SYST_CALIB_NOREF_Msk              (0x80000000UL)            /*!< PPB SYST_CALIB: NOREF (Bitfield-Mask: 0x01)                 */

/* --------------------------------  PPB_NVIC_ISER  ------------------------------- */
#define PPB_NVIC_ISER_SETENA_Pos              (0UL)                     /*!< PPB NVIC_ISER: SETENA (Bit 0)                               */
#define PPB_NVIC_ISER_SETENA_Msk              (0xffffffffUL)            /*!< PPB NVIC_ISER: SETENA (Bitfield-Mask: 0xffffffff)           */

/* --------------------------------  PPB_NVIC_ICER  ------------------------------- */
#define PPB_NVIC_ICER_CLRENA_Pos              (0UL)                     /*!< PPB NVIC_ICER: CLRENA (Bit 0)                               */
#define PPB_NVIC_ICER_CLRENA_Msk              (0xffffffffUL)            /*!< PPB NVIC_ICER: CLRENA (Bitfield-Mask: 0xffffffff)           */

/* --------------------------------  PPB_NVIC_ISPR  ------------------------------- */
#define PPB_NVIC_ISPR_SETPEND_Pos             (0UL)                     /*!< PPB NVIC_ISPR: SETPEND (Bit 0)                              */
#define PPB_NVIC_ISPR_SETPEND_Msk             (0xffffffffUL)            /*!< PPB NVIC_ISPR: SETPEND (Bitfield-Mask: 0xffffffff)          */

/* --------------------------------  PPB_NVIC_ICPR  ------------------------------- */
#define PPB_NVIC_ICPR_CLRPEND_Pos             (0UL)                     /*!< PPB NVIC_ICPR: CLRPEND (Bit 0)                              */
#define PPB_NVIC_ICPR_CLRPEND_Msk             (0xffffffffUL)            /*!< PPB NVIC_ICPR: CLRPEND (Bitfield-Mask: 0xffffffff)          */

/* --------------------------------  PPB_NVIC_IPR0  ------------------------------- */
#define PPB_NVIC_IPR0_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR0: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR0_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR0: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR0_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR0: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR0_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR0: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR0_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR0: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR0_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR0: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR0_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR0: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR0_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR0: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR1  ------------------------------- */
#define PPB_NVIC_IPR1_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR1: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR1_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR1: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR1_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR1: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR1_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR1: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR1_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR1: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR1_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR1: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR1_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR1: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR1_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR1: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR2  ------------------------------- */
#define PPB_NVIC_IPR2_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR2: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR2_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR2: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR2_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR2: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR2_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR2: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR2_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR2: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR2_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR2: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR2_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR2: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR2_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR2: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR3  ------------------------------- */
#define PPB_NVIC_IPR3_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR3: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR3_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR3: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR3_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR3: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR3_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR3: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR3_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR3: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR3_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR3: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR3_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR3: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR3_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR3: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR4  ------------------------------- */
#define PPB_NVIC_IPR4_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR4: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR4_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR4: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR4_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR4: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR4_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR4: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR4_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR4: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR4_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR4: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR4_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR4: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR4_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR4: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR5  ------------------------------- */
#define PPB_NVIC_IPR5_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR5: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR5_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR5: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR5_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR5: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR5_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR5: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR5_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR5: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR5_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR5: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR5_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR5: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR5_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR5: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR6  ------------------------------- */
#define PPB_NVIC_IPR6_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR6: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR6_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR6: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR6_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR6: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR6_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR6: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR6_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR6: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR6_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR6: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR6_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR6: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR6_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR6: PRI_3 (Bitfield-Mask: 0xff)                  */

/* --------------------------------  PPB_NVIC_IPR7  ------------------------------- */
#define PPB_NVIC_IPR7_PRI_0_Pos               (0UL)                     /*!< PPB NVIC_IPR7: PRI_0 (Bit 0)                                */
#define PPB_NVIC_IPR7_PRI_0_Msk               (0xffUL)                  /*!< PPB NVIC_IPR7: PRI_0 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR7_PRI_1_Pos               (8UL)                     /*!< PPB NVIC_IPR7: PRI_1 (Bit 8)                                */
#define PPB_NVIC_IPR7_PRI_1_Msk               (0xff00UL)                /*!< PPB NVIC_IPR7: PRI_1 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR7_PRI_2_Pos               (16UL)                    /*!< PPB NVIC_IPR7: PRI_2 (Bit 16)                               */
#define PPB_NVIC_IPR7_PRI_2_Msk               (0xff0000UL)              /*!< PPB NVIC_IPR7: PRI_2 (Bitfield-Mask: 0xff)                  */
#define PPB_NVIC_IPR7_PRI_3_Pos               (24UL)                    /*!< PPB NVIC_IPR7: PRI_3 (Bit 24)                               */
#define PPB_NVIC_IPR7_PRI_3_Msk               (0xff000000UL)            /*!< PPB NVIC_IPR7: PRI_3 (Bitfield-Mask: 0xff)                  */

/* ----------------------------------  PPB_CPUID  --------------------------------- */
#define PPB_CPUID_Revision_Pos                (0UL)                     /*!< PPB CPUID: Revision (Bit 0)                                 */
#define PPB_CPUID_Revision_Msk                (0xfUL)                   /*!< PPB CPUID: Revision (Bitfield-Mask: 0x0f)                   */
#define PPB_CPUID_PartNo_Pos                  (4UL)                     /*!< PPB CPUID: PartNo (Bit 4)                                   */
#define PPB_CPUID_PartNo_Msk                  (0xfff0UL)                /*!< PPB CPUID: PartNo (Bitfield-Mask: 0xfff)                    */
#define PPB_CPUID_Architecture_Pos            (16UL)                    /*!< PPB CPUID: Architecture (Bit 16)                            */
#define PPB_CPUID_Architecture_Msk            (0xf0000UL)               /*!< PPB CPUID: Architecture (Bitfield-Mask: 0x0f)               */
#define PPB_CPUID_Variant_Pos                 (20UL)                    /*!< PPB CPUID: Variant (Bit 20)                                 */
#define PPB_CPUID_Variant_Msk                 (0xf00000UL)              /*!< PPB CPUID: Variant (Bitfield-Mask: 0x0f)                    */
#define PPB_CPUID_Implementer_Pos             (24UL)                    /*!< PPB CPUID: Implementer (Bit 24)                             */
#define PPB_CPUID_Implementer_Msk             (0xff000000UL)            /*!< PPB CPUID: Implementer (Bitfield-Mask: 0xff)                */

/* ----------------------------------  PPB_ICSR  ---------------------------------- */
#define PPB_ICSR_VECTACTIVE_Pos               (0UL)                     /*!< PPB ICSR: VECTACTIVE (Bit 0)                                */
#define PPB_ICSR_VECTACTIVE_Msk               (0x3fUL)                  /*!< PPB ICSR: VECTACTIVE (Bitfield-Mask: 0x3f)                  */
#define PPB_ICSR_VECTPENDING_Pos              (12UL)                    /*!< PPB ICSR: VECTPENDING (Bit 12)                              */
#define PPB_ICSR_VECTPENDING_Msk              (0x3f000UL)               /*!< PPB ICSR: VECTPENDING (Bitfield-Mask: 0x3f)                 */
#define PPB_ICSR_ISRPENDING_Pos               (22UL)                    /*!< PPB ICSR: ISRPENDING (Bit 22)                               */
#define PPB_ICSR_ISRPENDING_Msk               (0x400000UL)              /*!< PPB ICSR: ISRPENDING (Bitfield-Mask: 0x01)                  */
#define PPB_ICSR_PENDSTCLR_Pos                (25UL)                    /*!< PPB ICSR: PENDSTCLR (Bit 25)                                */
#define PPB_ICSR_PENDSTCLR_Msk                (0x2000000UL)             /*!< PPB ICSR: PENDSTCLR (Bitfield-Mask: 0x01)                   */
#define PPB_ICSR_PENDSTSET_Pos                (26UL)                    /*!< PPB ICSR: PENDSTSET (Bit 26)                                */
#define PPB_ICSR_PENDSTSET_Msk                (0x4000000UL)             /*!< PPB ICSR: PENDSTSET (Bitfield-Mask: 0x01)                   */
#define PPB_ICSR_PENDSVCLR_Pos                (27UL)                    /*!< PPB ICSR: PENDSVCLR (Bit 27)                                */
#define PPB_ICSR_PENDSVCLR_Msk                (0x8000000UL)             /*!< PPB ICSR: PENDSVCLR (Bitfield-Mask: 0x01)                   */
#define PPB_ICSR_PENDSVSET_Pos                (28UL)                    /*!< PPB ICSR: PENDSVSET (Bit 28)                                */
#define PPB_ICSR_PENDSVSET_Msk                (0x10000000UL)            /*!< PPB ICSR: PENDSVSET (Bitfield-Mask: 0x01)                   */

/* ----------------------------------  PPB_AIRCR  --------------------------------- */
#define PPB_AIRCR_SYSRESETREQ_Pos             (2UL)                     /*!< PPB AIRCR: SYSRESETREQ (Bit 2)                              */
#define PPB_AIRCR_SYSRESETREQ_Msk             (0x4UL)                   /*!< PPB AIRCR: SYSRESETREQ (Bitfield-Mask: 0x01)                */
#define PPB_AIRCR_ENDIANNESS_Pos              (15UL)                    /*!< PPB AIRCR: ENDIANNESS (Bit 15)                              */
#define PPB_AIRCR_ENDIANNESS_Msk              (0x8000UL)                /*!< PPB AIRCR: ENDIANNESS (Bitfield-Mask: 0x01)                 */
#define PPB_AIRCR_VECTKEY_Pos                 (16UL)                    /*!< PPB AIRCR: VECTKEY (Bit 16)                                 */
#define PPB_AIRCR_VECTKEY_Msk                 (0xffff0000UL)            /*!< PPB AIRCR: VECTKEY (Bitfield-Mask: 0xffff)                  */

/* -----------------------------------  PPB_SCR  ---------------------------------- */
#define PPB_SCR_SLEEPONEXIT_Pos               (1UL)                     /*!< PPB SCR: SLEEPONEXIT (Bit 1)                                */
#define PPB_SCR_SLEEPONEXIT_Msk               (0x2UL)                   /*!< PPB SCR: SLEEPONEXIT (Bitfield-Mask: 0x01)                  */
#define PPB_SCR_SLEEPDEEP_Pos                 (2UL)                     /*!< PPB SCR: SLEEPDEEP (Bit 2)                                  */
#define PPB_SCR_SLEEPDEEP_Msk                 (0x4UL)                   /*!< PPB SCR: SLEEPDEEP (Bitfield-Mask: 0x01)                    */
#define PPB_SCR_SEVONPEND_Pos                 (4UL)                     /*!< PPB SCR: SEVONPEND (Bit 4)                                  */
#define PPB_SCR_SEVONPEND_Msk                 (0x10UL)                  /*!< PPB SCR: SEVONPEND (Bitfield-Mask: 0x01)                    */

/* -----------------------------------  PPB_CCR  ---------------------------------- */
#define PPB_CCR_UNALIGN_TRP_Pos               (3UL)                     /*!< PPB CCR: UNALIGN_TRP (Bit 3)                                */
#define PPB_CCR_UNALIGN_TRP_Msk               (0x8UL)                   /*!< PPB CCR: UNALIGN_TRP (Bitfield-Mask: 0x01)                  */
#define PPB_CCR_STKALIGN_Pos                  (9UL)                     /*!< PPB CCR: STKALIGN (Bit 9)                                   */
#define PPB_CCR_STKALIGN_Msk                  (0x200UL)                 /*!< PPB CCR: STKALIGN (Bitfield-Mask: 0x01)                     */

/* ----------------------------------  PPB_SHPR2  --------------------------------- */
#define PPB_SHPR2_PRI_11_Pos                  (24UL)                    /*!< PPB SHPR2: PRI_11 (Bit 24)                                  */
#define PPB_SHPR2_PRI_11_Msk                  (0xff000000UL)            /*!< PPB SHPR2: PRI_11 (Bitfield-Mask: 0xff)                     */

/* ----------------------------------  PPB_SHPR3  --------------------------------- */
#define PPB_SHPR3_PRI_14_Pos                  (16UL)                    /*!< PPB SHPR3: PRI_14 (Bit 16)                                  */
#define PPB_SHPR3_PRI_14_Msk                  (0xff0000UL)              /*!< PPB SHPR3: PRI_14 (Bitfield-Mask: 0xff)                     */
#define PPB_SHPR3_PRI_15_Pos                  (24UL)                    /*!< PPB SHPR3: PRI_15 (Bit 24)                                  */
#define PPB_SHPR3_PRI_15_Msk                  (0xff000000UL)            /*!< PPB SHPR3: PRI_15 (Bitfield-Mask: 0xff)                     */

/* ----------------------------------  PPB_SHCSR  --------------------------------- */
#define PPB_SHCSR_SVCALLPENDED_Pos            (15UL)                    /*!< PPB SHCSR: SVCALLPENDED (Bit 15)                            */
#define PPB_SHCSR_SVCALLPENDED_Msk            (0x8000UL)                /*!< PPB SHCSR: SVCALLPENDED (Bitfield-Mask: 0x01)               */


/* ================================================================================ */
/* ================           Group 'ERU' Position & Mask          ================ */
/* ================================================================================ */


/* ---------------------------------  ERU_EXISEL  --------------------------------- */
#define ERU_EXISEL_EXS0A_Pos                  (0UL)                     /*!< ERU EXISEL: EXS0A (Bit 0)                                   */
#define ERU_EXISEL_EXS0A_Msk                  (0x3UL)                   /*!< ERU EXISEL: EXS0A (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS0B_Pos                  (2UL)                     /*!< ERU EXISEL: EXS0B (Bit 2)                                   */
#define ERU_EXISEL_EXS0B_Msk                  (0xcUL)                   /*!< ERU EXISEL: EXS0B (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS1A_Pos                  (4UL)                     /*!< ERU EXISEL: EXS1A (Bit 4)                                   */
#define ERU_EXISEL_EXS1A_Msk                  (0x30UL)                  /*!< ERU EXISEL: EXS1A (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS1B_Pos                  (6UL)                     /*!< ERU EXISEL: EXS1B (Bit 6)                                   */
#define ERU_EXISEL_EXS1B_Msk                  (0xc0UL)                  /*!< ERU EXISEL: EXS1B (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS2A_Pos                  (8UL)                     /*!< ERU EXISEL: EXS2A (Bit 8)                                   */
#define ERU_EXISEL_EXS2A_Msk                  (0x300UL)                 /*!< ERU EXISEL: EXS2A (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS2B_Pos                  (10UL)                    /*!< ERU EXISEL: EXS2B (Bit 10)                                  */
#define ERU_EXISEL_EXS2B_Msk                  (0xc00UL)                 /*!< ERU EXISEL: EXS2B (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS3A_Pos                  (12UL)                    /*!< ERU EXISEL: EXS3A (Bit 12)                                  */
#define ERU_EXISEL_EXS3A_Msk                  (0x3000UL)                /*!< ERU EXISEL: EXS3A (Bitfield-Mask: 0x03)                     */
#define ERU_EXISEL_EXS3B_Pos                  (14UL)                    /*!< ERU EXISEL: EXS3B (Bit 14)                                  */
#define ERU_EXISEL_EXS3B_Msk                  (0xc000UL)                /*!< ERU EXISEL: EXS3B (Bitfield-Mask: 0x03)                     */

/* ---------------------------------  ERU_EXICON  --------------------------------- */
#define ERU_EXICON_PE_Pos                     (0UL)                     /*!< ERU EXICON: PE (Bit 0)                                      */
#define ERU_EXICON_PE_Msk                     (0x1UL)                   /*!< ERU EXICON: PE (Bitfield-Mask: 0x01)                        */
#define ERU_EXICON_LD_Pos                     (1UL)                     /*!< ERU EXICON: LD (Bit 1)                                      */
#define ERU_EXICON_LD_Msk                     (0x2UL)                   /*!< ERU EXICON: LD (Bitfield-Mask: 0x01)                        */
#define ERU_EXICON_RE_Pos                     (2UL)                     /*!< ERU EXICON: RE (Bit 2)                                      */
#define ERU_EXICON_RE_Msk                     (0x4UL)                   /*!< ERU EXICON: RE (Bitfield-Mask: 0x01)                        */
#define ERU_EXICON_FE_Pos                     (3UL)                     /*!< ERU EXICON: FE (Bit 3)                                      */
#define ERU_EXICON_FE_Msk                     (0x8UL)                   /*!< ERU EXICON: FE (Bitfield-Mask: 0x01)                        */
#define ERU_EXICON_OCS_Pos                    (4UL)                     /*!< ERU EXICON: OCS (Bit 4)                                     */
#define ERU_EXICON_OCS_Msk                    (0x70UL)                  /*!< ERU EXICON: OCS (Bitfield-Mask: 0x07)                       */
#define ERU_EXICON_FL_Pos                     (7UL)                     /*!< ERU EXICON: FL (Bit 7)                                      */
#define ERU_EXICON_FL_Msk                     (0x80UL)                  /*!< ERU EXICON: FL (Bitfield-Mask: 0x01)                        */
#define ERU_EXICON_SS_Pos                     (8UL)                     /*!< ERU EXICON: SS (Bit 8)                                      */
#define ERU_EXICON_SS_Msk                     (0x300UL)                 /*!< ERU EXICON: SS (Bitfield-Mask: 0x03)                        */
#define ERU_EXICON_NA_Pos                     (10UL)                    /*!< ERU EXICON: NA (Bit 10)                                     */
#define ERU_EXICON_NA_Msk                     (0x400UL)                 /*!< ERU EXICON: NA (Bitfield-Mask: 0x01)                        */
#define ERU_EXICON_NB_Pos                     (11UL)                    /*!< ERU EXICON: NB (Bit 11)                                     */
#define ERU_EXICON_NB_Msk                     (0x800UL)                 /*!< ERU EXICON: NB (Bitfield-Mask: 0x01)                        */

/* ---------------------------------  ERU_EXOCON  --------------------------------- */
#define ERU_EXOCON_ISS_Pos                    (0UL)                     /*!< ERU EXOCON: ISS (Bit 0)                                     */
#define ERU_EXOCON_ISS_Msk                    (0x3UL)                   /*!< ERU EXOCON: ISS (Bitfield-Mask: 0x03)                       */
#define ERU_EXOCON_GEEN_Pos                   (2UL)                     /*!< ERU EXOCON: GEEN (Bit 2)                                    */
#define ERU_EXOCON_GEEN_Msk                   (0x4UL)                   /*!< ERU EXOCON: GEEN (Bitfield-Mask: 0x01)                      */
#define ERU_EXOCON_PDR_Pos                    (3UL)                     /*!< ERU EXOCON: PDR (Bit 3)                                     */
#define ERU_EXOCON_PDR_Msk                    (0x8UL)                   /*!< ERU EXOCON: PDR (Bitfield-Mask: 0x01)                       */
#define ERU_EXOCON_GP_Pos                     (4UL)                     /*!< ERU EXOCON: GP (Bit 4)                                      */
#define ERU_EXOCON_GP_Msk                     (0x30UL)                  /*!< ERU EXOCON: GP (Bitfield-Mask: 0x03)                        */
#define ERU_EXOCON_IPEN0_Pos                  (12UL)                    /*!< ERU EXOCON: IPEN0 (Bit 12)                                  */
#define ERU_EXOCON_IPEN0_Msk                  (0x1000UL)                /*!< ERU EXOCON: IPEN0 (Bitfield-Mask: 0x01)                     */
#define ERU_EXOCON_IPEN1_Pos                  (13UL)                    /*!< ERU EXOCON: IPEN1 (Bit 13)                                  */
#define ERU_EXOCON_IPEN1_Msk                  (0x2000UL)                /*!< ERU EXOCON: IPEN1 (Bitfield-Mask: 0x01)                     */
#define ERU_EXOCON_IPEN2_Pos                  (14UL)                    /*!< ERU EXOCON: IPEN2 (Bit 14)                                  */
#define ERU_EXOCON_IPEN2_Msk                  (0x4000UL)                /*!< ERU EXOCON: IPEN2 (Bitfield-Mask: 0x01)                     */
#define ERU_EXOCON_IPEN3_Pos                  (15UL)                    /*!< ERU EXOCON: IPEN3 (Bit 15)                                  */
#define ERU_EXOCON_IPEN3_Msk                  (0x8000UL)                /*!< ERU EXOCON: IPEN3 (Bitfield-Mask: 0x01)                     */


/* ================================================================================ */
/* ================          struct 'PAU' Position & Mask          ================ */
/* ================================================================================ */


/* ---------------------------------  PAU_AVAIL0  --------------------------------- */
#define PAU_AVAIL0_AVAIL5_Pos                 (5UL)                     /*!< PAU AVAIL0: AVAIL5 (Bit 5)                                  */
#define PAU_AVAIL0_AVAIL5_Msk                 (0x20UL)                  /*!< PAU AVAIL0: AVAIL5 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL0_AVAIL6_Pos                 (6UL)                     /*!< PAU AVAIL0: AVAIL6 (Bit 6)                                  */
#define PAU_AVAIL0_AVAIL6_Msk                 (0x40UL)                  /*!< PAU AVAIL0: AVAIL6 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL0_AVAIL7_Pos                 (7UL)                     /*!< PAU AVAIL0: AVAIL7 (Bit 7)                                  */
#define PAU_AVAIL0_AVAIL7_Msk                 (0x80UL)                  /*!< PAU AVAIL0: AVAIL7 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL0_AVAIL22_Pos                (22UL)                    /*!< PAU AVAIL0: AVAIL22 (Bit 22)                                */
#define PAU_AVAIL0_AVAIL22_Msk                (0x400000UL)              /*!< PAU AVAIL0: AVAIL22 (Bitfield-Mask: 0x01)                   */
#define PAU_AVAIL0_AVAIL23_Pos                (23UL)                    /*!< PAU AVAIL0: AVAIL23 (Bit 23)                                */
#define PAU_AVAIL0_AVAIL23_Msk                (0x800000UL)              /*!< PAU AVAIL0: AVAIL23 (Bitfield-Mask: 0x01)                   */
#define PAU_AVAIL0_AVAIL24_Pos                (24UL)                    /*!< PAU AVAIL0: AVAIL24 (Bit 24)                                */
#define PAU_AVAIL0_AVAIL24_Msk                (0x1000000UL)             /*!< PAU AVAIL0: AVAIL24 (Bitfield-Mask: 0x01)                   */

/* ---------------------------------  PAU_AVAIL1  --------------------------------- */
#define PAU_AVAIL1_AVAIL0_Pos                 (0UL)                     /*!< PAU AVAIL1: AVAIL0 (Bit 0)                                  */
#define PAU_AVAIL1_AVAIL0_Msk                 (0x1UL)                   /*!< PAU AVAIL1: AVAIL0 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL1_AVAIL1_Pos                 (1UL)                     /*!< PAU AVAIL1: AVAIL1 (Bit 1)                                  */
#define PAU_AVAIL1_AVAIL1_Msk                 (0x2UL)                   /*!< PAU AVAIL1: AVAIL1 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL1_AVAIL4_Pos                 (4UL)                     /*!< PAU AVAIL1: AVAIL4 (Bit 4)                                  */
#define PAU_AVAIL1_AVAIL4_Msk                 (0x10UL)                  /*!< PAU AVAIL1: AVAIL4 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL1_AVAIL5_Pos                 (5UL)                     /*!< PAU AVAIL1: AVAIL5 (Bit 5)                                  */
#define PAU_AVAIL1_AVAIL5_Msk                 (0x20UL)                  /*!< PAU AVAIL1: AVAIL5 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL1_AVAIL8_Pos                 (8UL)                     /*!< PAU AVAIL1: AVAIL8 (Bit 8)                                  */
#define PAU_AVAIL1_AVAIL8_Msk                 (0x100UL)                 /*!< PAU AVAIL1: AVAIL8 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL1_AVAIL9_Pos                 (9UL)                     /*!< PAU AVAIL1: AVAIL9 (Bit 9)                                  */
#define PAU_AVAIL1_AVAIL9_Msk                 (0x200UL)                 /*!< PAU AVAIL1: AVAIL9 (Bitfield-Mask: 0x01)                    */
#define PAU_AVAIL1_AVAIL10_Pos                (10UL)                    /*!< PAU AVAIL1: AVAIL10 (Bit 10)                                */
#define PAU_AVAIL1_AVAIL10_Msk                (0x400UL)                 /*!< PAU AVAIL1: AVAIL10 (Bitfield-Mask: 0x01)                   */
#define PAU_AVAIL1_AVAIL11_Pos                (11UL)                    /*!< PAU AVAIL1: AVAIL11 (Bit 11)                                */
#define PAU_AVAIL1_AVAIL11_Msk                (0x800UL)                 /*!< PAU AVAIL1: AVAIL11 (Bitfield-Mask: 0x01)                   */
#define PAU_AVAIL1_AVAIL12_Pos                (12UL)                    /*!< PAU AVAIL1: AVAIL12 (Bit 12)                                */
#define PAU_AVAIL1_AVAIL12_Msk                (0x1000UL)                /*!< PAU AVAIL1: AVAIL12 (Bitfield-Mask: 0x01)                   */

/* --------------------------------  PAU_PRIVDIS0  -------------------------------- */
#define PAU_PRIVDIS0_PDIS2_Pos                (2UL)                     /*!< PAU PRIVDIS0: PDIS2 (Bit 2)                                 */
#define PAU_PRIVDIS0_PDIS2_Msk                (0x4UL)                   /*!< PAU PRIVDIS0: PDIS2 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS0_PDIS5_Pos                (5UL)                     /*!< PAU PRIVDIS0: PDIS5 (Bit 5)                                 */
#define PAU_PRIVDIS0_PDIS5_Msk                (0x20UL)                  /*!< PAU PRIVDIS0: PDIS5 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS0_PDIS6_Pos                (6UL)                     /*!< PAU PRIVDIS0: PDIS6 (Bit 6)                                 */
#define PAU_PRIVDIS0_PDIS6_Msk                (0x40UL)                  /*!< PAU PRIVDIS0: PDIS6 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS0_PDIS7_Pos                (7UL)                     /*!< PAU PRIVDIS0: PDIS7 (Bit 7)                                 */
#define PAU_PRIVDIS0_PDIS7_Msk                (0x80UL)                  /*!< PAU PRIVDIS0: PDIS7 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS0_PDIS19_Pos               (19UL)                    /*!< PAU PRIVDIS0: PDIS19 (Bit 19)                               */
#define PAU_PRIVDIS0_PDIS19_Msk               (0x80000UL)               /*!< PAU PRIVDIS0: PDIS19 (Bitfield-Mask: 0x01)                  */
#define PAU_PRIVDIS0_PDIS22_Pos               (22UL)                    /*!< PAU PRIVDIS0: PDIS22 (Bit 22)                               */
#define PAU_PRIVDIS0_PDIS22_Msk               (0x400000UL)              /*!< PAU PRIVDIS0: PDIS22 (Bitfield-Mask: 0x01)                  */
#define PAU_PRIVDIS0_PDIS23_Pos               (23UL)                    /*!< PAU PRIVDIS0: PDIS23 (Bit 23)                               */
#define PAU_PRIVDIS0_PDIS23_Msk               (0x800000UL)              /*!< PAU PRIVDIS0: PDIS23 (Bitfield-Mask: 0x01)                  */
#define PAU_PRIVDIS0_PDIS24_Pos               (24UL)                    /*!< PAU PRIVDIS0: PDIS24 (Bit 24)                               */
#define PAU_PRIVDIS0_PDIS24_Msk               (0x1000000UL)             /*!< PAU PRIVDIS0: PDIS24 (Bitfield-Mask: 0x01)                  */

/* --------------------------------  PAU_PRIVDIS1  -------------------------------- */
#define PAU_PRIVDIS1_PDIS0_Pos                (0UL)                     /*!< PAU PRIVDIS1: PDIS0 (Bit 0)                                 */
#define PAU_PRIVDIS1_PDIS0_Msk                (0x1UL)                   /*!< PAU PRIVDIS1: PDIS0 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS1_PDIS1_Pos                (1UL)                     /*!< PAU PRIVDIS1: PDIS1 (Bit 1)                                 */
#define PAU_PRIVDIS1_PDIS1_Msk                (0x2UL)                   /*!< PAU PRIVDIS1: PDIS1 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS1_PDIS5_Pos                (5UL)                     /*!< PAU PRIVDIS1: PDIS5 (Bit 5)                                 */
#define PAU_PRIVDIS1_PDIS5_Msk                (0x20UL)                  /*!< PAU PRIVDIS1: PDIS5 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS1_PDIS8_Pos                (8UL)                     /*!< PAU PRIVDIS1: PDIS8 (Bit 8)                                 */
#define PAU_PRIVDIS1_PDIS8_Msk                (0x100UL)                 /*!< PAU PRIVDIS1: PDIS8 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS1_PDIS9_Pos                (9UL)                     /*!< PAU PRIVDIS1: PDIS9 (Bit 9)                                 */
#define PAU_PRIVDIS1_PDIS9_Msk                (0x200UL)                 /*!< PAU PRIVDIS1: PDIS9 (Bitfield-Mask: 0x01)                   */
#define PAU_PRIVDIS1_PDIS10_Pos               (10UL)                    /*!< PAU PRIVDIS1: PDIS10 (Bit 10)                               */
#define PAU_PRIVDIS1_PDIS10_Msk               (0x400UL)                 /*!< PAU PRIVDIS1: PDIS10 (Bitfield-Mask: 0x01)                  */
#define PAU_PRIVDIS1_PDIS11_Pos               (11UL)                    /*!< PAU PRIVDIS1: PDIS11 (Bit 11)                               */
#define PAU_PRIVDIS1_PDIS11_Msk               (0x800UL)                 /*!< PAU PRIVDIS1: PDIS11 (Bitfield-Mask: 0x01)                  */
#define PAU_PRIVDIS1_PDIS12_Pos               (12UL)                    /*!< PAU PRIVDIS1: PDIS12 (Bit 12)                               */
#define PAU_PRIVDIS1_PDIS12_Msk               (0x1000UL)                /*!< PAU PRIVDIS1: PDIS12 (Bitfield-Mask: 0x01)                  */

/* ---------------------------------  PAU_ROMSIZE  -------------------------------- */
#define PAU_ROMSIZE_ADDR_Pos                  (8UL)                     /*!< PAU ROMSIZE: ADDR (Bit 8)                                   */
#define PAU_ROMSIZE_ADDR_Msk                  (0x3f00UL)                /*!< PAU ROMSIZE: ADDR (Bitfield-Mask: 0x3f)                     */

/* ---------------------------------  PAU_FLSIZE  --------------------------------- */
#define PAU_FLSIZE_ADDR_Pos                   (12UL)                    /*!< PAU FLSIZE: ADDR (Bit 12)                                   */
#define PAU_FLSIZE_ADDR_Msk                   (0x3f000UL)               /*!< PAU FLSIZE: ADDR (Bitfield-Mask: 0x3f)                      */

/* --------------------------------  PAU_RAM0SIZE  -------------------------------- */
#define PAU_RAM0SIZE_ADDR_Pos                 (8UL)                     /*!< PAU RAM0SIZE: ADDR (Bit 8)                                  */
#define PAU_RAM0SIZE_ADDR_Msk                 (0x1f00UL)                /*!< PAU RAM0SIZE: ADDR (Bitfield-Mask: 0x1f)                    */


/* ================================================================================ */
/* ================          struct 'NVM' Position & Mask          ================ */
/* ================================================================================ */


/* --------------------------------  NVM_NVMSTATUS  ------------------------------- */
#define NVM_NVMSTATUS_BUSY_Pos                (0UL)                     /*!< NVM NVMSTATUS: BUSY (Bit 0)                                 */
#define NVM_NVMSTATUS_BUSY_Msk                (0x1UL)                   /*!< NVM NVMSTATUS: BUSY (Bitfield-Mask: 0x01)                   */
#define NVM_NVMSTATUS_SLEEP_Pos               (1UL)                     /*!< NVM NVMSTATUS: SLEEP (Bit 1)                                */
#define NVM_NVMSTATUS_SLEEP_Msk               (0x2UL)                   /*!< NVM NVMSTATUS: SLEEP (Bitfield-Mask: 0x01)                  */
#define NVM_NVMSTATUS_VERR_Pos                (2UL)                     /*!< NVM NVMSTATUS: VERR (Bit 2)                                 */
#define NVM_NVMSTATUS_VERR_Msk                (0xcUL)                   /*!< NVM NVMSTATUS: VERR (Bitfield-Mask: 0x03)                   */
#define NVM_NVMSTATUS_ECC1READ_Pos            (4UL)                     /*!< NVM NVMSTATUS: ECC1READ (Bit 4)                             */
#define NVM_NVMSTATUS_ECC1READ_Msk            (0x10UL)                  /*!< NVM NVMSTATUS: ECC1READ (Bitfield-Mask: 0x01)               */
#define NVM_NVMSTATUS_ECC2READ_Pos            (5UL)                     /*!< NVM NVMSTATUS: ECC2READ (Bit 5)                             */
#define NVM_NVMSTATUS_ECC2READ_Msk            (0x20UL)                  /*!< NVM NVMSTATUS: ECC2READ (Bitfield-Mask: 0x01)               */
#define NVM_NVMSTATUS_WRPERR_Pos              (6UL)                     /*!< NVM NVMSTATUS: WRPERR (Bit 6)                               */
#define NVM_NVMSTATUS_WRPERR_Msk              (0x40UL)                  /*!< NVM NVMSTATUS: WRPERR (Bitfield-Mask: 0x01)                 */

/* ---------------------------------  NVM_NVMPROG  -------------------------------- */
#define NVM_NVMPROG_ACTION_Pos                (0UL)                     /*!< NVM NVMPROG: ACTION (Bit 0)                                 */
#define NVM_NVMPROG_ACTION_Msk                (0xffUL)                  /*!< NVM NVMPROG: ACTION (Bitfield-Mask: 0xff)                   */
#define NVM_NVMPROG_RSTVERR_Pos               (12UL)                    /*!< NVM NVMPROG: RSTVERR (Bit 12)                               */
#define NVM_NVMPROG_RSTVERR_Msk               (0x1000UL)                /*!< NVM NVMPROG: RSTVERR (Bitfield-Mask: 0x01)                  */
#define NVM_NVMPROG_RSTECC_Pos                (13UL)                    /*!< NVM NVMPROG: RSTECC (Bit 13)                                */
#define NVM_NVMPROG_RSTECC_Msk                (0x2000UL)                /*!< NVM NVMPROG: RSTECC (Bitfield-Mask: 0x01)                   */

/* ---------------------------------  NVM_NVMCONF  -------------------------------- */
#define NVM_NVMCONF_HRLEV_Pos                 (1UL)                     /*!< NVM NVMCONF: HRLEV (Bit 1)                                  */
#define NVM_NVMCONF_HRLEV_Msk                 (0x6UL)                   /*!< NVM NVMCONF: HRLEV (Bitfield-Mask: 0x03)                    */
#define NVM_NVMCONF_SECPROT_Pos               (4UL)                     /*!< NVM NVMCONF: SECPROT (Bit 4)                                */
#define NVM_NVMCONF_SECPROT_Msk               (0xff0UL)                 /*!< NVM NVMCONF: SECPROT (Bitfield-Mask: 0xff)                  */
#define NVM_NVMCONF_INT_ON_Pos                (14UL)                    /*!< NVM NVMCONF: INT_ON (Bit 14)                                */
#define NVM_NVMCONF_INT_ON_Msk                (0x4000UL)                /*!< NVM NVMCONF: INT_ON (Bitfield-Mask: 0x01)                   */
#define NVM_NVMCONF_NVM_ON_Pos                (15UL)                    /*!< NVM NVMCONF: NVM_ON (Bit 15)                                */
#define NVM_NVMCONF_NVM_ON_Msk                (0x8000UL)                /*!< NVM NVMCONF: NVM_ON (Bitfield-Mask: 0x01)                   */


/* ================================================================================ */
/* ================          struct 'WDT' Position & Mask          ================ */
/* ================================================================================ */


/* -----------------------------------  WDT_ID  ----------------------------------- */
#define WDT_ID_MOD_REV_Pos                    (0UL)                     /*!< WDT ID: MOD_REV (Bit 0)                                     */
#define WDT_ID_MOD_REV_Msk                    (0xffUL)                  /*!< WDT ID: MOD_REV (Bitfield-Mask: 0xff)                       */
#define WDT_ID_MOD_TYPE_Pos                   (8UL)                     /*!< WDT ID: MOD_TYPE (Bit 8)                                    */
#define WDT_ID_MOD_TYPE_Msk                   (0xff00UL)                /*!< WDT ID: MOD_TYPE (Bitfield-Mask: 0xff)                      */
#define WDT_ID_MOD_NUMBER_Pos                 (16UL)                    /*!< WDT ID: MOD_NUMBER (Bit 16)                                 */
#define WDT_ID_MOD_NUMBER_Msk                 (0xffff0000UL)            /*!< WDT ID: MOD_NUMBER (Bitfield-Mask: 0xffff)                  */

/* -----------------------------------  WDT_CTR  ---------------------------------- */
#define WDT_CTR_ENB_Pos                       (0UL)                     /*!< WDT CTR: ENB (Bit 0)                                        */
#define WDT_CTR_ENB_Msk                       (0x1UL)                   /*!< WDT CTR: ENB (Bitfield-Mask: 0x01)                          */
#define WDT_CTR_PRE_Pos                       (1UL)                     /*!< WDT CTR: PRE (Bit 1)                                        */
#define WDT_CTR_PRE_Msk                       (0x2UL)                   /*!< WDT CTR: PRE (Bitfield-Mask: 0x01)                          */
#define WDT_CTR_DSP_Pos                       (4UL)                     /*!< WDT CTR: DSP (Bit 4)                                        */
#define WDT_CTR_DSP_Msk                       (0x10UL)                  /*!< WDT CTR: DSP (Bitfield-Mask: 0x01)                          */
#define WDT_CTR_SPW_Pos                       (8UL)                     /*!< WDT CTR: SPW (Bit 8)                                        */
#define WDT_CTR_SPW_Msk                       (0xff00UL)                /*!< WDT CTR: SPW (Bitfield-Mask: 0xff)                          */

/* -----------------------------------  WDT_SRV  ---------------------------------- */
#define WDT_SRV_SRV_Pos                       (0UL)                     /*!< WDT SRV: SRV (Bit 0)                                        */
#define WDT_SRV_SRV_Msk                       (0xffffffffUL)            /*!< WDT SRV: SRV (Bitfield-Mask: 0xffffffff)                    */

/* -----------------------------------  WDT_TIM  ---------------------------------- */
#define WDT_TIM_TIM_Pos                       (0UL)                     /*!< WDT TIM: TIM (Bit 0)                                        */
#define WDT_TIM_TIM_Msk                       (0xffffffffUL)            /*!< WDT TIM: TIM (Bitfield-Mask: 0xffffffff)                    */

/* -----------------------------------  WDT_WLB  ---------------------------------- */
#define WDT_WLB_WLB_Pos                       (0UL)                     /*!< WDT WLB: WLB (Bit 0)                                        */
#define WDT_WLB_WLB_Msk                       (0xffffffffUL)            /*!< WDT WLB: WLB (Bitfield-Mask: 0xffffffff)                    */

/* -----------------------------------  WDT_WUB  ---------------------------------- */
#define WDT_WUB_WUB_Pos                       (0UL)                     /*!< WDT WUB: WUB (Bit 0)                                        */
#define WDT_WUB_WUB_Msk                       (0xffffffffUL)            /*!< WDT WUB: WUB (Bitfield-Mask: 0xffffffff)                    */

/* ---------------------------------  WDT_WDTSTS  --------------------------------- */
#define WDT_WDTSTS_ALMS_Pos                   (0UL)                     /*!< WDT WDTSTS: ALMS (Bit 0)                                    */
#define WDT_WDTSTS_ALMS_Msk                   (0x1UL)                   /*!< WDT WDTSTS: ALMS (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  WDT_WDTCLR  --------------------------------- */
#define WDT_WDTCLR_ALMC_Pos                   (0UL)                     /*!< WDT WDTCLR: ALMC (Bit 0)                                    */
#define WDT_WDTCLR_ALMC_Msk                   (0x1UL)                   /*!< WDT WDTCLR: ALMC (Bitfield-Mask: 0x01)                      */


/* ================================================================================ */
/* ================          struct 'RTC' Position & Mask          ================ */
/* ================================================================================ */


/* -----------------------------------  RTC_ID  ----------------------------------- */
#define RTC_ID_MOD_REV_Pos                    (0UL)                     /*!< RTC ID: MOD_REV (Bit 0)                                     */
#define RTC_ID_MOD_REV_Msk                    (0xffUL)                  /*!< RTC ID: MOD_REV (Bitfield-Mask: 0xff)                       */
#define RTC_ID_MOD_TYPE_Pos                   (8UL)                     /*!< RTC ID: MOD_TYPE (Bit 8)                                    */
#define RTC_ID_MOD_TYPE_Msk                   (0xff00UL)                /*!< RTC ID: MOD_TYPE (Bitfield-Mask: 0xff)                      */
#define RTC_ID_MOD_NUMBER_Pos                 (16UL)                    /*!< RTC ID: MOD_NUMBER (Bit 16)                                 */
#define RTC_ID_MOD_NUMBER_Msk                 (0xffff0000UL)            /*!< RTC ID: MOD_NUMBER (Bitfield-Mask: 0xffff)                  */

/* -----------------------------------  RTC_CTR  ---------------------------------- */
#define RTC_CTR_ENB_Pos                       (0UL)                     /*!< RTC CTR: ENB (Bit 0)                                        */
#define RTC_CTR_ENB_Msk                       (0x1UL)                   /*!< RTC CTR: ENB (Bitfield-Mask: 0x01)                          */
#define RTC_CTR_SUS_Pos                       (1UL)                     /*!< RTC CTR: SUS (Bit 1)                                        */
#define RTC_CTR_SUS_Msk                       (0x2UL)                   /*!< RTC CTR: SUS (Bitfield-Mask: 0x01)                          */
#define RTC_CTR_DIV_Pos                       (16UL)                    /*!< RTC CTR: DIV (Bit 16)                                       */
#define RTC_CTR_DIV_Msk                       (0xffff0000UL)            /*!< RTC CTR: DIV (Bitfield-Mask: 0xffff)                        */

/* ---------------------------------  RTC_RAWSTAT  -------------------------------- */
#define RTC_RAWSTAT_RPSE_Pos                  (0UL)                     /*!< RTC RAWSTAT: RPSE (Bit 0)                                   */
#define RTC_RAWSTAT_RPSE_Msk                  (0x1UL)                   /*!< RTC RAWSTAT: RPSE (Bitfield-Mask: 0x01)                     */
#define RTC_RAWSTAT_RPMI_Pos                  (1UL)                     /*!< RTC RAWSTAT: RPMI (Bit 1)                                   */
#define RTC_RAWSTAT_RPMI_Msk                  (0x2UL)                   /*!< RTC RAWSTAT: RPMI (Bitfield-Mask: 0x01)                     */
#define RTC_RAWSTAT_RPHO_Pos                  (2UL)                     /*!< RTC RAWSTAT: RPHO (Bit 2)                                   */
#define RTC_RAWSTAT_RPHO_Msk                  (0x4UL)                   /*!< RTC RAWSTAT: RPHO (Bitfield-Mask: 0x01)                     */
#define RTC_RAWSTAT_RPDA_Pos                  (3UL)                     /*!< RTC RAWSTAT: RPDA (Bit 3)                                   */
#define RTC_RAWSTAT_RPDA_Msk                  (0x8UL)                   /*!< RTC RAWSTAT: RPDA (Bitfield-Mask: 0x01)                     */
#define RTC_RAWSTAT_RPMO_Pos                  (5UL)                     /*!< RTC RAWSTAT: RPMO (Bit 5)                                   */
#define RTC_RAWSTAT_RPMO_Msk                  (0x20UL)                  /*!< RTC RAWSTAT: RPMO (Bitfield-Mask: 0x01)                     */
#define RTC_RAWSTAT_RPYE_Pos                  (6UL)                     /*!< RTC RAWSTAT: RPYE (Bit 6)                                   */
#define RTC_RAWSTAT_RPYE_Msk                  (0x40UL)                  /*!< RTC RAWSTAT: RPYE (Bitfield-Mask: 0x01)                     */
#define RTC_RAWSTAT_RAI_Pos                   (8UL)                     /*!< RTC RAWSTAT: RAI (Bit 8)                                    */
#define RTC_RAWSTAT_RAI_Msk                   (0x100UL)                 /*!< RTC RAWSTAT: RAI (Bitfield-Mask: 0x01)                      */

/* ----------------------------------  RTC_STSSR  --------------------------------- */
#define RTC_STSSR_SPSE_Pos                    (0UL)                     /*!< RTC STSSR: SPSE (Bit 0)                                     */
#define RTC_STSSR_SPSE_Msk                    (0x1UL)                   /*!< RTC STSSR: SPSE (Bitfield-Mask: 0x01)                       */
#define RTC_STSSR_SPMI_Pos                    (1UL)                     /*!< RTC STSSR: SPMI (Bit 1)                                     */
#define RTC_STSSR_SPMI_Msk                    (0x2UL)                   /*!< RTC STSSR: SPMI (Bitfield-Mask: 0x01)                       */
#define RTC_STSSR_SPHO_Pos                    (2UL)                     /*!< RTC STSSR: SPHO (Bit 2)                                     */
#define RTC_STSSR_SPHO_Msk                    (0x4UL)                   /*!< RTC STSSR: SPHO (Bitfield-Mask: 0x01)                       */
#define RTC_STSSR_SPDA_Pos                    (3UL)                     /*!< RTC STSSR: SPDA (Bit 3)                                     */
#define RTC_STSSR_SPDA_Msk                    (0x8UL)                   /*!< RTC STSSR: SPDA (Bitfield-Mask: 0x01)                       */
#define RTC_STSSR_SPMO_Pos                    (5UL)                     /*!< RTC STSSR: SPMO (Bit 5)                                     */
#define RTC_STSSR_SPMO_Msk                    (0x20UL)                  /*!< RTC STSSR: SPMO (Bitfield-Mask: 0x01)                       */
#define RTC_STSSR_SPYE_Pos                    (6UL)                     /*!< RTC STSSR: SPYE (Bit 6)                                     */
#define RTC_STSSR_SPYE_Msk                    (0x40UL)                  /*!< RTC STSSR: SPYE (Bitfield-Mask: 0x01)                       */
#define RTC_STSSR_SAI_Pos                     (8UL)                     /*!< RTC STSSR: SAI (Bit 8)                                      */
#define RTC_STSSR_SAI_Msk                     (0x100UL)                 /*!< RTC STSSR: SAI (Bitfield-Mask: 0x01)                        */

/* ----------------------------------  RTC_MSKSR  --------------------------------- */
#define RTC_MSKSR_MPSE_Pos                    (0UL)                     /*!< RTC MSKSR: MPSE (Bit 0)                                     */
#define RTC_MSKSR_MPSE_Msk                    (0x1UL)                   /*!< RTC MSKSR: MPSE (Bitfield-Mask: 0x01)                       */
#define RTC_MSKSR_MPMI_Pos                    (1UL)                     /*!< RTC MSKSR: MPMI (Bit 1)                                     */
#define RTC_MSKSR_MPMI_Msk                    (0x2UL)                   /*!< RTC MSKSR: MPMI (Bitfield-Mask: 0x01)                       */
#define RTC_MSKSR_MPHO_Pos                    (2UL)                     /*!< RTC MSKSR: MPHO (Bit 2)                                     */
#define RTC_MSKSR_MPHO_Msk                    (0x4UL)                   /*!< RTC MSKSR: MPHO (Bitfield-Mask: 0x01)                       */
#define RTC_MSKSR_MPDA_Pos                    (3UL)                     /*!< RTC MSKSR: MPDA (Bit 3)                                     */
#define RTC_MSKSR_MPDA_Msk                    (0x8UL)                   /*!< RTC MSKSR: MPDA (Bitfield-Mask: 0x01)                       */
#define RTC_MSKSR_MPMO_Pos                    (5UL)                     /*!< RTC MSKSR: MPMO (Bit 5)                                     */
#define RTC_MSKSR_MPMO_Msk                    (0x20UL)                  /*!< RTC MSKSR: MPMO (Bitfield-Mask: 0x01)                       */
#define RTC_MSKSR_MPYE_Pos                    (6UL)                     /*!< RTC MSKSR: MPYE (Bit 6)                                     */
#define RTC_MSKSR_MPYE_Msk                    (0x40UL)                  /*!< RTC MSKSR: MPYE (Bitfield-Mask: 0x01)                       */
#define RTC_MSKSR_MAI_Pos                     (8UL)                     /*!< RTC MSKSR: MAI (Bit 8)                                      */
#define RTC_MSKSR_MAI_Msk                     (0x100UL)                 /*!< RTC MSKSR: MAI (Bitfield-Mask: 0x01)                        */

/* ----------------------------------  RTC_CLRSR  --------------------------------- */
#define RTC_CLRSR_RPSE_Pos                    (0UL)                     /*!< RTC CLRSR: RPSE (Bit 0)                                     */
#define RTC_CLRSR_RPSE_Msk                    (0x1UL)                   /*!< RTC CLRSR: RPSE (Bitfield-Mask: 0x01)                       */
#define RTC_CLRSR_RPMI_Pos                    (1UL)                     /*!< RTC CLRSR: RPMI (Bit 1)                                     */
#define RTC_CLRSR_RPMI_Msk                    (0x2UL)                   /*!< RTC CLRSR: RPMI (Bitfield-Mask: 0x01)                       */
#define RTC_CLRSR_RPHO_Pos                    (2UL)                     /*!< RTC CLRSR: RPHO (Bit 2)                                     */
#define RTC_CLRSR_RPHO_Msk                    (0x4UL)                   /*!< RTC CLRSR: RPHO (Bitfield-Mask: 0x01)                       */
#define RTC_CLRSR_RPDA_Pos                    (3UL)                     /*!< RTC CLRSR: RPDA (Bit 3)                                     */
#define RTC_CLRSR_RPDA_Msk                    (0x8UL)                   /*!< RTC CLRSR: RPDA (Bitfield-Mask: 0x01)                       */
#define RTC_CLRSR_RPMO_Pos                    (5UL)                     /*!< RTC CLRSR: RPMO (Bit 5)                                     */
#define RTC_CLRSR_RPMO_Msk                    (0x20UL)                  /*!< RTC CLRSR: RPMO (Bitfield-Mask: 0x01)                       */
#define RTC_CLRSR_RPYE_Pos                    (6UL)                     /*!< RTC CLRSR: RPYE (Bit 6)                                     */
#define RTC_CLRSR_RPYE_Msk                    (0x40UL)                  /*!< RTC CLRSR: RPYE (Bitfield-Mask: 0x01)                       */
#define RTC_CLRSR_RAI_Pos                     (8UL)                     /*!< RTC CLRSR: RAI (Bit 8)                                      */
#define RTC_CLRSR_RAI_Msk                     (0x100UL)                 /*!< RTC CLRSR: RAI (Bitfield-Mask: 0x01)                        */

/* ----------------------------------  RTC_ATIM0  --------------------------------- */
#define RTC_ATIM0_ASE_Pos                     (0UL)                     /*!< RTC ATIM0: ASE (Bit 0)                                      */
#define RTC_ATIM0_ASE_Msk                     (0x3fUL)                  /*!< RTC ATIM0: ASE (Bitfield-Mask: 0x3f)                        */
#define RTC_ATIM0_AMI_Pos                     (8UL)                     /*!< RTC ATIM0: AMI (Bit 8)                                      */
#define RTC_ATIM0_AMI_Msk                     (0x3f00UL)                /*!< RTC ATIM0: AMI (Bitfield-Mask: 0x3f)                        */
#define RTC_ATIM0_AHO_Pos                     (16UL)                    /*!< RTC ATIM0: AHO (Bit 16)                                     */
#define RTC_ATIM0_AHO_Msk                     (0x1f0000UL)              /*!< RTC ATIM0: AHO (Bitfield-Mask: 0x1f)                        */
#define RTC_ATIM0_ADA_Pos                     (24UL)                    /*!< RTC ATIM0: ADA (Bit 24)                                     */
#define RTC_ATIM0_ADA_Msk                     (0x1f000000UL)            /*!< RTC ATIM0: ADA (Bitfield-Mask: 0x1f)                        */

/* ----------------------------------  RTC_ATIM1  --------------------------------- */
#define RTC_ATIM1_AMO_Pos                     (8UL)                     /*!< RTC ATIM1: AMO (Bit 8)                                      */
#define RTC_ATIM1_AMO_Msk                     (0xf00UL)                 /*!< RTC ATIM1: AMO (Bitfield-Mask: 0x0f)                        */
#define RTC_ATIM1_AYE_Pos                     (16UL)                    /*!< RTC ATIM1: AYE (Bit 16)                                     */
#define RTC_ATIM1_AYE_Msk                     (0xffff0000UL)            /*!< RTC ATIM1: AYE (Bitfield-Mask: 0xffff)                      */

/* ----------------------------------  RTC_TIM0  ---------------------------------- */
#define RTC_TIM0_SE_Pos                       (0UL)                     /*!< RTC TIM0: SE (Bit 0)                                        */
#define RTC_TIM0_SE_Msk                       (0x3fUL)                  /*!< RTC TIM0: SE (Bitfield-Mask: 0x3f)                          */
#define RTC_TIM0_MI_Pos                       (8UL)                     /*!< RTC TIM0: MI (Bit 8)                                        */
#define RTC_TIM0_MI_Msk                       (0x3f00UL)                /*!< RTC TIM0: MI (Bitfield-Mask: 0x3f)                          */
#define RTC_TIM0_HO_Pos                       (16UL)                    /*!< RTC TIM0: HO (Bit 16)                                       */
#define RTC_TIM0_HO_Msk                       (0x1f0000UL)              /*!< RTC TIM0: HO (Bitfield-Mask: 0x1f)                          */
#define RTC_TIM0_DA_Pos                       (24UL)                    /*!< RTC TIM0: DA (Bit 24)                                       */
#define RTC_TIM0_DA_Msk                       (0x1f000000UL)            /*!< RTC TIM0: DA (Bitfield-Mask: 0x1f)                          */

/* ----------------------------------  RTC_TIM1  ---------------------------------- */
#define RTC_TIM1_DAWE_Pos                     (0UL)                     /*!< RTC TIM1: DAWE (Bit 0)                                      */
#define RTC_TIM1_DAWE_Msk                     (0x7UL)                   /*!< RTC TIM1: DAWE (Bitfield-Mask: 0x07)                        */
#define RTC_TIM1_MO_Pos                       (8UL)                     /*!< RTC TIM1: MO (Bit 8)                                        */
#define RTC_TIM1_MO_Msk                       (0xf00UL)                 /*!< RTC TIM1: MO (Bitfield-Mask: 0x0f)                          */
#define RTC_TIM1_YE_Pos                       (16UL)                    /*!< RTC TIM1: YE (Bit 16)                                       */
#define RTC_TIM1_YE_Msk                       (0xffff0000UL)            /*!< RTC TIM1: YE (Bitfield-Mask: 0xffff)                        */


/* ================================================================================ */
/* ================          struct 'PRNG' Position & Mask         ================ */
/* ================================================================================ */


/* ----------------------------------  PRNG_WORD  --------------------------------- */
#define PRNG_WORD_RDATA_Pos                   (0UL)                     /*!< PRNG WORD: RDATA (Bit 0)                                    */
#define PRNG_WORD_RDATA_Msk                   (0xffffUL)                /*!< PRNG WORD: RDATA (Bitfield-Mask: 0xffff)                    */

/* ----------------------------------  PRNG_CHK  ---------------------------------- */
#define PRNG_CHK_RDV_Pos                      (0UL)                     /*!< PRNG CHK: RDV (Bit 0)                                       */
#define PRNG_CHK_RDV_Msk                      (0x1UL)                   /*!< PRNG CHK: RDV (Bitfield-Mask: 0x01)                         */

/* ----------------------------------  PRNG_CTRL  --------------------------------- */
#define PRNG_CTRL_RDBS_Pos                    (1UL)                     /*!< PRNG CTRL: RDBS (Bit 1)                                     */
#define PRNG_CTRL_RDBS_Msk                    (0x6UL)                   /*!< PRNG CTRL: RDBS (Bitfield-Mask: 0x03)                       */
#define PRNG_CTRL_KLD_Pos                     (3UL)                     /*!< PRNG CTRL: KLD (Bit 3)                                      */
#define PRNG_CTRL_KLD_Msk                     (0x8UL)                   /*!< PRNG CTRL: KLD (Bitfield-Mask: 0x01)                        */


/* ================================================================================ */
/* ================          Group 'USIC' Position & Mask          ================ */
/* ================================================================================ */


/* -----------------------------------  USIC_ID  ---------------------------------- */
#define USIC_ID_MOD_REV_Pos                   (0UL)                     /*!< USIC ID: MOD_REV (Bit 0)                                    */
#define USIC_ID_MOD_REV_Msk                   (0xffUL)                  /*!< USIC ID: MOD_REV (Bitfield-Mask: 0xff)                      */
#define USIC_ID_MOD_TYPE_Pos                  (8UL)                     /*!< USIC ID: MOD_TYPE (Bit 8)                                   */
#define USIC_ID_MOD_TYPE_Msk                  (0xff00UL)                /*!< USIC ID: MOD_TYPE (Bitfield-Mask: 0xff)                     */
#define USIC_ID_MOD_NUMBER_Pos                (16UL)                    /*!< USIC ID: MOD_NUMBER (Bit 16)                                */
#define USIC_ID_MOD_NUMBER_Msk                (0xffff0000UL)            /*!< USIC ID: MOD_NUMBER (Bitfield-Mask: 0xffff)                 */


/* ================================================================================ */
/* ================         Group 'USIC_CH' Position & Mask        ================ */
/* ================================================================================ */


/* --------------------------------  USIC_CH_CCFG  -------------------------------- */
#define USIC_CH_CCFG_SSC_Pos                  (0UL)                     /*!< USIC_CH CCFG: SSC (Bit 0)                                   */
#define USIC_CH_CCFG_SSC_Msk                  (0x1UL)                   /*!< USIC_CH CCFG: SSC (Bitfield-Mask: 0x01)                     */
#define USIC_CH_CCFG_ASC_Pos                  (1UL)                     /*!< USIC_CH CCFG: ASC (Bit 1)                                   */
#define USIC_CH_CCFG_ASC_Msk                  (0x2UL)                   /*!< USIC_CH CCFG: ASC (Bitfield-Mask: 0x01)                     */
#define USIC_CH_CCFG_IIC_Pos                  (2UL)                     /*!< USIC_CH CCFG: IIC (Bit 2)                                   */
#define USIC_CH_CCFG_IIC_Msk                  (0x4UL)                   /*!< USIC_CH CCFG: IIC (Bitfield-Mask: 0x01)                     */
#define USIC_CH_CCFG_IIS_Pos                  (3UL)                     /*!< USIC_CH CCFG: IIS (Bit 3)                                   */
#define USIC_CH_CCFG_IIS_Msk                  (0x8UL)                   /*!< USIC_CH CCFG: IIS (Bitfield-Mask: 0x01)                     */
#define USIC_CH_CCFG_RB_Pos                   (6UL)                     /*!< USIC_CH CCFG: RB (Bit 6)                                    */
#define USIC_CH_CCFG_RB_Msk                   (0x40UL)                  /*!< USIC_CH CCFG: RB (Bitfield-Mask: 0x01)                      */
#define USIC_CH_CCFG_TB_Pos                   (7UL)                     /*!< USIC_CH CCFG: TB (Bit 7)                                    */
#define USIC_CH_CCFG_TB_Msk                   (0x80UL)                  /*!< USIC_CH CCFG: TB (Bitfield-Mask: 0x01)                      */

/* --------------------------------  USIC_CH_KSCFG  ------------------------------- */
#define USIC_CH_KSCFG_MODEN_Pos               (0UL)                     /*!< USIC_CH KSCFG: MODEN (Bit 0)                                */
#define USIC_CH_KSCFG_MODEN_Msk               (0x1UL)                   /*!< USIC_CH KSCFG: MODEN (Bitfield-Mask: 0x01)                  */
#define USIC_CH_KSCFG_BPMODEN_Pos             (1UL)                     /*!< USIC_CH KSCFG: BPMODEN (Bit 1)                              */
#define USIC_CH_KSCFG_BPMODEN_Msk             (0x2UL)                   /*!< USIC_CH KSCFG: BPMODEN (Bitfield-Mask: 0x01)                */
#define USIC_CH_KSCFG_NOMCFG_Pos              (4UL)                     /*!< USIC_CH KSCFG: NOMCFG (Bit 4)                               */
#define USIC_CH_KSCFG_NOMCFG_Msk              (0x30UL)                  /*!< USIC_CH KSCFG: NOMCFG (Bitfield-Mask: 0x03)                 */
#define USIC_CH_KSCFG_BPNOM_Pos               (7UL)                     /*!< USIC_CH KSCFG: BPNOM (Bit 7)                                */
#define USIC_CH_KSCFG_BPNOM_Msk               (0x80UL)                  /*!< USIC_CH KSCFG: BPNOM (Bitfield-Mask: 0x01)                  */
#define USIC_CH_KSCFG_SUMCFG_Pos              (8UL)                     /*!< USIC_CH KSCFG: SUMCFG (Bit 8)                               */
#define USIC_CH_KSCFG_SUMCFG_Msk              (0x300UL)                 /*!< USIC_CH KSCFG: SUMCFG (Bitfield-Mask: 0x03)                 */
#define USIC_CH_KSCFG_BPSUM_Pos               (11UL)                    /*!< USIC_CH KSCFG: BPSUM (Bit 11)                               */
#define USIC_CH_KSCFG_BPSUM_Msk               (0x800UL)                 /*!< USIC_CH KSCFG: BPSUM (Bitfield-Mask: 0x01)                  */

/* ---------------------------------  USIC_CH_FDR  -------------------------------- */
#define USIC_CH_FDR_STEP_Pos                  (0UL)                     /*!< USIC_CH FDR: STEP (Bit 0)                                   */
#define USIC_CH_FDR_STEP_Msk                  (0x3ffUL)                 /*!< USIC_CH FDR: STEP (Bitfield-Mask: 0x3ff)                    */
#define USIC_CH_FDR_DM_Pos                    (14UL)                    /*!< USIC_CH FDR: DM (Bit 14)                                    */
#define USIC_CH_FDR_DM_Msk                    (0xc000UL)                /*!< USIC_CH FDR: DM (Bitfield-Mask: 0x03)                       */
#define USIC_CH_FDR_RESULT_Pos                (16UL)                    /*!< USIC_CH FDR: RESULT (Bit 16)                                */
#define USIC_CH_FDR_RESULT_Msk                (0x3ff0000UL)             /*!< USIC_CH FDR: RESULT (Bitfield-Mask: 0x3ff)                  */

/* ---------------------------------  USIC_CH_BRG  -------------------------------- */
#define USIC_CH_BRG_CLKSEL_Pos                (0UL)                     /*!< USIC_CH BRG: CLKSEL (Bit 0)                                 */
#define USIC_CH_BRG_CLKSEL_Msk                (0x3UL)                   /*!< USIC_CH BRG: CLKSEL (Bitfield-Mask: 0x03)                   */
#define USIC_CH_BRG_TMEN_Pos                  (3UL)                     /*!< USIC_CH BRG: TMEN (Bit 3)                                   */
#define USIC_CH_BRG_TMEN_Msk                  (0x8UL)                   /*!< USIC_CH BRG: TMEN (Bitfield-Mask: 0x01)                     */
#define USIC_CH_BRG_PPPEN_Pos                 (4UL)                     /*!< USIC_CH BRG: PPPEN (Bit 4)                                  */
#define USIC_CH_BRG_PPPEN_Msk                 (0x10UL)                  /*!< USIC_CH BRG: PPPEN (Bitfield-Mask: 0x01)                    */
#define USIC_CH_BRG_CTQSEL_Pos                (6UL)                     /*!< USIC_CH BRG: CTQSEL (Bit 6)                                 */
#define USIC_CH_BRG_CTQSEL_Msk                (0xc0UL)                  /*!< USIC_CH BRG: CTQSEL (Bitfield-Mask: 0x03)                   */
#define USIC_CH_BRG_PCTQ_Pos                  (8UL)                     /*!< USIC_CH BRG: PCTQ (Bit 8)                                   */
#define USIC_CH_BRG_PCTQ_Msk                  (0x300UL)                 /*!< USIC_CH BRG: PCTQ (Bitfield-Mask: 0x03)                     */
#define USIC_CH_BRG_DCTQ_Pos                  (10UL)                    /*!< USIC_CH BRG: DCTQ (Bit 10)                                  */
#define USIC_CH_BRG_DCTQ_Msk                  (0x7c00UL)                /*!< USIC_CH BRG: DCTQ (Bitfield-Mask: 0x1f)                     */
#define USIC_CH_BRG_PDIV_Pos                  (16UL)                    /*!< USIC_CH BRG: PDIV (Bit 16)                                  */
#define USIC_CH_BRG_PDIV_Msk                  (0x3ff0000UL)             /*!< USIC_CH BRG: PDIV (Bitfield-Mask: 0x3ff)                    */
#define USIC_CH_BRG_SCLKOSEL_Pos              (28UL)                    /*!< USIC_CH BRG: SCLKOSEL (Bit 28)                              */
#define USIC_CH_BRG_SCLKOSEL_Msk              (0x10000000UL)            /*!< USIC_CH BRG: SCLKOSEL (Bitfield-Mask: 0x01)                 */
#define USIC_CH_BRG_MCLKCFG_Pos               (29UL)                    /*!< USIC_CH BRG: MCLKCFG (Bit 29)                               */
#define USIC_CH_BRG_MCLKCFG_Msk               (0x20000000UL)            /*!< USIC_CH BRG: MCLKCFG (Bitfield-Mask: 0x01)                  */
#define USIC_CH_BRG_SCLKCFG_Pos               (30UL)                    /*!< USIC_CH BRG: SCLKCFG (Bit 30)                               */
#define USIC_CH_BRG_SCLKCFG_Msk               (0xc0000000UL)            /*!< USIC_CH BRG: SCLKCFG (Bitfield-Mask: 0x03)                  */

/* --------------------------------  USIC_CH_INPR  -------------------------------- */
#define USIC_CH_INPR_TSINP_Pos                (0UL)                     /*!< USIC_CH INPR: TSINP (Bit 0)                                 */
#define USIC_CH_INPR_TSINP_Msk                (0x7UL)                   /*!< USIC_CH INPR: TSINP (Bitfield-Mask: 0x07)                   */
#define USIC_CH_INPR_TBINP_Pos                (4UL)                     /*!< USIC_CH INPR: TBINP (Bit 4)                                 */
#define USIC_CH_INPR_TBINP_Msk                (0x70UL)                  /*!< USIC_CH INPR: TBINP (Bitfield-Mask: 0x07)                   */
#define USIC_CH_INPR_RINP_Pos                 (8UL)                     /*!< USIC_CH INPR: RINP (Bit 8)                                  */
#define USIC_CH_INPR_RINP_Msk                 (0x700UL)                 /*!< USIC_CH INPR: RINP (Bitfield-Mask: 0x07)                    */
#define USIC_CH_INPR_AINP_Pos                 (12UL)                    /*!< USIC_CH INPR: AINP (Bit 12)                                 */
#define USIC_CH_INPR_AINP_Msk                 (0x7000UL)                /*!< USIC_CH INPR: AINP (Bitfield-Mask: 0x07)                    */
#define USIC_CH_INPR_PINP_Pos                 (16UL)                    /*!< USIC_CH INPR: PINP (Bit 16)                                 */
#define USIC_CH_INPR_PINP_Msk                 (0x70000UL)               /*!< USIC_CH INPR: PINP (Bitfield-Mask: 0x07)                    */

/* --------------------------------  USIC_CH_DX0CR  ------------------------------- */
#define USIC_CH_DX0CR_DSEL_Pos                (0UL)                     /*!< USIC_CH DX0CR: DSEL (Bit 0)                                 */
#define USIC_CH_DX0CR_DSEL_Msk                (0x7UL)                   /*!< USIC_CH DX0CR: DSEL (Bitfield-Mask: 0x07)                   */
#define USIC_CH_DX0CR_INSW_Pos                (4UL)                     /*!< USIC_CH DX0CR: INSW (Bit 4)                                 */
#define USIC_CH_DX0CR_INSW_Msk                (0x10UL)                  /*!< USIC_CH DX0CR: INSW (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX0CR_DFEN_Pos                (5UL)                     /*!< USIC_CH DX0CR: DFEN (Bit 5)                                 */
#define USIC_CH_DX0CR_DFEN_Msk                (0x20UL)                  /*!< USIC_CH DX0CR: DFEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX0CR_DSEN_Pos                (6UL)                     /*!< USIC_CH DX0CR: DSEN (Bit 6)                                 */
#define USIC_CH_DX0CR_DSEN_Msk                (0x40UL)                  /*!< USIC_CH DX0CR: DSEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX0CR_DPOL_Pos                (8UL)                     /*!< USIC_CH DX0CR: DPOL (Bit 8)                                 */
#define USIC_CH_DX0CR_DPOL_Msk                (0x100UL)                 /*!< USIC_CH DX0CR: DPOL (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX0CR_SFSEL_Pos               (9UL)                     /*!< USIC_CH DX0CR: SFSEL (Bit 9)                                */
#define USIC_CH_DX0CR_SFSEL_Msk               (0x200UL)                 /*!< USIC_CH DX0CR: SFSEL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_DX0CR_CM_Pos                  (10UL)                    /*!< USIC_CH DX0CR: CM (Bit 10)                                  */
#define USIC_CH_DX0CR_CM_Msk                  (0xc00UL)                 /*!< USIC_CH DX0CR: CM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_DX0CR_DXS_Pos                 (15UL)                    /*!< USIC_CH DX0CR: DXS (Bit 15)                                 */
#define USIC_CH_DX0CR_DXS_Msk                 (0x8000UL)                /*!< USIC_CH DX0CR: DXS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_DX1CR  ------------------------------- */
#define USIC_CH_DX1CR_DSEL_Pos                (0UL)                     /*!< USIC_CH DX1CR: DSEL (Bit 0)                                 */
#define USIC_CH_DX1CR_DSEL_Msk                (0x7UL)                   /*!< USIC_CH DX1CR: DSEL (Bitfield-Mask: 0x07)                   */
#define USIC_CH_DX1CR_DCEN_Pos                (3UL)                     /*!< USIC_CH DX1CR: DCEN (Bit 3)                                 */
#define USIC_CH_DX1CR_DCEN_Msk                (0x8UL)                   /*!< USIC_CH DX1CR: DCEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX1CR_INSW_Pos                (4UL)                     /*!< USIC_CH DX1CR: INSW (Bit 4)                                 */
#define USIC_CH_DX1CR_INSW_Msk                (0x10UL)                  /*!< USIC_CH DX1CR: INSW (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX1CR_DFEN_Pos                (5UL)                     /*!< USIC_CH DX1CR: DFEN (Bit 5)                                 */
#define USIC_CH_DX1CR_DFEN_Msk                (0x20UL)                  /*!< USIC_CH DX1CR: DFEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX1CR_DSEN_Pos                (6UL)                     /*!< USIC_CH DX1CR: DSEN (Bit 6)                                 */
#define USIC_CH_DX1CR_DSEN_Msk                (0x40UL)                  /*!< USIC_CH DX1CR: DSEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX1CR_DPOL_Pos                (8UL)                     /*!< USIC_CH DX1CR: DPOL (Bit 8)                                 */
#define USIC_CH_DX1CR_DPOL_Msk                (0x100UL)                 /*!< USIC_CH DX1CR: DPOL (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX1CR_SFSEL_Pos               (9UL)                     /*!< USIC_CH DX1CR: SFSEL (Bit 9)                                */
#define USIC_CH_DX1CR_SFSEL_Msk               (0x200UL)                 /*!< USIC_CH DX1CR: SFSEL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_DX1CR_CM_Pos                  (10UL)                    /*!< USIC_CH DX1CR: CM (Bit 10)                                  */
#define USIC_CH_DX1CR_CM_Msk                  (0xc00UL)                 /*!< USIC_CH DX1CR: CM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_DX1CR_DXS_Pos                 (15UL)                    /*!< USIC_CH DX1CR: DXS (Bit 15)                                 */
#define USIC_CH_DX1CR_DXS_Msk                 (0x8000UL)                /*!< USIC_CH DX1CR: DXS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_DX2CR  ------------------------------- */
#define USIC_CH_DX2CR_DSEL_Pos                (0UL)                     /*!< USIC_CH DX2CR: DSEL (Bit 0)                                 */
#define USIC_CH_DX2CR_DSEL_Msk                (0x7UL)                   /*!< USIC_CH DX2CR: DSEL (Bitfield-Mask: 0x07)                   */
#define USIC_CH_DX2CR_INSW_Pos                (4UL)                     /*!< USIC_CH DX2CR: INSW (Bit 4)                                 */
#define USIC_CH_DX2CR_INSW_Msk                (0x10UL)                  /*!< USIC_CH DX2CR: INSW (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX2CR_DFEN_Pos                (5UL)                     /*!< USIC_CH DX2CR: DFEN (Bit 5)                                 */
#define USIC_CH_DX2CR_DFEN_Msk                (0x20UL)                  /*!< USIC_CH DX2CR: DFEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX2CR_DSEN_Pos                (6UL)                     /*!< USIC_CH DX2CR: DSEN (Bit 6)                                 */
#define USIC_CH_DX2CR_DSEN_Msk                (0x40UL)                  /*!< USIC_CH DX2CR: DSEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX2CR_DPOL_Pos                (8UL)                     /*!< USIC_CH DX2CR: DPOL (Bit 8)                                 */
#define USIC_CH_DX2CR_DPOL_Msk                (0x100UL)                 /*!< USIC_CH DX2CR: DPOL (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX2CR_SFSEL_Pos               (9UL)                     /*!< USIC_CH DX2CR: SFSEL (Bit 9)                                */
#define USIC_CH_DX2CR_SFSEL_Msk               (0x200UL)                 /*!< USIC_CH DX2CR: SFSEL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_DX2CR_CM_Pos                  (10UL)                    /*!< USIC_CH DX2CR: CM (Bit 10)                                  */
#define USIC_CH_DX2CR_CM_Msk                  (0xc00UL)                 /*!< USIC_CH DX2CR: CM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_DX2CR_DXS_Pos                 (15UL)                    /*!< USIC_CH DX2CR: DXS (Bit 15)                                 */
#define USIC_CH_DX2CR_DXS_Msk                 (0x8000UL)                /*!< USIC_CH DX2CR: DXS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_DX3CR  ------------------------------- */
#define USIC_CH_DX3CR_DSEL_Pos                (0UL)                     /*!< USIC_CH DX3CR: DSEL (Bit 0)                                 */
#define USIC_CH_DX3CR_DSEL_Msk                (0x7UL)                   /*!< USIC_CH DX3CR: DSEL (Bitfield-Mask: 0x07)                   */
#define USIC_CH_DX3CR_INSW_Pos                (4UL)                     /*!< USIC_CH DX3CR: INSW (Bit 4)                                 */
#define USIC_CH_DX3CR_INSW_Msk                (0x10UL)                  /*!< USIC_CH DX3CR: INSW (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX3CR_DFEN_Pos                (5UL)                     /*!< USIC_CH DX3CR: DFEN (Bit 5)                                 */
#define USIC_CH_DX3CR_DFEN_Msk                (0x20UL)                  /*!< USIC_CH DX3CR: DFEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX3CR_DSEN_Pos                (6UL)                     /*!< USIC_CH DX3CR: DSEN (Bit 6)                                 */
#define USIC_CH_DX3CR_DSEN_Msk                (0x40UL)                  /*!< USIC_CH DX3CR: DSEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX3CR_DPOL_Pos                (8UL)                     /*!< USIC_CH DX3CR: DPOL (Bit 8)                                 */
#define USIC_CH_DX3CR_DPOL_Msk                (0x100UL)                 /*!< USIC_CH DX3CR: DPOL (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX3CR_SFSEL_Pos               (9UL)                     /*!< USIC_CH DX3CR: SFSEL (Bit 9)                                */
#define USIC_CH_DX3CR_SFSEL_Msk               (0x200UL)                 /*!< USIC_CH DX3CR: SFSEL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_DX3CR_CM_Pos                  (10UL)                    /*!< USIC_CH DX3CR: CM (Bit 10)                                  */
#define USIC_CH_DX3CR_CM_Msk                  (0xc00UL)                 /*!< USIC_CH DX3CR: CM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_DX3CR_DXS_Pos                 (15UL)                    /*!< USIC_CH DX3CR: DXS (Bit 15)                                 */
#define USIC_CH_DX3CR_DXS_Msk                 (0x8000UL)                /*!< USIC_CH DX3CR: DXS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_DX4CR  ------------------------------- */
#define USIC_CH_DX4CR_DSEL_Pos                (0UL)                     /*!< USIC_CH DX4CR: DSEL (Bit 0)                                 */
#define USIC_CH_DX4CR_DSEL_Msk                (0x7UL)                   /*!< USIC_CH DX4CR: DSEL (Bitfield-Mask: 0x07)                   */
#define USIC_CH_DX4CR_INSW_Pos                (4UL)                     /*!< USIC_CH DX4CR: INSW (Bit 4)                                 */
#define USIC_CH_DX4CR_INSW_Msk                (0x10UL)                  /*!< USIC_CH DX4CR: INSW (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX4CR_DFEN_Pos                (5UL)                     /*!< USIC_CH DX4CR: DFEN (Bit 5)                                 */
#define USIC_CH_DX4CR_DFEN_Msk                (0x20UL)                  /*!< USIC_CH DX4CR: DFEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX4CR_DSEN_Pos                (6UL)                     /*!< USIC_CH DX4CR: DSEN (Bit 6)                                 */
#define USIC_CH_DX4CR_DSEN_Msk                (0x40UL)                  /*!< USIC_CH DX4CR: DSEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX4CR_DPOL_Pos                (8UL)                     /*!< USIC_CH DX4CR: DPOL (Bit 8)                                 */
#define USIC_CH_DX4CR_DPOL_Msk                (0x100UL)                 /*!< USIC_CH DX4CR: DPOL (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX4CR_SFSEL_Pos               (9UL)                     /*!< USIC_CH DX4CR: SFSEL (Bit 9)                                */
#define USIC_CH_DX4CR_SFSEL_Msk               (0x200UL)                 /*!< USIC_CH DX4CR: SFSEL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_DX4CR_CM_Pos                  (10UL)                    /*!< USIC_CH DX4CR: CM (Bit 10)                                  */
#define USIC_CH_DX4CR_CM_Msk                  (0xc00UL)                 /*!< USIC_CH DX4CR: CM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_DX4CR_DXS_Pos                 (15UL)                    /*!< USIC_CH DX4CR: DXS (Bit 15)                                 */
#define USIC_CH_DX4CR_DXS_Msk                 (0x8000UL)                /*!< USIC_CH DX4CR: DXS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_DX5CR  ------------------------------- */
#define USIC_CH_DX5CR_DSEL_Pos                (0UL)                     /*!< USIC_CH DX5CR: DSEL (Bit 0)                                 */
#define USIC_CH_DX5CR_DSEL_Msk                (0x7UL)                   /*!< USIC_CH DX5CR: DSEL (Bitfield-Mask: 0x07)                   */
#define USIC_CH_DX5CR_INSW_Pos                (4UL)                     /*!< USIC_CH DX5CR: INSW (Bit 4)                                 */
#define USIC_CH_DX5CR_INSW_Msk                (0x10UL)                  /*!< USIC_CH DX5CR: INSW (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX5CR_DFEN_Pos                (5UL)                     /*!< USIC_CH DX5CR: DFEN (Bit 5)                                 */
#define USIC_CH_DX5CR_DFEN_Msk                (0x20UL)                  /*!< USIC_CH DX5CR: DFEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX5CR_DSEN_Pos                (6UL)                     /*!< USIC_CH DX5CR: DSEN (Bit 6)                                 */
#define USIC_CH_DX5CR_DSEN_Msk                (0x40UL)                  /*!< USIC_CH DX5CR: DSEN (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX5CR_DPOL_Pos                (8UL)                     /*!< USIC_CH DX5CR: DPOL (Bit 8)                                 */
#define USIC_CH_DX5CR_DPOL_Msk                (0x100UL)                 /*!< USIC_CH DX5CR: DPOL (Bitfield-Mask: 0x01)                   */
#define USIC_CH_DX5CR_SFSEL_Pos               (9UL)                     /*!< USIC_CH DX5CR: SFSEL (Bit 9)                                */
#define USIC_CH_DX5CR_SFSEL_Msk               (0x200UL)                 /*!< USIC_CH DX5CR: SFSEL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_DX5CR_CM_Pos                  (10UL)                    /*!< USIC_CH DX5CR: CM (Bit 10)                                  */
#define USIC_CH_DX5CR_CM_Msk                  (0xc00UL)                 /*!< USIC_CH DX5CR: CM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_DX5CR_DXS_Pos                 (15UL)                    /*!< USIC_CH DX5CR: DXS (Bit 15)                                 */
#define USIC_CH_DX5CR_DXS_Msk                 (0x8000UL)                /*!< USIC_CH DX5CR: DXS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_SCTR  -------------------------------- */
#define USIC_CH_SCTR_SDIR_Pos                 (0UL)                     /*!< USIC_CH SCTR: SDIR (Bit 0)                                  */
#define USIC_CH_SCTR_SDIR_Msk                 (0x1UL)                   /*!< USIC_CH SCTR: SDIR (Bitfield-Mask: 0x01)                    */
#define USIC_CH_SCTR_PDL_Pos                  (1UL)                     /*!< USIC_CH SCTR: PDL (Bit 1)                                   */
#define USIC_CH_SCTR_PDL_Msk                  (0x2UL)                   /*!< USIC_CH SCTR: PDL (Bitfield-Mask: 0x01)                     */
#define USIC_CH_SCTR_DSM_Pos                  (2UL)                     /*!< USIC_CH SCTR: DSM (Bit 2)                                   */
#define USIC_CH_SCTR_DSM_Msk                  (0xcUL)                   /*!< USIC_CH SCTR: DSM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_SCTR_HPCDIR_Pos               (4UL)                     /*!< USIC_CH SCTR: HPCDIR (Bit 4)                                */
#define USIC_CH_SCTR_HPCDIR_Msk               (0x10UL)                  /*!< USIC_CH SCTR: HPCDIR (Bitfield-Mask: 0x01)                  */
#define USIC_CH_SCTR_DOCFG_Pos                (6UL)                     /*!< USIC_CH SCTR: DOCFG (Bit 6)                                 */
#define USIC_CH_SCTR_DOCFG_Msk                (0xc0UL)                  /*!< USIC_CH SCTR: DOCFG (Bitfield-Mask: 0x03)                   */
#define USIC_CH_SCTR_TRM_Pos                  (8UL)                     /*!< USIC_CH SCTR: TRM (Bit 8)                                   */
#define USIC_CH_SCTR_TRM_Msk                  (0x300UL)                 /*!< USIC_CH SCTR: TRM (Bitfield-Mask: 0x03)                     */
#define USIC_CH_SCTR_FLE_Pos                  (16UL)                    /*!< USIC_CH SCTR: FLE (Bit 16)                                  */
#define USIC_CH_SCTR_FLE_Msk                  (0x3f0000UL)              /*!< USIC_CH SCTR: FLE (Bitfield-Mask: 0x3f)                     */
#define USIC_CH_SCTR_WLE_Pos                  (24UL)                    /*!< USIC_CH SCTR: WLE (Bit 24)                                  */
#define USIC_CH_SCTR_WLE_Msk                  (0xf000000UL)             /*!< USIC_CH SCTR: WLE (Bitfield-Mask: 0x0f)                     */

/* --------------------------------  USIC_CH_TCSR  -------------------------------- */
#define USIC_CH_TCSR_WLEMD_Pos                (0UL)                     /*!< USIC_CH TCSR: WLEMD (Bit 0)                                 */
#define USIC_CH_TCSR_WLEMD_Msk                (0x1UL)                   /*!< USIC_CH TCSR: WLEMD (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TCSR_SELMD_Pos                (1UL)                     /*!< USIC_CH TCSR: SELMD (Bit 1)                                 */
#define USIC_CH_TCSR_SELMD_Msk                (0x2UL)                   /*!< USIC_CH TCSR: SELMD (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TCSR_FLEMD_Pos                (2UL)                     /*!< USIC_CH TCSR: FLEMD (Bit 2)                                 */
#define USIC_CH_TCSR_FLEMD_Msk                (0x4UL)                   /*!< USIC_CH TCSR: FLEMD (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TCSR_WAMD_Pos                 (3UL)                     /*!< USIC_CH TCSR: WAMD (Bit 3)                                  */
#define USIC_CH_TCSR_WAMD_Msk                 (0x8UL)                   /*!< USIC_CH TCSR: WAMD (Bitfield-Mask: 0x01)                    */
#define USIC_CH_TCSR_HPCMD_Pos                (4UL)                     /*!< USIC_CH TCSR: HPCMD (Bit 4)                                 */
#define USIC_CH_TCSR_HPCMD_Msk                (0x10UL)                  /*!< USIC_CH TCSR: HPCMD (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TCSR_SOF_Pos                  (5UL)                     /*!< USIC_CH TCSR: SOF (Bit 5)                                   */
#define USIC_CH_TCSR_SOF_Msk                  (0x20UL)                  /*!< USIC_CH TCSR: SOF (Bitfield-Mask: 0x01)                     */
#define USIC_CH_TCSR_EOF_Pos                  (6UL)                     /*!< USIC_CH TCSR: EOF (Bit 6)                                   */
#define USIC_CH_TCSR_EOF_Msk                  (0x40UL)                  /*!< USIC_CH TCSR: EOF (Bitfield-Mask: 0x01)                     */
#define USIC_CH_TCSR_TDV_Pos                  (7UL)                     /*!< USIC_CH TCSR: TDV (Bit 7)                                   */
#define USIC_CH_TCSR_TDV_Msk                  (0x80UL)                  /*!< USIC_CH TCSR: TDV (Bitfield-Mask: 0x01)                     */
#define USIC_CH_TCSR_TDSSM_Pos                (8UL)                     /*!< USIC_CH TCSR: TDSSM (Bit 8)                                 */
#define USIC_CH_TCSR_TDSSM_Msk                (0x100UL)                 /*!< USIC_CH TCSR: TDSSM (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TCSR_TDEN_Pos                 (10UL)                    /*!< USIC_CH TCSR: TDEN (Bit 10)                                 */
#define USIC_CH_TCSR_TDEN_Msk                 (0xc00UL)                 /*!< USIC_CH TCSR: TDEN (Bitfield-Mask: 0x03)                    */
#define USIC_CH_TCSR_TDVTR_Pos                (12UL)                    /*!< USIC_CH TCSR: TDVTR (Bit 12)                                */
#define USIC_CH_TCSR_TDVTR_Msk                (0x1000UL)                /*!< USIC_CH TCSR: TDVTR (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TCSR_WA_Pos                   (13UL)                    /*!< USIC_CH TCSR: WA (Bit 13)                                   */
#define USIC_CH_TCSR_WA_Msk                   (0x2000UL)                /*!< USIC_CH TCSR: WA (Bitfield-Mask: 0x01)                      */
#define USIC_CH_TCSR_TSOF_Pos                 (24UL)                    /*!< USIC_CH TCSR: TSOF (Bit 24)                                 */
#define USIC_CH_TCSR_TSOF_Msk                 (0x1000000UL)             /*!< USIC_CH TCSR: TSOF (Bitfield-Mask: 0x01)                    */
#define USIC_CH_TCSR_TV_Pos                   (26UL)                    /*!< USIC_CH TCSR: TV (Bit 26)                                   */
#define USIC_CH_TCSR_TV_Msk                   (0x4000000UL)             /*!< USIC_CH TCSR: TV (Bitfield-Mask: 0x01)                      */
#define USIC_CH_TCSR_TVC_Pos                  (27UL)                    /*!< USIC_CH TCSR: TVC (Bit 27)                                  */
#define USIC_CH_TCSR_TVC_Msk                  (0x8000000UL)             /*!< USIC_CH TCSR: TVC (Bitfield-Mask: 0x01)                     */
#define USIC_CH_TCSR_TE_Pos                   (28UL)                    /*!< USIC_CH TCSR: TE (Bit 28)                                   */
#define USIC_CH_TCSR_TE_Msk                   (0x10000000UL)            /*!< USIC_CH TCSR: TE (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  USIC_CH_PCR  -------------------------------- */
#define USIC_CH_PCR_CTR0_Pos                  (0UL)                     /*!< USIC_CH PCR: CTR0 (Bit 0)                                   */
#define USIC_CH_PCR_CTR0_Msk                  (0x1UL)                   /*!< USIC_CH PCR: CTR0 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR1_Pos                  (1UL)                     /*!< USIC_CH PCR: CTR1 (Bit 1)                                   */
#define USIC_CH_PCR_CTR1_Msk                  (0x2UL)                   /*!< USIC_CH PCR: CTR1 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR2_Pos                  (2UL)                     /*!< USIC_CH PCR: CTR2 (Bit 2)                                   */
#define USIC_CH_PCR_CTR2_Msk                  (0x4UL)                   /*!< USIC_CH PCR: CTR2 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR3_Pos                  (3UL)                     /*!< USIC_CH PCR: CTR3 (Bit 3)                                   */
#define USIC_CH_PCR_CTR3_Msk                  (0x8UL)                   /*!< USIC_CH PCR: CTR3 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR4_Pos                  (4UL)                     /*!< USIC_CH PCR: CTR4 (Bit 4)                                   */
#define USIC_CH_PCR_CTR4_Msk                  (0x10UL)                  /*!< USIC_CH PCR: CTR4 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR5_Pos                  (5UL)                     /*!< USIC_CH PCR: CTR5 (Bit 5)                                   */
#define USIC_CH_PCR_CTR5_Msk                  (0x20UL)                  /*!< USIC_CH PCR: CTR5 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR6_Pos                  (6UL)                     /*!< USIC_CH PCR: CTR6 (Bit 6)                                   */
#define USIC_CH_PCR_CTR6_Msk                  (0x40UL)                  /*!< USIC_CH PCR: CTR6 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR7_Pos                  (7UL)                     /*!< USIC_CH PCR: CTR7 (Bit 7)                                   */
#define USIC_CH_PCR_CTR7_Msk                  (0x80UL)                  /*!< USIC_CH PCR: CTR7 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR8_Pos                  (8UL)                     /*!< USIC_CH PCR: CTR8 (Bit 8)                                   */
#define USIC_CH_PCR_CTR8_Msk                  (0x100UL)                 /*!< USIC_CH PCR: CTR8 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR9_Pos                  (9UL)                     /*!< USIC_CH PCR: CTR9 (Bit 9)                                   */
#define USIC_CH_PCR_CTR9_Msk                  (0x200UL)                 /*!< USIC_CH PCR: CTR9 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PCR_CTR10_Pos                 (10UL)                    /*!< USIC_CH PCR: CTR10 (Bit 10)                                 */
#define USIC_CH_PCR_CTR10_Msk                 (0x400UL)                 /*!< USIC_CH PCR: CTR10 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR11_Pos                 (11UL)                    /*!< USIC_CH PCR: CTR11 (Bit 11)                                 */
#define USIC_CH_PCR_CTR11_Msk                 (0x800UL)                 /*!< USIC_CH PCR: CTR11 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR12_Pos                 (12UL)                    /*!< USIC_CH PCR: CTR12 (Bit 12)                                 */
#define USIC_CH_PCR_CTR12_Msk                 (0x1000UL)                /*!< USIC_CH PCR: CTR12 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR13_Pos                 (13UL)                    /*!< USIC_CH PCR: CTR13 (Bit 13)                                 */
#define USIC_CH_PCR_CTR13_Msk                 (0x2000UL)                /*!< USIC_CH PCR: CTR13 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR14_Pos                 (14UL)                    /*!< USIC_CH PCR: CTR14 (Bit 14)                                 */
#define USIC_CH_PCR_CTR14_Msk                 (0x4000UL)                /*!< USIC_CH PCR: CTR14 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR15_Pos                 (15UL)                    /*!< USIC_CH PCR: CTR15 (Bit 15)                                 */
#define USIC_CH_PCR_CTR15_Msk                 (0x8000UL)                /*!< USIC_CH PCR: CTR15 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR16_Pos                 (16UL)                    /*!< USIC_CH PCR: CTR16 (Bit 16)                                 */
#define USIC_CH_PCR_CTR16_Msk                 (0x10000UL)               /*!< USIC_CH PCR: CTR16 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR17_Pos                 (17UL)                    /*!< USIC_CH PCR: CTR17 (Bit 17)                                 */
#define USIC_CH_PCR_CTR17_Msk                 (0x20000UL)               /*!< USIC_CH PCR: CTR17 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR18_Pos                 (18UL)                    /*!< USIC_CH PCR: CTR18 (Bit 18)                                 */
#define USIC_CH_PCR_CTR18_Msk                 (0x40000UL)               /*!< USIC_CH PCR: CTR18 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR19_Pos                 (19UL)                    /*!< USIC_CH PCR: CTR19 (Bit 19)                                 */
#define USIC_CH_PCR_CTR19_Msk                 (0x80000UL)               /*!< USIC_CH PCR: CTR19 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR20_Pos                 (20UL)                    /*!< USIC_CH PCR: CTR20 (Bit 20)                                 */
#define USIC_CH_PCR_CTR20_Msk                 (0x100000UL)              /*!< USIC_CH PCR: CTR20 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR21_Pos                 (21UL)                    /*!< USIC_CH PCR: CTR21 (Bit 21)                                 */
#define USIC_CH_PCR_CTR21_Msk                 (0x200000UL)              /*!< USIC_CH PCR: CTR21 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR22_Pos                 (22UL)                    /*!< USIC_CH PCR: CTR22 (Bit 22)                                 */
#define USIC_CH_PCR_CTR22_Msk                 (0x400000UL)              /*!< USIC_CH PCR: CTR22 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR23_Pos                 (23UL)                    /*!< USIC_CH PCR: CTR23 (Bit 23)                                 */
#define USIC_CH_PCR_CTR23_Msk                 (0x800000UL)              /*!< USIC_CH PCR: CTR23 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR24_Pos                 (24UL)                    /*!< USIC_CH PCR: CTR24 (Bit 24)                                 */
#define USIC_CH_PCR_CTR24_Msk                 (0x1000000UL)             /*!< USIC_CH PCR: CTR24 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR25_Pos                 (25UL)                    /*!< USIC_CH PCR: CTR25 (Bit 25)                                 */
#define USIC_CH_PCR_CTR25_Msk                 (0x2000000UL)             /*!< USIC_CH PCR: CTR25 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR26_Pos                 (26UL)                    /*!< USIC_CH PCR: CTR26 (Bit 26)                                 */
#define USIC_CH_PCR_CTR26_Msk                 (0x4000000UL)             /*!< USIC_CH PCR: CTR26 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR27_Pos                 (27UL)                    /*!< USIC_CH PCR: CTR27 (Bit 27)                                 */
#define USIC_CH_PCR_CTR27_Msk                 (0x8000000UL)             /*!< USIC_CH PCR: CTR27 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR28_Pos                 (28UL)                    /*!< USIC_CH PCR: CTR28 (Bit 28)                                 */
#define USIC_CH_PCR_CTR28_Msk                 (0x10000000UL)            /*!< USIC_CH PCR: CTR28 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR29_Pos                 (29UL)                    /*!< USIC_CH PCR: CTR29 (Bit 29)                                 */
#define USIC_CH_PCR_CTR29_Msk                 (0x20000000UL)            /*!< USIC_CH PCR: CTR29 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR30_Pos                 (30UL)                    /*!< USIC_CH PCR: CTR30 (Bit 30)                                 */
#define USIC_CH_PCR_CTR30_Msk                 (0x40000000UL)            /*!< USIC_CH PCR: CTR30 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PCR_CTR31_Pos                 (31UL)                    /*!< USIC_CH PCR: CTR31 (Bit 31)                                 */
#define USIC_CH_PCR_CTR31_Msk                 (0x80000000UL)            /*!< USIC_CH PCR: CTR31 (Bitfield-Mask: 0x01)                    */

/* -----------------------------  USIC_CH_PCR_ASCMode  ---------------------------- */
#define USIC_CH_PCR_ASCMode_SMD_Pos           (0UL)                     /*!< USIC_CH PCR_ASCMode: SMD (Bit 0)                            */
#define USIC_CH_PCR_ASCMode_SMD_Msk           (0x1UL)                   /*!< USIC_CH PCR_ASCMode: SMD (Bitfield-Mask: 0x01)              */
#define USIC_CH_PCR_ASCMode_STPB_Pos          (1UL)                     /*!< USIC_CH PCR_ASCMode: STPB (Bit 1)                           */
#define USIC_CH_PCR_ASCMode_STPB_Msk          (0x2UL)                   /*!< USIC_CH PCR_ASCMode: STPB (Bitfield-Mask: 0x01)             */
#define USIC_CH_PCR_ASCMode_IDM_Pos           (2UL)                     /*!< USIC_CH PCR_ASCMode: IDM (Bit 2)                            */
#define USIC_CH_PCR_ASCMode_IDM_Msk           (0x4UL)                   /*!< USIC_CH PCR_ASCMode: IDM (Bitfield-Mask: 0x01)              */
#define USIC_CH_PCR_ASCMode_SBIEN_Pos         (3UL)                     /*!< USIC_CH PCR_ASCMode: SBIEN (Bit 3)                          */
#define USIC_CH_PCR_ASCMode_SBIEN_Msk         (0x8UL)                   /*!< USIC_CH PCR_ASCMode: SBIEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_ASCMode_CDEN_Pos          (4UL)                     /*!< USIC_CH PCR_ASCMode: CDEN (Bit 4)                           */
#define USIC_CH_PCR_ASCMode_CDEN_Msk          (0x10UL)                  /*!< USIC_CH PCR_ASCMode: CDEN (Bitfield-Mask: 0x01)             */
#define USIC_CH_PCR_ASCMode_RNIEN_Pos         (5UL)                     /*!< USIC_CH PCR_ASCMode: RNIEN (Bit 5)                          */
#define USIC_CH_PCR_ASCMode_RNIEN_Msk         (0x20UL)                  /*!< USIC_CH PCR_ASCMode: RNIEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_ASCMode_FEIEN_Pos         (6UL)                     /*!< USIC_CH PCR_ASCMode: FEIEN (Bit 6)                          */
#define USIC_CH_PCR_ASCMode_FEIEN_Msk         (0x40UL)                  /*!< USIC_CH PCR_ASCMode: FEIEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_ASCMode_FFIEN_Pos         (7UL)                     /*!< USIC_CH PCR_ASCMode: FFIEN (Bit 7)                          */
#define USIC_CH_PCR_ASCMode_FFIEN_Msk         (0x80UL)                  /*!< USIC_CH PCR_ASCMode: FFIEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_ASCMode_SP_Pos            (8UL)                     /*!< USIC_CH PCR_ASCMode: SP (Bit 8)                             */
#define USIC_CH_PCR_ASCMode_SP_Msk            (0x1f00UL)                /*!< USIC_CH PCR_ASCMode: SP (Bitfield-Mask: 0x1f)               */
#define USIC_CH_PCR_ASCMode_PL_Pos            (13UL)                    /*!< USIC_CH PCR_ASCMode: PL (Bit 13)                            */
#define USIC_CH_PCR_ASCMode_PL_Msk            (0xe000UL)                /*!< USIC_CH PCR_ASCMode: PL (Bitfield-Mask: 0x07)               */
#define USIC_CH_PCR_ASCMode_RSTEN_Pos         (16UL)                    /*!< USIC_CH PCR_ASCMode: RSTEN (Bit 16)                         */
#define USIC_CH_PCR_ASCMode_RSTEN_Msk         (0x10000UL)               /*!< USIC_CH PCR_ASCMode: RSTEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_ASCMode_TSTEN_Pos         (17UL)                    /*!< USIC_CH PCR_ASCMode: TSTEN (Bit 17)                         */
#define USIC_CH_PCR_ASCMode_TSTEN_Msk         (0x20000UL)               /*!< USIC_CH PCR_ASCMode: TSTEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_ASCMode_MCLK_Pos          (31UL)                    /*!< USIC_CH PCR_ASCMode: MCLK (Bit 31)                          */
#define USIC_CH_PCR_ASCMode_MCLK_Msk          (0x80000000UL)            /*!< USIC_CH PCR_ASCMode: MCLK (Bitfield-Mask: 0x01)             */

/* -----------------------------  USIC_CH_PCR_SSCMode  ---------------------------- */
#define USIC_CH_PCR_SSCMode_MSLSEN_Pos        (0UL)                     /*!< USIC_CH PCR_SSCMode: MSLSEN (Bit 0)                         */
#define USIC_CH_PCR_SSCMode_MSLSEN_Msk        (0x1UL)                   /*!< USIC_CH PCR_SSCMode: MSLSEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_SSCMode_SELCTR_Pos        (1UL)                     /*!< USIC_CH PCR_SSCMode: SELCTR (Bit 1)                         */
#define USIC_CH_PCR_SSCMode_SELCTR_Msk        (0x2UL)                   /*!< USIC_CH PCR_SSCMode: SELCTR (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_SSCMode_SELINV_Pos        (2UL)                     /*!< USIC_CH PCR_SSCMode: SELINV (Bit 2)                         */
#define USIC_CH_PCR_SSCMode_SELINV_Msk        (0x4UL)                   /*!< USIC_CH PCR_SSCMode: SELINV (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_SSCMode_FEM_Pos           (3UL)                     /*!< USIC_CH PCR_SSCMode: FEM (Bit 3)                            */
#define USIC_CH_PCR_SSCMode_FEM_Msk           (0x8UL)                   /*!< USIC_CH PCR_SSCMode: FEM (Bitfield-Mask: 0x01)              */
#define USIC_CH_PCR_SSCMode_CTQSEL1_Pos       (4UL)                     /*!< USIC_CH PCR_SSCMode: CTQSEL1 (Bit 4)                        */
#define USIC_CH_PCR_SSCMode_CTQSEL1_Msk       (0x30UL)                  /*!< USIC_CH PCR_SSCMode: CTQSEL1 (Bitfield-Mask: 0x03)          */
#define USIC_CH_PCR_SSCMode_PCTQ1_Pos         (6UL)                     /*!< USIC_CH PCR_SSCMode: PCTQ1 (Bit 6)                          */
#define USIC_CH_PCR_SSCMode_PCTQ1_Msk         (0xc0UL)                  /*!< USIC_CH PCR_SSCMode: PCTQ1 (Bitfield-Mask: 0x03)            */
#define USIC_CH_PCR_SSCMode_DCTQ1_Pos         (8UL)                     /*!< USIC_CH PCR_SSCMode: DCTQ1 (Bit 8)                          */
#define USIC_CH_PCR_SSCMode_DCTQ1_Msk         (0x1f00UL)                /*!< USIC_CH PCR_SSCMode: DCTQ1 (Bitfield-Mask: 0x1f)            */
#define USIC_CH_PCR_SSCMode_PARIEN_Pos        (13UL)                    /*!< USIC_CH PCR_SSCMode: PARIEN (Bit 13)                        */
#define USIC_CH_PCR_SSCMode_PARIEN_Msk        (0x2000UL)                /*!< USIC_CH PCR_SSCMode: PARIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_SSCMode_MSLSIEN_Pos       (14UL)                    /*!< USIC_CH PCR_SSCMode: MSLSIEN (Bit 14)                       */
#define USIC_CH_PCR_SSCMode_MSLSIEN_Msk       (0x4000UL)                /*!< USIC_CH PCR_SSCMode: MSLSIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_SSCMode_DX2TIEN_Pos       (15UL)                    /*!< USIC_CH PCR_SSCMode: DX2TIEN (Bit 15)                       */
#define USIC_CH_PCR_SSCMode_DX2TIEN_Msk       (0x8000UL)                /*!< USIC_CH PCR_SSCMode: DX2TIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_SSCMode_SELO_Pos          (16UL)                    /*!< USIC_CH PCR_SSCMode: SELO (Bit 16)                          */
#define USIC_CH_PCR_SSCMode_SELO_Msk          (0xff0000UL)              /*!< USIC_CH PCR_SSCMode: SELO (Bitfield-Mask: 0xff)             */
#define USIC_CH_PCR_SSCMode_TIWEN_Pos         (24UL)                    /*!< USIC_CH PCR_SSCMode: TIWEN (Bit 24)                         */
#define USIC_CH_PCR_SSCMode_TIWEN_Msk         (0x1000000UL)             /*!< USIC_CH PCR_SSCMode: TIWEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_SSCMode_SLPHSEL_Pos       (25UL)                    /*!< USIC_CH PCR_SSCMode: SLPHSEL (Bit 25)                       */
#define USIC_CH_PCR_SSCMode_SLPHSEL_Msk       (0x2000000UL)             /*!< USIC_CH PCR_SSCMode: SLPHSEL (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_SSCMode_MCLK_Pos          (31UL)                    /*!< USIC_CH PCR_SSCMode: MCLK (Bit 31)                          */
#define USIC_CH_PCR_SSCMode_MCLK_Msk          (0x80000000UL)            /*!< USIC_CH PCR_SSCMode: MCLK (Bitfield-Mask: 0x01)             */

/* -----------------------------  USIC_CH_PCR_IICMode  ---------------------------- */
#define USIC_CH_PCR_IICMode_SLAD_Pos          (0UL)                     /*!< USIC_CH PCR_IICMode: SLAD (Bit 0)                           */
#define USIC_CH_PCR_IICMode_SLAD_Msk          (0xffffUL)                /*!< USIC_CH PCR_IICMode: SLAD (Bitfield-Mask: 0xffff)           */
#define USIC_CH_PCR_IICMode_ACK00_Pos         (16UL)                    /*!< USIC_CH PCR_IICMode: ACK00 (Bit 16)                         */
#define USIC_CH_PCR_IICMode_ACK00_Msk         (0x10000UL)               /*!< USIC_CH PCR_IICMode: ACK00 (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_IICMode_STIM_Pos          (17UL)                    /*!< USIC_CH PCR_IICMode: STIM (Bit 17)                          */
#define USIC_CH_PCR_IICMode_STIM_Msk          (0x20000UL)               /*!< USIC_CH PCR_IICMode: STIM (Bitfield-Mask: 0x01)             */
#define USIC_CH_PCR_IICMode_SCRIEN_Pos        (18UL)                    /*!< USIC_CH PCR_IICMode: SCRIEN (Bit 18)                        */
#define USIC_CH_PCR_IICMode_SCRIEN_Msk        (0x40000UL)               /*!< USIC_CH PCR_IICMode: SCRIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IICMode_RSCRIEN_Pos       (19UL)                    /*!< USIC_CH PCR_IICMode: RSCRIEN (Bit 19)                       */
#define USIC_CH_PCR_IICMode_RSCRIEN_Msk       (0x80000UL)               /*!< USIC_CH PCR_IICMode: RSCRIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_IICMode_PCRIEN_Pos        (20UL)                    /*!< USIC_CH PCR_IICMode: PCRIEN (Bit 20)                        */
#define USIC_CH_PCR_IICMode_PCRIEN_Msk        (0x100000UL)              /*!< USIC_CH PCR_IICMode: PCRIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IICMode_NACKIEN_Pos       (21UL)                    /*!< USIC_CH PCR_IICMode: NACKIEN (Bit 21)                       */
#define USIC_CH_PCR_IICMode_NACKIEN_Msk       (0x200000UL)              /*!< USIC_CH PCR_IICMode: NACKIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_IICMode_ARLIEN_Pos        (22UL)                    /*!< USIC_CH PCR_IICMode: ARLIEN (Bit 22)                        */
#define USIC_CH_PCR_IICMode_ARLIEN_Msk        (0x400000UL)              /*!< USIC_CH PCR_IICMode: ARLIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IICMode_SRRIEN_Pos        (23UL)                    /*!< USIC_CH PCR_IICMode: SRRIEN (Bit 23)                        */
#define USIC_CH_PCR_IICMode_SRRIEN_Msk        (0x800000UL)              /*!< USIC_CH PCR_IICMode: SRRIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IICMode_ERRIEN_Pos        (24UL)                    /*!< USIC_CH PCR_IICMode: ERRIEN (Bit 24)                        */
#define USIC_CH_PCR_IICMode_ERRIEN_Msk        (0x1000000UL)             /*!< USIC_CH PCR_IICMode: ERRIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IICMode_SACKDIS_Pos       (25UL)                    /*!< USIC_CH PCR_IICMode: SACKDIS (Bit 25)                       */
#define USIC_CH_PCR_IICMode_SACKDIS_Msk       (0x2000000UL)             /*!< USIC_CH PCR_IICMode: SACKDIS (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_IICMode_HDEL_Pos          (26UL)                    /*!< USIC_CH PCR_IICMode: HDEL (Bit 26)                          */
#define USIC_CH_PCR_IICMode_HDEL_Msk          (0x3c000000UL)            /*!< USIC_CH PCR_IICMode: HDEL (Bitfield-Mask: 0x0f)             */
#define USIC_CH_PCR_IICMode_ACKIEN_Pos        (30UL)                    /*!< USIC_CH PCR_IICMode: ACKIEN (Bit 30)                        */
#define USIC_CH_PCR_IICMode_ACKIEN_Msk        (0x40000000UL)            /*!< USIC_CH PCR_IICMode: ACKIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IICMode_MCLK_Pos          (31UL)                    /*!< USIC_CH PCR_IICMode: MCLK (Bit 31)                          */
#define USIC_CH_PCR_IICMode_MCLK_Msk          (0x80000000UL)            /*!< USIC_CH PCR_IICMode: MCLK (Bitfield-Mask: 0x01)             */

/* -----------------------------  USIC_CH_PCR_IISMode  ---------------------------- */
#define USIC_CH_PCR_IISMode_WAGEN_Pos         (0UL)                     /*!< USIC_CH PCR_IISMode: WAGEN (Bit 0)                          */
#define USIC_CH_PCR_IISMode_WAGEN_Msk         (0x1UL)                   /*!< USIC_CH PCR_IISMode: WAGEN (Bitfield-Mask: 0x01)            */
#define USIC_CH_PCR_IISMode_DTEN_Pos          (1UL)                     /*!< USIC_CH PCR_IISMode: DTEN (Bit 1)                           */
#define USIC_CH_PCR_IISMode_DTEN_Msk          (0x2UL)                   /*!< USIC_CH PCR_IISMode: DTEN (Bitfield-Mask: 0x01)             */
#define USIC_CH_PCR_IISMode_SELINV_Pos        (2UL)                     /*!< USIC_CH PCR_IISMode: SELINV (Bit 2)                         */
#define USIC_CH_PCR_IISMode_SELINV_Msk        (0x4UL)                   /*!< USIC_CH PCR_IISMode: SELINV (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IISMode_WAFEIEN_Pos       (4UL)                     /*!< USIC_CH PCR_IISMode: WAFEIEN (Bit 4)                        */
#define USIC_CH_PCR_IISMode_WAFEIEN_Msk       (0x10UL)                  /*!< USIC_CH PCR_IISMode: WAFEIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_IISMode_WAREIEN_Pos       (5UL)                     /*!< USIC_CH PCR_IISMode: WAREIEN (Bit 5)                        */
#define USIC_CH_PCR_IISMode_WAREIEN_Msk       (0x20UL)                  /*!< USIC_CH PCR_IISMode: WAREIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_IISMode_ENDIEN_Pos        (6UL)                     /*!< USIC_CH PCR_IISMode: ENDIEN (Bit 6)                         */
#define USIC_CH_PCR_IISMode_ENDIEN_Msk        (0x40UL)                  /*!< USIC_CH PCR_IISMode: ENDIEN (Bitfield-Mask: 0x01)           */
#define USIC_CH_PCR_IISMode_DX2TIEN_Pos       (15UL)                    /*!< USIC_CH PCR_IISMode: DX2TIEN (Bit 15)                       */
#define USIC_CH_PCR_IISMode_DX2TIEN_Msk       (0x8000UL)                /*!< USIC_CH PCR_IISMode: DX2TIEN (Bitfield-Mask: 0x01)          */
#define USIC_CH_PCR_IISMode_TDEL_Pos          (16UL)                    /*!< USIC_CH PCR_IISMode: TDEL (Bit 16)                          */
#define USIC_CH_PCR_IISMode_TDEL_Msk          (0x3f0000UL)              /*!< USIC_CH PCR_IISMode: TDEL (Bitfield-Mask: 0x3f)             */
#define USIC_CH_PCR_IISMode_MCLK_Pos          (31UL)                    /*!< USIC_CH PCR_IISMode: MCLK (Bit 31)                          */
#define USIC_CH_PCR_IISMode_MCLK_Msk          (0x80000000UL)            /*!< USIC_CH PCR_IISMode: MCLK (Bitfield-Mask: 0x01)             */

/* ---------------------------------  USIC_CH_CCR  -------------------------------- */
#define USIC_CH_CCR_MODE_Pos                  (0UL)                     /*!< USIC_CH CCR: MODE (Bit 0)                                   */
#define USIC_CH_CCR_MODE_Msk                  (0xfUL)                   /*!< USIC_CH CCR: MODE (Bitfield-Mask: 0x0f)                     */
#define USIC_CH_CCR_HPCEN_Pos                 (6UL)                     /*!< USIC_CH CCR: HPCEN (Bit 6)                                  */
#define USIC_CH_CCR_HPCEN_Msk                 (0xc0UL)                  /*!< USIC_CH CCR: HPCEN (Bitfield-Mask: 0x03)                    */
#define USIC_CH_CCR_PM_Pos                    (8UL)                     /*!< USIC_CH CCR: PM (Bit 8)                                     */
#define USIC_CH_CCR_PM_Msk                    (0x300UL)                 /*!< USIC_CH CCR: PM (Bitfield-Mask: 0x03)                       */
#define USIC_CH_CCR_RSIEN_Pos                 (10UL)                    /*!< USIC_CH CCR: RSIEN (Bit 10)                                 */
#define USIC_CH_CCR_RSIEN_Msk                 (0x400UL)                 /*!< USIC_CH CCR: RSIEN (Bitfield-Mask: 0x01)                    */
#define USIC_CH_CCR_DLIEN_Pos                 (11UL)                    /*!< USIC_CH CCR: DLIEN (Bit 11)                                 */
#define USIC_CH_CCR_DLIEN_Msk                 (0x800UL)                 /*!< USIC_CH CCR: DLIEN (Bitfield-Mask: 0x01)                    */
#define USIC_CH_CCR_TSIEN_Pos                 (12UL)                    /*!< USIC_CH CCR: TSIEN (Bit 12)                                 */
#define USIC_CH_CCR_TSIEN_Msk                 (0x1000UL)                /*!< USIC_CH CCR: TSIEN (Bitfield-Mask: 0x01)                    */
#define USIC_CH_CCR_TBIEN_Pos                 (13UL)                    /*!< USIC_CH CCR: TBIEN (Bit 13)                                 */
#define USIC_CH_CCR_TBIEN_Msk                 (0x2000UL)                /*!< USIC_CH CCR: TBIEN (Bitfield-Mask: 0x01)                    */
#define USIC_CH_CCR_RIEN_Pos                  (14UL)                    /*!< USIC_CH CCR: RIEN (Bit 14)                                  */
#define USIC_CH_CCR_RIEN_Msk                  (0x4000UL)                /*!< USIC_CH CCR: RIEN (Bitfield-Mask: 0x01)                     */
#define USIC_CH_CCR_AIEN_Pos                  (15UL)                    /*!< USIC_CH CCR: AIEN (Bit 15)                                  */
#define USIC_CH_CCR_AIEN_Msk                  (0x8000UL)                /*!< USIC_CH CCR: AIEN (Bitfield-Mask: 0x01)                     */
#define USIC_CH_CCR_BRGIEN_Pos                (16UL)                    /*!< USIC_CH CCR: BRGIEN (Bit 16)                                */
#define USIC_CH_CCR_BRGIEN_Msk                (0x10000UL)               /*!< USIC_CH CCR: BRGIEN (Bitfield-Mask: 0x01)                   */

/* --------------------------------  USIC_CH_CMTR  -------------------------------- */
#define USIC_CH_CMTR_CTV_Pos                  (0UL)                     /*!< USIC_CH CMTR: CTV (Bit 0)                                   */
#define USIC_CH_CMTR_CTV_Msk                  (0x3ffUL)                 /*!< USIC_CH CMTR: CTV (Bitfield-Mask: 0x3ff)                    */

/* ---------------------------------  USIC_CH_PSR  -------------------------------- */
#define USIC_CH_PSR_ST0_Pos                   (0UL)                     /*!< USIC_CH PSR: ST0 (Bit 0)                                    */
#define USIC_CH_PSR_ST0_Msk                   (0x1UL)                   /*!< USIC_CH PSR: ST0 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST1_Pos                   (1UL)                     /*!< USIC_CH PSR: ST1 (Bit 1)                                    */
#define USIC_CH_PSR_ST1_Msk                   (0x2UL)                   /*!< USIC_CH PSR: ST1 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST2_Pos                   (2UL)                     /*!< USIC_CH PSR: ST2 (Bit 2)                                    */
#define USIC_CH_PSR_ST2_Msk                   (0x4UL)                   /*!< USIC_CH PSR: ST2 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST3_Pos                   (3UL)                     /*!< USIC_CH PSR: ST3 (Bit 3)                                    */
#define USIC_CH_PSR_ST3_Msk                   (0x8UL)                   /*!< USIC_CH PSR: ST3 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST4_Pos                   (4UL)                     /*!< USIC_CH PSR: ST4 (Bit 4)                                    */
#define USIC_CH_PSR_ST4_Msk                   (0x10UL)                  /*!< USIC_CH PSR: ST4 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST5_Pos                   (5UL)                     /*!< USIC_CH PSR: ST5 (Bit 5)                                    */
#define USIC_CH_PSR_ST5_Msk                   (0x20UL)                  /*!< USIC_CH PSR: ST5 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST6_Pos                   (6UL)                     /*!< USIC_CH PSR: ST6 (Bit 6)                                    */
#define USIC_CH_PSR_ST6_Msk                   (0x40UL)                  /*!< USIC_CH PSR: ST6 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST7_Pos                   (7UL)                     /*!< USIC_CH PSR: ST7 (Bit 7)                                    */
#define USIC_CH_PSR_ST7_Msk                   (0x80UL)                  /*!< USIC_CH PSR: ST7 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST8_Pos                   (8UL)                     /*!< USIC_CH PSR: ST8 (Bit 8)                                    */
#define USIC_CH_PSR_ST8_Msk                   (0x100UL)                 /*!< USIC_CH PSR: ST8 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_ST9_Pos                   (9UL)                     /*!< USIC_CH PSR: ST9 (Bit 9)                                    */
#define USIC_CH_PSR_ST9_Msk                   (0x200UL)                 /*!< USIC_CH PSR: ST9 (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_RSIF_Pos                  (10UL)                    /*!< USIC_CH PSR: RSIF (Bit 10)                                  */
#define USIC_CH_PSR_RSIF_Msk                  (0x400UL)                 /*!< USIC_CH PSR: RSIF (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PSR_DLIF_Pos                  (11UL)                    /*!< USIC_CH PSR: DLIF (Bit 11)                                  */
#define USIC_CH_PSR_DLIF_Msk                  (0x800UL)                 /*!< USIC_CH PSR: DLIF (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PSR_TSIF_Pos                  (12UL)                    /*!< USIC_CH PSR: TSIF (Bit 12)                                  */
#define USIC_CH_PSR_TSIF_Msk                  (0x1000UL)                /*!< USIC_CH PSR: TSIF (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PSR_TBIF_Pos                  (13UL)                    /*!< USIC_CH PSR: TBIF (Bit 13)                                  */
#define USIC_CH_PSR_TBIF_Msk                  (0x2000UL)                /*!< USIC_CH PSR: TBIF (Bitfield-Mask: 0x01)                     */
#define USIC_CH_PSR_RIF_Pos                   (14UL)                    /*!< USIC_CH PSR: RIF (Bit 14)                                   */
#define USIC_CH_PSR_RIF_Msk                   (0x4000UL)                /*!< USIC_CH PSR: RIF (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_AIF_Pos                   (15UL)                    /*!< USIC_CH PSR: AIF (Bit 15)                                   */
#define USIC_CH_PSR_AIF_Msk                   (0x8000UL)                /*!< USIC_CH PSR: AIF (Bitfield-Mask: 0x01)                      */
#define USIC_CH_PSR_BRGIF_Pos                 (16UL)                    /*!< USIC_CH PSR: BRGIF (Bit 16)                                 */
#define USIC_CH_PSR_BRGIF_Msk                 (0x10000UL)               /*!< USIC_CH PSR: BRGIF (Bitfield-Mask: 0x01)                    */

/* -----------------------------  USIC_CH_PSR_ASCMode  ---------------------------- */
#define USIC_CH_PSR_ASCMode_TXIDLE_Pos        (0UL)                     /*!< USIC_CH PSR_ASCMode: TXIDLE (Bit 0)                         */
#define USIC_CH_PSR_ASCMode_TXIDLE_Msk        (0x1UL)                   /*!< USIC_CH PSR_ASCMode: TXIDLE (Bitfield-Mask: 0x01)           */
#define USIC_CH_PSR_ASCMode_RXIDLE_Pos        (1UL)                     /*!< USIC_CH PSR_ASCMode: RXIDLE (Bit 1)                         */
#define USIC_CH_PSR_ASCMode_RXIDLE_Msk        (0x2UL)                   /*!< USIC_CH PSR_ASCMode: RXIDLE (Bitfield-Mask: 0x01)           */
#define USIC_CH_PSR_ASCMode_SBD_Pos           (2UL)                     /*!< USIC_CH PSR_ASCMode: SBD (Bit 2)                            */
#define USIC_CH_PSR_ASCMode_SBD_Msk           (0x4UL)                   /*!< USIC_CH PSR_ASCMode: SBD (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_COL_Pos           (3UL)                     /*!< USIC_CH PSR_ASCMode: COL (Bit 3)                            */
#define USIC_CH_PSR_ASCMode_COL_Msk           (0x8UL)                   /*!< USIC_CH PSR_ASCMode: COL (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_RNS_Pos           (4UL)                     /*!< USIC_CH PSR_ASCMode: RNS (Bit 4)                            */
#define USIC_CH_PSR_ASCMode_RNS_Msk           (0x10UL)                  /*!< USIC_CH PSR_ASCMode: RNS (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_FER0_Pos          (5UL)                     /*!< USIC_CH PSR_ASCMode: FER0 (Bit 5)                           */
#define USIC_CH_PSR_ASCMode_FER0_Msk          (0x20UL)                  /*!< USIC_CH PSR_ASCMode: FER0 (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_FER1_Pos          (6UL)                     /*!< USIC_CH PSR_ASCMode: FER1 (Bit 6)                           */
#define USIC_CH_PSR_ASCMode_FER1_Msk          (0x40UL)                  /*!< USIC_CH PSR_ASCMode: FER1 (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_RFF_Pos           (7UL)                     /*!< USIC_CH PSR_ASCMode: RFF (Bit 7)                            */
#define USIC_CH_PSR_ASCMode_RFF_Msk           (0x80UL)                  /*!< USIC_CH PSR_ASCMode: RFF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_TFF_Pos           (8UL)                     /*!< USIC_CH PSR_ASCMode: TFF (Bit 8)                            */
#define USIC_CH_PSR_ASCMode_TFF_Msk           (0x100UL)                 /*!< USIC_CH PSR_ASCMode: TFF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_BUSY_Pos          (9UL)                     /*!< USIC_CH PSR_ASCMode: BUSY (Bit 9)                           */
#define USIC_CH_PSR_ASCMode_BUSY_Msk          (0x200UL)                 /*!< USIC_CH PSR_ASCMode: BUSY (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_RSIF_Pos          (10UL)                    /*!< USIC_CH PSR_ASCMode: RSIF (Bit 10)                          */
#define USIC_CH_PSR_ASCMode_RSIF_Msk          (0x400UL)                 /*!< USIC_CH PSR_ASCMode: RSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_DLIF_Pos          (11UL)                    /*!< USIC_CH PSR_ASCMode: DLIF (Bit 11)                          */
#define USIC_CH_PSR_ASCMode_DLIF_Msk          (0x800UL)                 /*!< USIC_CH PSR_ASCMode: DLIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_TSIF_Pos          (12UL)                    /*!< USIC_CH PSR_ASCMode: TSIF (Bit 12)                          */
#define USIC_CH_PSR_ASCMode_TSIF_Msk          (0x1000UL)                /*!< USIC_CH PSR_ASCMode: TSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_TBIF_Pos          (13UL)                    /*!< USIC_CH PSR_ASCMode: TBIF (Bit 13)                          */
#define USIC_CH_PSR_ASCMode_TBIF_Msk          (0x2000UL)                /*!< USIC_CH PSR_ASCMode: TBIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_ASCMode_RIF_Pos           (14UL)                    /*!< USIC_CH PSR_ASCMode: RIF (Bit 14)                           */
#define USIC_CH_PSR_ASCMode_RIF_Msk           (0x4000UL)                /*!< USIC_CH PSR_ASCMode: RIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_AIF_Pos           (15UL)                    /*!< USIC_CH PSR_ASCMode: AIF (Bit 15)                           */
#define USIC_CH_PSR_ASCMode_AIF_Msk           (0x8000UL)                /*!< USIC_CH PSR_ASCMode: AIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_ASCMode_BRGIF_Pos         (16UL)                    /*!< USIC_CH PSR_ASCMode: BRGIF (Bit 16)                         */
#define USIC_CH_PSR_ASCMode_BRGIF_Msk         (0x10000UL)               /*!< USIC_CH PSR_ASCMode: BRGIF (Bitfield-Mask: 0x01)            */

/* -----------------------------  USIC_CH_PSR_SSCMode  ---------------------------- */
#define USIC_CH_PSR_SSCMode_MSLS_Pos          (0UL)                     /*!< USIC_CH PSR_SSCMode: MSLS (Bit 0)                           */
#define USIC_CH_PSR_SSCMode_MSLS_Msk          (0x1UL)                   /*!< USIC_CH PSR_SSCMode: MSLS (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_SSCMode_DX2S_Pos          (1UL)                     /*!< USIC_CH PSR_SSCMode: DX2S (Bit 1)                           */
#define USIC_CH_PSR_SSCMode_DX2S_Msk          (0x2UL)                   /*!< USIC_CH PSR_SSCMode: DX2S (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_SSCMode_MSLSEV_Pos        (2UL)                     /*!< USIC_CH PSR_SSCMode: MSLSEV (Bit 2)                         */
#define USIC_CH_PSR_SSCMode_MSLSEV_Msk        (0x4UL)                   /*!< USIC_CH PSR_SSCMode: MSLSEV (Bitfield-Mask: 0x01)           */
#define USIC_CH_PSR_SSCMode_DX2TEV_Pos        (3UL)                     /*!< USIC_CH PSR_SSCMode: DX2TEV (Bit 3)                         */
#define USIC_CH_PSR_SSCMode_DX2TEV_Msk        (0x8UL)                   /*!< USIC_CH PSR_SSCMode: DX2TEV (Bitfield-Mask: 0x01)           */
#define USIC_CH_PSR_SSCMode_PARERR_Pos        (4UL)                     /*!< USIC_CH PSR_SSCMode: PARERR (Bit 4)                         */
#define USIC_CH_PSR_SSCMode_PARERR_Msk        (0x10UL)                  /*!< USIC_CH PSR_SSCMode: PARERR (Bitfield-Mask: 0x01)           */
#define USIC_CH_PSR_SSCMode_RSIF_Pos          (10UL)                    /*!< USIC_CH PSR_SSCMode: RSIF (Bit 10)                          */
#define USIC_CH_PSR_SSCMode_RSIF_Msk          (0x400UL)                 /*!< USIC_CH PSR_SSCMode: RSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_SSCMode_DLIF_Pos          (11UL)                    /*!< USIC_CH PSR_SSCMode: DLIF (Bit 11)                          */
#define USIC_CH_PSR_SSCMode_DLIF_Msk          (0x800UL)                 /*!< USIC_CH PSR_SSCMode: DLIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_SSCMode_TSIF_Pos          (12UL)                    /*!< USIC_CH PSR_SSCMode: TSIF (Bit 12)                          */
#define USIC_CH_PSR_SSCMode_TSIF_Msk          (0x1000UL)                /*!< USIC_CH PSR_SSCMode: TSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_SSCMode_TBIF_Pos          (13UL)                    /*!< USIC_CH PSR_SSCMode: TBIF (Bit 13)                          */
#define USIC_CH_PSR_SSCMode_TBIF_Msk          (0x2000UL)                /*!< USIC_CH PSR_SSCMode: TBIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_SSCMode_RIF_Pos           (14UL)                    /*!< USIC_CH PSR_SSCMode: RIF (Bit 14)                           */
#define USIC_CH_PSR_SSCMode_RIF_Msk           (0x4000UL)                /*!< USIC_CH PSR_SSCMode: RIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_SSCMode_AIF_Pos           (15UL)                    /*!< USIC_CH PSR_SSCMode: AIF (Bit 15)                           */
#define USIC_CH_PSR_SSCMode_AIF_Msk           (0x8000UL)                /*!< USIC_CH PSR_SSCMode: AIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_SSCMode_BRGIF_Pos         (16UL)                    /*!< USIC_CH PSR_SSCMode: BRGIF (Bit 16)                         */
#define USIC_CH_PSR_SSCMode_BRGIF_Msk         (0x10000UL)               /*!< USIC_CH PSR_SSCMode: BRGIF (Bitfield-Mask: 0x01)            */

/* -----------------------------  USIC_CH_PSR_IICMode  ---------------------------- */
#define USIC_CH_PSR_IICMode_SLSEL_Pos         (0UL)                     /*!< USIC_CH PSR_IICMode: SLSEL (Bit 0)                          */
#define USIC_CH_PSR_IICMode_SLSEL_Msk         (0x1UL)                   /*!< USIC_CH PSR_IICMode: SLSEL (Bitfield-Mask: 0x01)            */
#define USIC_CH_PSR_IICMode_WTDF_Pos          (1UL)                     /*!< USIC_CH PSR_IICMode: WTDF (Bit 1)                           */
#define USIC_CH_PSR_IICMode_WTDF_Msk          (0x2UL)                   /*!< USIC_CH PSR_IICMode: WTDF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_SCR_Pos           (2UL)                     /*!< USIC_CH PSR_IICMode: SCR (Bit 2)                            */
#define USIC_CH_PSR_IICMode_SCR_Msk           (0x4UL)                   /*!< USIC_CH PSR_IICMode: SCR (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_RSCR_Pos          (3UL)                     /*!< USIC_CH PSR_IICMode: RSCR (Bit 3)                           */
#define USIC_CH_PSR_IICMode_RSCR_Msk          (0x8UL)                   /*!< USIC_CH PSR_IICMode: RSCR (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_PCR_Pos           (4UL)                     /*!< USIC_CH PSR_IICMode: PCR (Bit 4)                            */
#define USIC_CH_PSR_IICMode_PCR_Msk           (0x10UL)                  /*!< USIC_CH PSR_IICMode: PCR (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_NACK_Pos          (5UL)                     /*!< USIC_CH PSR_IICMode: NACK (Bit 5)                           */
#define USIC_CH_PSR_IICMode_NACK_Msk          (0x20UL)                  /*!< USIC_CH PSR_IICMode: NACK (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_ARL_Pos           (6UL)                     /*!< USIC_CH PSR_IICMode: ARL (Bit 6)                            */
#define USIC_CH_PSR_IICMode_ARL_Msk           (0x40UL)                  /*!< USIC_CH PSR_IICMode: ARL (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_SRR_Pos           (7UL)                     /*!< USIC_CH PSR_IICMode: SRR (Bit 7)                            */
#define USIC_CH_PSR_IICMode_SRR_Msk           (0x80UL)                  /*!< USIC_CH PSR_IICMode: SRR (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_ERR_Pos           (8UL)                     /*!< USIC_CH PSR_IICMode: ERR (Bit 8)                            */
#define USIC_CH_PSR_IICMode_ERR_Msk           (0x100UL)                 /*!< USIC_CH PSR_IICMode: ERR (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_ACK_Pos           (9UL)                     /*!< USIC_CH PSR_IICMode: ACK (Bit 9)                            */
#define USIC_CH_PSR_IICMode_ACK_Msk           (0x200UL)                 /*!< USIC_CH PSR_IICMode: ACK (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_RSIF_Pos          (10UL)                    /*!< USIC_CH PSR_IICMode: RSIF (Bit 10)                          */
#define USIC_CH_PSR_IICMode_RSIF_Msk          (0x400UL)                 /*!< USIC_CH PSR_IICMode: RSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_DLIF_Pos          (11UL)                    /*!< USIC_CH PSR_IICMode: DLIF (Bit 11)                          */
#define USIC_CH_PSR_IICMode_DLIF_Msk          (0x800UL)                 /*!< USIC_CH PSR_IICMode: DLIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_TSIF_Pos          (12UL)                    /*!< USIC_CH PSR_IICMode: TSIF (Bit 12)                          */
#define USIC_CH_PSR_IICMode_TSIF_Msk          (0x1000UL)                /*!< USIC_CH PSR_IICMode: TSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_TBIF_Pos          (13UL)                    /*!< USIC_CH PSR_IICMode: TBIF (Bit 13)                          */
#define USIC_CH_PSR_IICMode_TBIF_Msk          (0x2000UL)                /*!< USIC_CH PSR_IICMode: TBIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IICMode_RIF_Pos           (14UL)                    /*!< USIC_CH PSR_IICMode: RIF (Bit 14)                           */
#define USIC_CH_PSR_IICMode_RIF_Msk           (0x4000UL)                /*!< USIC_CH PSR_IICMode: RIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_AIF_Pos           (15UL)                    /*!< USIC_CH PSR_IICMode: AIF (Bit 15)                           */
#define USIC_CH_PSR_IICMode_AIF_Msk           (0x8000UL)                /*!< USIC_CH PSR_IICMode: AIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IICMode_BRGIF_Pos         (16UL)                    /*!< USIC_CH PSR_IICMode: BRGIF (Bit 16)                         */
#define USIC_CH_PSR_IICMode_BRGIF_Msk         (0x10000UL)               /*!< USIC_CH PSR_IICMode: BRGIF (Bitfield-Mask: 0x01)            */

/* -----------------------------  USIC_CH_PSR_IISMode  ---------------------------- */
#define USIC_CH_PSR_IISMode_WA_Pos            (0UL)                     /*!< USIC_CH PSR_IISMode: WA (Bit 0)                             */
#define USIC_CH_PSR_IISMode_WA_Msk            (0x1UL)                   /*!< USIC_CH PSR_IISMode: WA (Bitfield-Mask: 0x01)               */
#define USIC_CH_PSR_IISMode_DX2S_Pos          (1UL)                     /*!< USIC_CH PSR_IISMode: DX2S (Bit 1)                           */
#define USIC_CH_PSR_IISMode_DX2S_Msk          (0x2UL)                   /*!< USIC_CH PSR_IISMode: DX2S (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_DX2TEV_Pos        (3UL)                     /*!< USIC_CH PSR_IISMode: DX2TEV (Bit 3)                         */
#define USIC_CH_PSR_IISMode_DX2TEV_Msk        (0x8UL)                   /*!< USIC_CH PSR_IISMode: DX2TEV (Bitfield-Mask: 0x01)           */
#define USIC_CH_PSR_IISMode_WAFE_Pos          (4UL)                     /*!< USIC_CH PSR_IISMode: WAFE (Bit 4)                           */
#define USIC_CH_PSR_IISMode_WAFE_Msk          (0x10UL)                  /*!< USIC_CH PSR_IISMode: WAFE (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_WARE_Pos          (5UL)                     /*!< USIC_CH PSR_IISMode: WARE (Bit 5)                           */
#define USIC_CH_PSR_IISMode_WARE_Msk          (0x20UL)                  /*!< USIC_CH PSR_IISMode: WARE (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_END_Pos           (6UL)                     /*!< USIC_CH PSR_IISMode: END (Bit 6)                            */
#define USIC_CH_PSR_IISMode_END_Msk           (0x40UL)                  /*!< USIC_CH PSR_IISMode: END (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IISMode_RSIF_Pos          (10UL)                    /*!< USIC_CH PSR_IISMode: RSIF (Bit 10)                          */
#define USIC_CH_PSR_IISMode_RSIF_Msk          (0x400UL)                 /*!< USIC_CH PSR_IISMode: RSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_DLIF_Pos          (11UL)                    /*!< USIC_CH PSR_IISMode: DLIF (Bit 11)                          */
#define USIC_CH_PSR_IISMode_DLIF_Msk          (0x800UL)                 /*!< USIC_CH PSR_IISMode: DLIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_TSIF_Pos          (12UL)                    /*!< USIC_CH PSR_IISMode: TSIF (Bit 12)                          */
#define USIC_CH_PSR_IISMode_TSIF_Msk          (0x1000UL)                /*!< USIC_CH PSR_IISMode: TSIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_TBIF_Pos          (13UL)                    /*!< USIC_CH PSR_IISMode: TBIF (Bit 13)                          */
#define USIC_CH_PSR_IISMode_TBIF_Msk          (0x2000UL)                /*!< USIC_CH PSR_IISMode: TBIF (Bitfield-Mask: 0x01)             */
#define USIC_CH_PSR_IISMode_RIF_Pos           (14UL)                    /*!< USIC_CH PSR_IISMode: RIF (Bit 14)                           */
#define USIC_CH_PSR_IISMode_RIF_Msk           (0x4000UL)                /*!< USIC_CH PSR_IISMode: RIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IISMode_AIF_Pos           (15UL)                    /*!< USIC_CH PSR_IISMode: AIF (Bit 15)                           */
#define USIC_CH_PSR_IISMode_AIF_Msk           (0x8000UL)                /*!< USIC_CH PSR_IISMode: AIF (Bitfield-Mask: 0x01)              */
#define USIC_CH_PSR_IISMode_BRGIF_Pos         (16UL)                    /*!< USIC_CH PSR_IISMode: BRGIF (Bit 16)                         */
#define USIC_CH_PSR_IISMode_BRGIF_Msk         (0x10000UL)               /*!< USIC_CH PSR_IISMode: BRGIF (Bitfield-Mask: 0x01)            */

/* --------------------------------  USIC_CH_PSCR  -------------------------------- */
#define USIC_CH_PSCR_CST0_Pos                 (0UL)                     /*!< USIC_CH PSCR: CST0 (Bit 0)                                  */
#define USIC_CH_PSCR_CST0_Msk                 (0x1UL)                   /*!< USIC_CH PSCR: CST0 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST1_Pos                 (1UL)                     /*!< USIC_CH PSCR: CST1 (Bit 1)                                  */
#define USIC_CH_PSCR_CST1_Msk                 (0x2UL)                   /*!< USIC_CH PSCR: CST1 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST2_Pos                 (2UL)                     /*!< USIC_CH PSCR: CST2 (Bit 2)                                  */
#define USIC_CH_PSCR_CST2_Msk                 (0x4UL)                   /*!< USIC_CH PSCR: CST2 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST3_Pos                 (3UL)                     /*!< USIC_CH PSCR: CST3 (Bit 3)                                  */
#define USIC_CH_PSCR_CST3_Msk                 (0x8UL)                   /*!< USIC_CH PSCR: CST3 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST4_Pos                 (4UL)                     /*!< USIC_CH PSCR: CST4 (Bit 4)                                  */
#define USIC_CH_PSCR_CST4_Msk                 (0x10UL)                  /*!< USIC_CH PSCR: CST4 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST5_Pos                 (5UL)                     /*!< USIC_CH PSCR: CST5 (Bit 5)                                  */
#define USIC_CH_PSCR_CST5_Msk                 (0x20UL)                  /*!< USIC_CH PSCR: CST5 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST6_Pos                 (6UL)                     /*!< USIC_CH PSCR: CST6 (Bit 6)                                  */
#define USIC_CH_PSCR_CST6_Msk                 (0x40UL)                  /*!< USIC_CH PSCR: CST6 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST7_Pos                 (7UL)                     /*!< USIC_CH PSCR: CST7 (Bit 7)                                  */
#define USIC_CH_PSCR_CST7_Msk                 (0x80UL)                  /*!< USIC_CH PSCR: CST7 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST8_Pos                 (8UL)                     /*!< USIC_CH PSCR: CST8 (Bit 8)                                  */
#define USIC_CH_PSCR_CST8_Msk                 (0x100UL)                 /*!< USIC_CH PSCR: CST8 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CST9_Pos                 (9UL)                     /*!< USIC_CH PSCR: CST9 (Bit 9)                                  */
#define USIC_CH_PSCR_CST9_Msk                 (0x200UL)                 /*!< USIC_CH PSCR: CST9 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CRSIF_Pos                (10UL)                    /*!< USIC_CH PSCR: CRSIF (Bit 10)                                */
#define USIC_CH_PSCR_CRSIF_Msk                (0x400UL)                 /*!< USIC_CH PSCR: CRSIF (Bitfield-Mask: 0x01)                   */
#define USIC_CH_PSCR_CDLIF_Pos                (11UL)                    /*!< USIC_CH PSCR: CDLIF (Bit 11)                                */
#define USIC_CH_PSCR_CDLIF_Msk                (0x800UL)                 /*!< USIC_CH PSCR: CDLIF (Bitfield-Mask: 0x01)                   */
#define USIC_CH_PSCR_CTSIF_Pos                (12UL)                    /*!< USIC_CH PSCR: CTSIF (Bit 12)                                */
#define USIC_CH_PSCR_CTSIF_Msk                (0x1000UL)                /*!< USIC_CH PSCR: CTSIF (Bitfield-Mask: 0x01)                   */
#define USIC_CH_PSCR_CTBIF_Pos                (13UL)                    /*!< USIC_CH PSCR: CTBIF (Bit 13)                                */
#define USIC_CH_PSCR_CTBIF_Msk                (0x2000UL)                /*!< USIC_CH PSCR: CTBIF (Bitfield-Mask: 0x01)                   */
#define USIC_CH_PSCR_CRIF_Pos                 (14UL)                    /*!< USIC_CH PSCR: CRIF (Bit 14)                                 */
#define USIC_CH_PSCR_CRIF_Msk                 (0x4000UL)                /*!< USIC_CH PSCR: CRIF (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CAIF_Pos                 (15UL)                    /*!< USIC_CH PSCR: CAIF (Bit 15)                                 */
#define USIC_CH_PSCR_CAIF_Msk                 (0x8000UL)                /*!< USIC_CH PSCR: CAIF (Bitfield-Mask: 0x01)                    */
#define USIC_CH_PSCR_CBRGIF_Pos               (16UL)                    /*!< USIC_CH PSCR: CBRGIF (Bit 16)                               */
#define USIC_CH_PSCR_CBRGIF_Msk               (0x10000UL)               /*!< USIC_CH PSCR: CBRGIF (Bitfield-Mask: 0x01)                  */

/* -------------------------------  USIC_CH_RBUFSR  ------------------------------- */
#define USIC_CH_RBUFSR_WLEN_Pos               (0UL)                     /*!< USIC_CH RBUFSR: WLEN (Bit 0)                                */
#define USIC_CH_RBUFSR_WLEN_Msk               (0xfUL)                   /*!< USIC_CH RBUFSR: WLEN (Bitfield-Mask: 0x0f)                  */
#define USIC_CH_RBUFSR_SOF_Pos                (6UL)                     /*!< USIC_CH RBUFSR: SOF (Bit 6)                                 */
#define USIC_CH_RBUFSR_SOF_Msk                (0x40UL)                  /*!< USIC_CH RBUFSR: SOF (Bitfield-Mask: 0x01)                   */
#define USIC_CH_RBUFSR_PAR_Pos                (8UL)                     /*!< USIC_CH RBUFSR: PAR (Bit 8)                                 */
#define USIC_CH_RBUFSR_PAR_Msk                (0x100UL)                 /*!< USIC_CH RBUFSR: PAR (Bitfield-Mask: 0x01)                   */
#define USIC_CH_RBUFSR_PERR_Pos               (9UL)                     /*!< USIC_CH RBUFSR: PERR (Bit 9)                                */
#define USIC_CH_RBUFSR_PERR_Msk               (0x200UL)                 /*!< USIC_CH RBUFSR: PERR (Bitfield-Mask: 0x01)                  */
#define USIC_CH_RBUFSR_RDV0_Pos               (13UL)                    /*!< USIC_CH RBUFSR: RDV0 (Bit 13)                               */
#define USIC_CH_RBUFSR_RDV0_Msk               (0x2000UL)                /*!< USIC_CH RBUFSR: RDV0 (Bitfield-Mask: 0x01)                  */
#define USIC_CH_RBUFSR_RDV1_Pos               (14UL)                    /*!< USIC_CH RBUFSR: RDV1 (Bit 14)                               */
#define USIC_CH_RBUFSR_RDV1_Msk               (0x4000UL)                /*!< USIC_CH RBUFSR: RDV1 (Bitfield-Mask: 0x01)                  */
#define USIC_CH_RBUFSR_DS_Pos                 (15UL)                    /*!< USIC_CH RBUFSR: DS (Bit 15)                                 */
#define USIC_CH_RBUFSR_DS_Msk                 (0x8000UL)                /*!< USIC_CH RBUFSR: DS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  USIC_CH_RBUF  -------------------------------- */
#define USIC_CH_RBUF_DSR_Pos                  (0UL)                     /*!< USIC_CH RBUF: DSR (Bit 0)                                   */
#define USIC_CH_RBUF_DSR_Msk                  (0xffffUL)                /*!< USIC_CH RBUF: DSR (Bitfield-Mask: 0xffff)                   */

/* --------------------------------  USIC_CH_RBUFD  ------------------------------- */
#define USIC_CH_RBUFD_DSR_Pos                 (0UL)                     /*!< USIC_CH RBUFD: DSR (Bit 0)                                  */
#define USIC_CH_RBUFD_DSR_Msk                 (0xffffUL)                /*!< USIC_CH RBUFD: DSR (Bitfield-Mask: 0xffff)                  */

/* --------------------------------  USIC_CH_RBUF0  ------------------------------- */
#define USIC_CH_RBUF0_DSR0_Pos                (0UL)                     /*!< USIC_CH RBUF0: DSR0 (Bit 0)                                 */
#define USIC_CH_RBUF0_DSR0_Msk                (0xffffUL)                /*!< USIC_CH RBUF0: DSR0 (Bitfield-Mask: 0xffff)                 */

/* --------------------------------  USIC_CH_RBUF1  ------------------------------- */
#define USIC_CH_RBUF1_DSR1_Pos                (0UL)                     /*!< USIC_CH RBUF1: DSR1 (Bit 0)                                 */
#define USIC_CH_RBUF1_DSR1_Msk                (0xffffUL)                /*!< USIC_CH RBUF1: DSR1 (Bitfield-Mask: 0xffff)                 */

/* ------------------------------  USIC_CH_RBUF01SR  ------------------------------ */
#define USIC_CH_RBUF01SR_WLEN0_Pos            (0UL)                     /*!< USIC_CH RBUF01SR: WLEN0 (Bit 0)                             */
#define USIC_CH_RBUF01SR_WLEN0_Msk            (0xfUL)                   /*!< USIC_CH RBUF01SR: WLEN0 (Bitfield-Mask: 0x0f)               */
#define USIC_CH_RBUF01SR_SOF0_Pos             (6UL)                     /*!< USIC_CH RBUF01SR: SOF0 (Bit 6)                              */
#define USIC_CH_RBUF01SR_SOF0_Msk             (0x40UL)                  /*!< USIC_CH RBUF01SR: SOF0 (Bitfield-Mask: 0x01)                */
#define USIC_CH_RBUF01SR_PAR0_Pos             (8UL)                     /*!< USIC_CH RBUF01SR: PAR0 (Bit 8)                              */
#define USIC_CH_RBUF01SR_PAR0_Msk             (0x100UL)                 /*!< USIC_CH RBUF01SR: PAR0 (Bitfield-Mask: 0x01)                */
#define USIC_CH_RBUF01SR_PERR0_Pos            (9UL)                     /*!< USIC_CH RBUF01SR: PERR0 (Bit 9)                             */
#define USIC_CH_RBUF01SR_PERR0_Msk            (0x200UL)                 /*!< USIC_CH RBUF01SR: PERR0 (Bitfield-Mask: 0x01)               */
#define USIC_CH_RBUF01SR_RDV00_Pos            (13UL)                    /*!< USIC_CH RBUF01SR: RDV00 (Bit 13)                            */
#define USIC_CH_RBUF01SR_RDV00_Msk            (0x2000UL)                /*!< USIC_CH RBUF01SR: RDV00 (Bitfield-Mask: 0x01)               */
#define USIC_CH_RBUF01SR_RDV01_Pos            (14UL)                    /*!< USIC_CH RBUF01SR: RDV01 (Bit 14)                            */
#define USIC_CH_RBUF01SR_RDV01_Msk            (0x4000UL)                /*!< USIC_CH RBUF01SR: RDV01 (Bitfield-Mask: 0x01)               */
#define USIC_CH_RBUF01SR_DS0_Pos              (15UL)                    /*!< USIC_CH RBUF01SR: DS0 (Bit 15)                              */
#define USIC_CH_RBUF01SR_DS0_Msk              (0x8000UL)                /*!< USIC_CH RBUF01SR: DS0 (Bitfield-Mask: 0x01)                 */
#define USIC_CH_RBUF01SR_WLEN1_Pos            (16UL)                    /*!< USIC_CH RBUF01SR: WLEN1 (Bit 16)                            */
#define USIC_CH_RBUF01SR_WLEN1_Msk            (0xf0000UL)               /*!< USIC_CH RBUF01SR: WLEN1 (Bitfield-Mask: 0x0f)               */
#define USIC_CH_RBUF01SR_SOF1_Pos             (22UL)                    /*!< USIC_CH RBUF01SR: SOF1 (Bit 22)                             */
#define USIC_CH_RBUF01SR_SOF1_Msk             (0x400000UL)              /*!< USIC_CH RBUF01SR: SOF1 (Bitfield-Mask: 0x01)                */
#define USIC_CH_RBUF01SR_PAR1_Pos             (24UL)                    /*!< USIC_CH RBUF01SR: PAR1 (Bit 24)                             */
#define USIC_CH_RBUF01SR_PAR1_Msk             (0x1000000UL)             /*!< USIC_CH RBUF01SR: PAR1 (Bitfield-Mask: 0x01)                */
#define USIC_CH_RBUF01SR_PERR1_Pos            (25UL)                    /*!< USIC_CH RBUF01SR: PERR1 (Bit 25)                            */
#define USIC_CH_RBUF01SR_PERR1_Msk            (0x2000000UL)             /*!< USIC_CH RBUF01SR: PERR1 (Bitfield-Mask: 0x01)               */
#define USIC_CH_RBUF01SR_RDV10_Pos            (29UL)                    /*!< USIC_CH RBUF01SR: RDV10 (Bit 29)                            */
#define USIC_CH_RBUF01SR_RDV10_Msk            (0x20000000UL)            /*!< USIC_CH RBUF01SR: RDV10 (Bitfield-Mask: 0x01)               */
#define USIC_CH_RBUF01SR_RDV11_Pos            (30UL)                    /*!< USIC_CH RBUF01SR: RDV11 (Bit 30)                            */
#define USIC_CH_RBUF01SR_RDV11_Msk            (0x40000000UL)            /*!< USIC_CH RBUF01SR: RDV11 (Bitfield-Mask: 0x01)               */
#define USIC_CH_RBUF01SR_DS1_Pos              (31UL)                    /*!< USIC_CH RBUF01SR: DS1 (Bit 31)                              */
#define USIC_CH_RBUF01SR_DS1_Msk              (0x80000000UL)            /*!< USIC_CH RBUF01SR: DS1 (Bitfield-Mask: 0x01)                 */

/* ---------------------------------  USIC_CH_FMR  -------------------------------- */
#define USIC_CH_FMR_MTDV_Pos                  (0UL)                     /*!< USIC_CH FMR: MTDV (Bit 0)                                   */
#define USIC_CH_FMR_MTDV_Msk                  (0x3UL)                   /*!< USIC_CH FMR: MTDV (Bitfield-Mask: 0x03)                     */
#define USIC_CH_FMR_ATVC_Pos                  (4UL)                     /*!< USIC_CH FMR: ATVC (Bit 4)                                   */
#define USIC_CH_FMR_ATVC_Msk                  (0x10UL)                  /*!< USIC_CH FMR: ATVC (Bitfield-Mask: 0x01)                     */
#define USIC_CH_FMR_CRDV0_Pos                 (14UL)                    /*!< USIC_CH FMR: CRDV0 (Bit 14)                                 */
#define USIC_CH_FMR_CRDV0_Msk                 (0x4000UL)                /*!< USIC_CH FMR: CRDV0 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_FMR_CRDV1_Pos                 (15UL)                    /*!< USIC_CH FMR: CRDV1 (Bit 15)                                 */
#define USIC_CH_FMR_CRDV1_Msk                 (0x8000UL)                /*!< USIC_CH FMR: CRDV1 (Bitfield-Mask: 0x01)                    */
#define USIC_CH_FMR_SIO0_Pos                  (16UL)                    /*!< USIC_CH FMR: SIO0 (Bit 16)                                  */
#define USIC_CH_FMR_SIO0_Msk                  (0x10000UL)               /*!< USIC_CH FMR: SIO0 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_FMR_SIO1_Pos                  (17UL)                    /*!< USIC_CH FMR: SIO1 (Bit 17)                                  */
#define USIC_CH_FMR_SIO1_Msk                  (0x20000UL)               /*!< USIC_CH FMR: SIO1 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_FMR_SIO2_Pos                  (18UL)                    /*!< USIC_CH FMR: SIO2 (Bit 18)                                  */
#define USIC_CH_FMR_SIO2_Msk                  (0x40000UL)               /*!< USIC_CH FMR: SIO2 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_FMR_SIO3_Pos                  (19UL)                    /*!< USIC_CH FMR: SIO3 (Bit 19)                                  */
#define USIC_CH_FMR_SIO3_Msk                  (0x80000UL)               /*!< USIC_CH FMR: SIO3 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_FMR_SIO4_Pos                  (20UL)                    /*!< USIC_CH FMR: SIO4 (Bit 20)                                  */
#define USIC_CH_FMR_SIO4_Msk                  (0x100000UL)              /*!< USIC_CH FMR: SIO4 (Bitfield-Mask: 0x01)                     */
#define USIC_CH_FMR_SIO5_Pos                  (21UL)                    /*!< USIC_CH FMR: SIO5 (Bit 21)                                  */
#define USIC_CH_FMR_SIO5_Msk                  (0x200000UL)              /*!< USIC_CH FMR: SIO5 (Bitfield-Mask: 0x01)                     */

/* --------------------------------  USIC_CH_TBUF  -------------------------------- */
#define USIC_CH_TBUF_TDATA_Pos                (0UL)                     /*!< USIC_CH TBUF: TDATA (Bit 0)                                 */
#define USIC_CH_TBUF_TDATA_Msk                (0xffffUL)                /*!< USIC_CH TBUF: TDATA (Bitfield-Mask: 0xffff)                 */

/* ---------------------------------  USIC_CH_BYP  -------------------------------- */
#define USIC_CH_BYP_BDATA_Pos                 (0UL)                     /*!< USIC_CH BYP: BDATA (Bit 0)                                  */
#define USIC_CH_BYP_BDATA_Msk                 (0xffffUL)                /*!< USIC_CH BYP: BDATA (Bitfield-Mask: 0xffff)                  */

/* --------------------------------  USIC_CH_BYPCR  ------------------------------- */
#define USIC_CH_BYPCR_BWLE_Pos                (0UL)                     /*!< USIC_CH BYPCR: BWLE (Bit 0)                                 */
#define USIC_CH_BYPCR_BWLE_Msk                (0xfUL)                   /*!< USIC_CH BYPCR: BWLE (Bitfield-Mask: 0x0f)                   */
#define USIC_CH_BYPCR_BDSSM_Pos               (8UL)                     /*!< USIC_CH BYPCR: BDSSM (Bit 8)                                */
#define USIC_CH_BYPCR_BDSSM_Msk               (0x100UL)                 /*!< USIC_CH BYPCR: BDSSM (Bitfield-Mask: 0x01)                  */
#define USIC_CH_BYPCR_BDEN_Pos                (10UL)                    /*!< USIC_CH BYPCR: BDEN (Bit 10)                                */
#define USIC_CH_BYPCR_BDEN_Msk                (0xc00UL)                 /*!< USIC_CH BYPCR: BDEN (Bitfield-Mask: 0x03)                   */
#define USIC_CH_BYPCR_BDVTR_Pos               (12UL)                    /*!< USIC_CH BYPCR: BDVTR (Bit 12)                               */
#define USIC_CH_BYPCR_BDVTR_Msk               (0x1000UL)                /*!< USIC_CH BYPCR: BDVTR (Bitfield-Mask: 0x01)                  */
#define USIC_CH_BYPCR_BPRIO_Pos               (13UL)                    /*!< USIC_CH BYPCR: BPRIO (Bit 13)                               */
#define USIC_CH_BYPCR_BPRIO_Msk               (0x2000UL)                /*!< USIC_CH BYPCR: BPRIO (Bitfield-Mask: 0x01)                  */
#define USIC_CH_BYPCR_BDV_Pos                 (15UL)                    /*!< USIC_CH BYPCR: BDV (Bit 15)                                 */
#define USIC_CH_BYPCR_BDV_Msk                 (0x8000UL)                /*!< USIC_CH BYPCR: BDV (Bitfield-Mask: 0x01)                    */
#define USIC_CH_BYPCR_BSELO_Pos               (16UL)                    /*!< USIC_CH BYPCR: BSELO (Bit 16)                               */
#define USIC_CH_BYPCR_BSELO_Msk               (0x1f0000UL)              /*!< USIC_CH BYPCR: BSELO (Bitfield-Mask: 0x1f)                  */
#define USIC_CH_BYPCR_BHPC_Pos                (21UL)                    /*!< USIC_CH BYPCR: BHPC (Bit 21)                                */
#define USIC_CH_BYPCR_BHPC_Msk                (0xe00000UL)              /*!< USIC_CH BYPCR: BHPC (Bitfield-Mask: 0x07)                   */

/* --------------------------------  USIC_CH_TBCTR  ------------------------------- */
#define USIC_CH_TBCTR_DPTR_Pos                (0UL)                     /*!< USIC_CH TBCTR: DPTR (Bit 0)                                 */
#define USIC_CH_TBCTR_DPTR_Msk                (0x3fUL)                  /*!< USIC_CH TBCTR: DPTR (Bitfield-Mask: 0x3f)                   */
#define USIC_CH_TBCTR_LIMIT_Pos               (8UL)                     /*!< USIC_CH TBCTR: LIMIT (Bit 8)                                */
#define USIC_CH_TBCTR_LIMIT_Msk               (0x3f00UL)                /*!< USIC_CH TBCTR: LIMIT (Bitfield-Mask: 0x3f)                  */
#define USIC_CH_TBCTR_STBTM_Pos               (14UL)                    /*!< USIC_CH TBCTR: STBTM (Bit 14)                               */
#define USIC_CH_TBCTR_STBTM_Msk               (0x4000UL)                /*!< USIC_CH TBCTR: STBTM (Bitfield-Mask: 0x01)                  */
#define USIC_CH_TBCTR_STBTEN_Pos              (15UL)                    /*!< USIC_CH TBCTR: STBTEN (Bit 15)                              */
#define USIC_CH_TBCTR_STBTEN_Msk              (0x8000UL)                /*!< USIC_CH TBCTR: STBTEN (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TBCTR_STBINP_Pos              (16UL)                    /*!< USIC_CH TBCTR: STBINP (Bit 16)                              */
#define USIC_CH_TBCTR_STBINP_Msk              (0x70000UL)               /*!< USIC_CH TBCTR: STBINP (Bitfield-Mask: 0x07)                 */
#define USIC_CH_TBCTR_ATBINP_Pos              (19UL)                    /*!< USIC_CH TBCTR: ATBINP (Bit 19)                              */
#define USIC_CH_TBCTR_ATBINP_Msk              (0x380000UL)              /*!< USIC_CH TBCTR: ATBINP (Bitfield-Mask: 0x07)                 */
#define USIC_CH_TBCTR_SIZE_Pos                (24UL)                    /*!< USIC_CH TBCTR: SIZE (Bit 24)                                */
#define USIC_CH_TBCTR_SIZE_Msk                (0x7000000UL)             /*!< USIC_CH TBCTR: SIZE (Bitfield-Mask: 0x07)                   */
#define USIC_CH_TBCTR_LOF_Pos                 (28UL)                    /*!< USIC_CH TBCTR: LOF (Bit 28)                                 */
#define USIC_CH_TBCTR_LOF_Msk                 (0x10000000UL)            /*!< USIC_CH TBCTR: LOF (Bitfield-Mask: 0x01)                    */
#define USIC_CH_TBCTR_STBIEN_Pos              (30UL)                    /*!< USIC_CH TBCTR: STBIEN (Bit 30)                              */
#define USIC_CH_TBCTR_STBIEN_Msk              (0x40000000UL)            /*!< USIC_CH TBCTR: STBIEN (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TBCTR_TBERIEN_Pos             (31UL)                    /*!< USIC_CH TBCTR: TBERIEN (Bit 31)                             */
#define USIC_CH_TBCTR_TBERIEN_Msk             (0x80000000UL)            /*!< USIC_CH TBCTR: TBERIEN (Bitfield-Mask: 0x01)                */

/* --------------------------------  USIC_CH_RBCTR  ------------------------------- */
#define USIC_CH_RBCTR_DPTR_Pos                (0UL)                     /*!< USIC_CH RBCTR: DPTR (Bit 0)                                 */
#define USIC_CH_RBCTR_DPTR_Msk                (0x3fUL)                  /*!< USIC_CH RBCTR: DPTR (Bitfield-Mask: 0x3f)                   */
#define USIC_CH_RBCTR_LIMIT_Pos               (8UL)                     /*!< USIC_CH RBCTR: LIMIT (Bit 8)                                */
#define USIC_CH_RBCTR_LIMIT_Msk               (0x3f00UL)                /*!< USIC_CH RBCTR: LIMIT (Bitfield-Mask: 0x3f)                  */
#define USIC_CH_RBCTR_SRBTM_Pos               (14UL)                    /*!< USIC_CH RBCTR: SRBTM (Bit 14)                               */
#define USIC_CH_RBCTR_SRBTM_Msk               (0x4000UL)                /*!< USIC_CH RBCTR: SRBTM (Bitfield-Mask: 0x01)                  */
#define USIC_CH_RBCTR_SRBTEN_Pos              (15UL)                    /*!< USIC_CH RBCTR: SRBTEN (Bit 15)                              */
#define USIC_CH_RBCTR_SRBTEN_Msk              (0x8000UL)                /*!< USIC_CH RBCTR: SRBTEN (Bitfield-Mask: 0x01)                 */
#define USIC_CH_RBCTR_SRBINP_Pos              (16UL)                    /*!< USIC_CH RBCTR: SRBINP (Bit 16)                              */
#define USIC_CH_RBCTR_SRBINP_Msk              (0x70000UL)               /*!< USIC_CH RBCTR: SRBINP (Bitfield-Mask: 0x07)                 */
#define USIC_CH_RBCTR_ARBINP_Pos              (19UL)                    /*!< USIC_CH RBCTR: ARBINP (Bit 19)                              */
#define USIC_CH_RBCTR_ARBINP_Msk              (0x380000UL)              /*!< USIC_CH RBCTR: ARBINP (Bitfield-Mask: 0x07)                 */
#define USIC_CH_RBCTR_RCIM_Pos                (22UL)                    /*!< USIC_CH RBCTR: RCIM (Bit 22)                                */
#define USIC_CH_RBCTR_RCIM_Msk                (0xc00000UL)              /*!< USIC_CH RBCTR: RCIM (Bitfield-Mask: 0x03)                   */
#define USIC_CH_RBCTR_SIZE_Pos                (24UL)                    /*!< USIC_CH RBCTR: SIZE (Bit 24)                                */
#define USIC_CH_RBCTR_SIZE_Msk                (0x7000000UL)             /*!< USIC_CH RBCTR: SIZE (Bitfield-Mask: 0x07)                   */
#define USIC_CH_RBCTR_RNM_Pos                 (27UL)                    /*!< USIC_CH RBCTR: RNM (Bit 27)                                 */
#define USIC_CH_RBCTR_RNM_Msk                 (0x8000000UL)             /*!< USIC_CH RBCTR: RNM (Bitfield-Mask: 0x01)                    */
#define USIC_CH_RBCTR_LOF_Pos                 (28UL)                    /*!< USIC_CH RBCTR: LOF (Bit 28)                                 */
#define USIC_CH_RBCTR_LOF_Msk                 (0x10000000UL)            /*!< USIC_CH RBCTR: LOF (Bitfield-Mask: 0x01)                    */
#define USIC_CH_RBCTR_ARBIEN_Pos              (29UL)                    /*!< USIC_CH RBCTR: ARBIEN (Bit 29)                              */
#define USIC_CH_RBCTR_ARBIEN_Msk              (0x20000000UL)            /*!< USIC_CH RBCTR: ARBIEN (Bitfield-Mask: 0x01)                 */
#define USIC_CH_RBCTR_SRBIEN_Pos              (30UL)                    /*!< USIC_CH RBCTR: SRBIEN (Bit 30)                              */
#define USIC_CH_RBCTR_SRBIEN_Msk              (0x40000000UL)            /*!< USIC_CH RBCTR: SRBIEN (Bitfield-Mask: 0x01)                 */
#define USIC_CH_RBCTR_RBERIEN_Pos             (31UL)                    /*!< USIC_CH RBCTR: RBERIEN (Bit 31)                             */
#define USIC_CH_RBCTR_RBERIEN_Msk             (0x80000000UL)            /*!< USIC_CH RBCTR: RBERIEN (Bitfield-Mask: 0x01)                */

/* -------------------------------  USIC_CH_TRBPTR  ------------------------------- */
#define USIC_CH_TRBPTR_TDIPTR_Pos             (0UL)                     /*!< USIC_CH TRBPTR: TDIPTR (Bit 0)                              */
#define USIC_CH_TRBPTR_TDIPTR_Msk             (0x3fUL)                  /*!< USIC_CH TRBPTR: TDIPTR (Bitfield-Mask: 0x3f)                */
#define USIC_CH_TRBPTR_TDOPTR_Pos             (8UL)                     /*!< USIC_CH TRBPTR: TDOPTR (Bit 8)                              */
#define USIC_CH_TRBPTR_TDOPTR_Msk             (0x3f00UL)                /*!< USIC_CH TRBPTR: TDOPTR (Bitfield-Mask: 0x3f)                */
#define USIC_CH_TRBPTR_RDIPTR_Pos             (16UL)                    /*!< USIC_CH TRBPTR: RDIPTR (Bit 16)                             */
#define USIC_CH_TRBPTR_RDIPTR_Msk             (0x3f0000UL)              /*!< USIC_CH TRBPTR: RDIPTR (Bitfield-Mask: 0x3f)                */
#define USIC_CH_TRBPTR_RDOPTR_Pos             (24UL)                    /*!< USIC_CH TRBPTR: RDOPTR (Bit 24)                             */
#define USIC_CH_TRBPTR_RDOPTR_Msk             (0x3f000000UL)            /*!< USIC_CH TRBPTR: RDOPTR (Bitfield-Mask: 0x3f)                */

/* --------------------------------  USIC_CH_TRBSR  ------------------------------- */
#define USIC_CH_TRBSR_SRBI_Pos                (0UL)                     /*!< USIC_CH TRBSR: SRBI (Bit 0)                                 */
#define USIC_CH_TRBSR_SRBI_Msk                (0x1UL)                   /*!< USIC_CH TRBSR: SRBI (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_RBERI_Pos               (1UL)                     /*!< USIC_CH TRBSR: RBERI (Bit 1)                                */
#define USIC_CH_TRBSR_RBERI_Msk               (0x2UL)                   /*!< USIC_CH TRBSR: RBERI (Bitfield-Mask: 0x01)                  */
#define USIC_CH_TRBSR_ARBI_Pos                (2UL)                     /*!< USIC_CH TRBSR: ARBI (Bit 2)                                 */
#define USIC_CH_TRBSR_ARBI_Msk                (0x4UL)                   /*!< USIC_CH TRBSR: ARBI (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_REMPTY_Pos              (3UL)                     /*!< USIC_CH TRBSR: REMPTY (Bit 3)                               */
#define USIC_CH_TRBSR_REMPTY_Msk              (0x8UL)                   /*!< USIC_CH TRBSR: REMPTY (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TRBSR_RFULL_Pos               (4UL)                     /*!< USIC_CH TRBSR: RFULL (Bit 4)                                */
#define USIC_CH_TRBSR_RFULL_Msk               (0x10UL)                  /*!< USIC_CH TRBSR: RFULL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_TRBSR_RBUS_Pos                (5UL)                     /*!< USIC_CH TRBSR: RBUS (Bit 5)                                 */
#define USIC_CH_TRBSR_RBUS_Msk                (0x20UL)                  /*!< USIC_CH TRBSR: RBUS (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_SRBT_Pos                (6UL)                     /*!< USIC_CH TRBSR: SRBT (Bit 6)                                 */
#define USIC_CH_TRBSR_SRBT_Msk                (0x40UL)                  /*!< USIC_CH TRBSR: SRBT (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_STBI_Pos                (8UL)                     /*!< USIC_CH TRBSR: STBI (Bit 8)                                 */
#define USIC_CH_TRBSR_STBI_Msk                (0x100UL)                 /*!< USIC_CH TRBSR: STBI (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_TBERI_Pos               (9UL)                     /*!< USIC_CH TRBSR: TBERI (Bit 9)                                */
#define USIC_CH_TRBSR_TBERI_Msk               (0x200UL)                 /*!< USIC_CH TRBSR: TBERI (Bitfield-Mask: 0x01)                  */
#define USIC_CH_TRBSR_TEMPTY_Pos              (11UL)                    /*!< USIC_CH TRBSR: TEMPTY (Bit 11)                              */
#define USIC_CH_TRBSR_TEMPTY_Msk              (0x800UL)                 /*!< USIC_CH TRBSR: TEMPTY (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TRBSR_TFULL_Pos               (12UL)                    /*!< USIC_CH TRBSR: TFULL (Bit 12)                               */
#define USIC_CH_TRBSR_TFULL_Msk               (0x1000UL)                /*!< USIC_CH TRBSR: TFULL (Bitfield-Mask: 0x01)                  */
#define USIC_CH_TRBSR_TBUS_Pos                (13UL)                    /*!< USIC_CH TRBSR: TBUS (Bit 13)                                */
#define USIC_CH_TRBSR_TBUS_Msk                (0x2000UL)                /*!< USIC_CH TRBSR: TBUS (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_STBT_Pos                (14UL)                    /*!< USIC_CH TRBSR: STBT (Bit 14)                                */
#define USIC_CH_TRBSR_STBT_Msk                (0x4000UL)                /*!< USIC_CH TRBSR: STBT (Bitfield-Mask: 0x01)                   */
#define USIC_CH_TRBSR_RBFLVL_Pos              (16UL)                    /*!< USIC_CH TRBSR: RBFLVL (Bit 16)                              */
#define USIC_CH_TRBSR_RBFLVL_Msk              (0x7f0000UL)              /*!< USIC_CH TRBSR: RBFLVL (Bitfield-Mask: 0x7f)                 */
#define USIC_CH_TRBSR_TBFLVL_Pos              (24UL)                    /*!< USIC_CH TRBSR: TBFLVL (Bit 24)                              */
#define USIC_CH_TRBSR_TBFLVL_Msk              (0x7f000000UL)            /*!< USIC_CH TRBSR: TBFLVL (Bitfield-Mask: 0x7f)                 */

/* -------------------------------  USIC_CH_TRBSCR  ------------------------------- */
#define USIC_CH_TRBSCR_CSRBI_Pos              (0UL)                     /*!< USIC_CH TRBSCR: CSRBI (Bit 0)                               */
#define USIC_CH_TRBSCR_CSRBI_Msk              (0x1UL)                   /*!< USIC_CH TRBSCR: CSRBI (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TRBSCR_CRBERI_Pos             (1UL)                     /*!< USIC_CH TRBSCR: CRBERI (Bit 1)                              */
#define USIC_CH_TRBSCR_CRBERI_Msk             (0x2UL)                   /*!< USIC_CH TRBSCR: CRBERI (Bitfield-Mask: 0x01)                */
#define USIC_CH_TRBSCR_CARBI_Pos              (2UL)                     /*!< USIC_CH TRBSCR: CARBI (Bit 2)                               */
#define USIC_CH_TRBSCR_CARBI_Msk              (0x4UL)                   /*!< USIC_CH TRBSCR: CARBI (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TRBSCR_CSTBI_Pos              (8UL)                     /*!< USIC_CH TRBSCR: CSTBI (Bit 8)                               */
#define USIC_CH_TRBSCR_CSTBI_Msk              (0x100UL)                 /*!< USIC_CH TRBSCR: CSTBI (Bitfield-Mask: 0x01)                 */
#define USIC_CH_TRBSCR_CTBERI_Pos             (9UL)                     /*!< USIC_CH TRBSCR: CTBERI (Bit 9)                              */
#define USIC_CH_TRBSCR_CTBERI_Msk             (0x200UL)                 /*!< USIC_CH TRBSCR: CTBERI (Bitfield-Mask: 0x01)                */
#define USIC_CH_TRBSCR_CBDV_Pos               (10UL)                    /*!< USIC_CH TRBSCR: CBDV (Bit 10)                               */
#define USIC_CH_TRBSCR_CBDV_Msk               (0x400UL)                 /*!< USIC_CH TRBSCR: CBDV (Bitfield-Mask: 0x01)                  */
#define USIC_CH_TRBSCR_FLUSHRB_Pos            (14UL)                    /*!< USIC_CH TRBSCR: FLUSHRB (Bit 14)                            */
#define USIC_CH_TRBSCR_FLUSHRB_Msk            (0x4000UL)                /*!< USIC_CH TRBSCR: FLUSHRB (Bitfield-Mask: 0x01)               */
#define USIC_CH_TRBSCR_FLUSHTB_Pos            (15UL)                    /*!< USIC_CH TRBSCR: FLUSHTB (Bit 15)                            */
#define USIC_CH_TRBSCR_FLUSHTB_Msk            (0x8000UL)                /*!< USIC_CH TRBSCR: FLUSHTB (Bitfield-Mask: 0x01)               */

/* --------------------------------  USIC_CH_OUTR  -------------------------------- */
#define USIC_CH_OUTR_DSR_Pos                  (0UL)                     /*!< USIC_CH OUTR: DSR (Bit 0)                                   */
#define USIC_CH_OUTR_DSR_Msk                  (0xffffUL)                /*!< USIC_CH OUTR: DSR (Bitfield-Mask: 0xffff)                   */
#define USIC_CH_OUTR_RCI_Pos                  (16UL)                    /*!< USIC_CH OUTR: RCI (Bit 16)                                  */
#define USIC_CH_OUTR_RCI_Msk                  (0x1f0000UL)              /*!< USIC_CH OUTR: RCI (Bitfield-Mask: 0x1f)                     */

/* --------------------------------  USIC_CH_OUTDR  ------------------------------- */
#define USIC_CH_OUTDR_DSR_Pos                 (0UL)                     /*!< USIC_CH OUTDR: DSR (Bit 0)                                  */
#define USIC_CH_OUTDR_DSR_Msk                 (0xffffUL)                /*!< USIC_CH OUTDR: DSR (Bitfield-Mask: 0xffff)                  */
#define USIC_CH_OUTDR_RCI_Pos                 (16UL)                    /*!< USIC_CH OUTDR: RCI (Bit 16)                                 */
#define USIC_CH_OUTDR_RCI_Msk                 (0x1f0000UL)              /*!< USIC_CH OUTDR: RCI (Bitfield-Mask: 0x1f)                    */

/* ---------------------------------  USIC_CH_IN  --------------------------------- */
#define USIC_CH_IN_TDATA_Pos                  (0UL)                     /*!< USIC_CH IN: TDATA (Bit 0)                                   */
#define USIC_CH_IN_TDATA_Msk                  (0xffffUL)                /*!< USIC_CH IN: TDATA (Bitfield-Mask: 0xffff)                   */


/* ================================================================================ */
/* ================      struct 'SCU_GENERAL' Position & Mask      ================ */
/* ================================================================================ */


/* ----------------------------  SCU_GENERAL_DBGROMID  ---------------------------- */
#define SCU_GENERAL_DBGROMID_MANUFID_Pos      (1UL)                     /*!< SCU_GENERAL DBGROMID: MANUFID (Bit 1)                       */
#define SCU_GENERAL_DBGROMID_MANUFID_Msk      (0xffeUL)                 /*!< SCU_GENERAL DBGROMID: MANUFID (Bitfield-Mask: 0x7ff)        */
#define SCU_GENERAL_DBGROMID_PARTNO_Pos       (12UL)                    /*!< SCU_GENERAL DBGROMID: PARTNO (Bit 12)                       */
#define SCU_GENERAL_DBGROMID_PARTNO_Msk       (0xffff000UL)             /*!< SCU_GENERAL DBGROMID: PARTNO (Bitfield-Mask: 0xffff)        */
#define SCU_GENERAL_DBGROMID_VERSION_Pos      (28UL)                    /*!< SCU_GENERAL DBGROMID: VERSION (Bit 28)                      */
#define SCU_GENERAL_DBGROMID_VERSION_Msk      (0xf0000000UL)            /*!< SCU_GENERAL DBGROMID: VERSION (Bitfield-Mask: 0x0f)         */

/* -----------------------------  SCU_GENERAL_IDCHIP  ----------------------------- */
#define SCU_GENERAL_IDCHIP_IDCHIP_Pos         (0UL)                     /*!< SCU_GENERAL IDCHIP: IDCHIP (Bit 0)                          */
#define SCU_GENERAL_IDCHIP_IDCHIP_Msk         (0xffffffffUL)            /*!< SCU_GENERAL IDCHIP: IDCHIP (Bitfield-Mask: 0xffffffff)      */

/* -------------------------------  SCU_GENERAL_ID  ------------------------------- */
#define SCU_GENERAL_ID_MOD_REV_Pos            (0UL)                     /*!< SCU_GENERAL ID: MOD_REV (Bit 0)                             */
#define SCU_GENERAL_ID_MOD_REV_Msk            (0xffUL)                  /*!< SCU_GENERAL ID: MOD_REV (Bitfield-Mask: 0xff)               */
#define SCU_GENERAL_ID_MOD_TYPE_Pos           (8UL)                     /*!< SCU_GENERAL ID: MOD_TYPE (Bit 8)                            */
#define SCU_GENERAL_ID_MOD_TYPE_Msk           (0xff00UL)                /*!< SCU_GENERAL ID: MOD_TYPE (Bitfield-Mask: 0xff)              */
#define SCU_GENERAL_ID_MOD_NUMBER_Pos         (16UL)                    /*!< SCU_GENERAL ID: MOD_NUMBER (Bit 16)                         */
#define SCU_GENERAL_ID_MOD_NUMBER_Msk         (0xffff0000UL)            /*!< SCU_GENERAL ID: MOD_NUMBER (Bitfield-Mask: 0xffff)          */

/* ------------------------------  SCU_GENERAL_SSW0  ------------------------------ */
#define SCU_GENERAL_SSW0_DAT_Pos              (0UL)                     /*!< SCU_GENERAL SSW0: DAT (Bit 0)                               */
#define SCU_GENERAL_SSW0_DAT_Msk              (0xffffffffUL)            /*!< SCU_GENERAL SSW0: DAT (Bitfield-Mask: 0xffffffff)           */

/* -----------------------------  SCU_GENERAL_PASSWD  ----------------------------- */
#define SCU_GENERAL_PASSWD_MODE_Pos           (0UL)                     /*!< SCU_GENERAL PASSWD: MODE (Bit 0)                            */
#define SCU_GENERAL_PASSWD_MODE_Msk           (0x3UL)                   /*!< SCU_GENERAL PASSWD: MODE (Bitfield-Mask: 0x03)              */
#define SCU_GENERAL_PASSWD_PROTS_Pos          (2UL)                     /*!< SCU_GENERAL PASSWD: PROTS (Bit 2)                           */
#define SCU_GENERAL_PASSWD_PROTS_Msk          (0x4UL)                   /*!< SCU_GENERAL PASSWD: PROTS (Bitfield-Mask: 0x01)             */
#define SCU_GENERAL_PASSWD_PASS_Pos           (3UL)                     /*!< SCU_GENERAL PASSWD: PASS (Bit 3)                            */
#define SCU_GENERAL_PASSWD_PASS_Msk           (0xf8UL)                  /*!< SCU_GENERAL PASSWD: PASS (Bitfield-Mask: 0x1f)              */

/* -----------------------------  SCU_GENERAL_CCUCON  ----------------------------- */
#define SCU_GENERAL_CCUCON_GSC40_Pos          (0UL)                     /*!< SCU_GENERAL CCUCON: GSC40 (Bit 0)                           */
#define SCU_GENERAL_CCUCON_GSC40_Msk          (0x1UL)                   /*!< SCU_GENERAL CCUCON: GSC40 (Bitfield-Mask: 0x01)             */

/* -----------------------------  SCU_GENERAL_MIRRSTS  ---------------------------- */
#define SCU_GENERAL_MIRRSTS_RTC_CTR_Pos       (0UL)                     /*!< SCU_GENERAL MIRRSTS: RTC_CTR (Bit 0)                        */
#define SCU_GENERAL_MIRRSTS_RTC_CTR_Msk       (0x1UL)                   /*!< SCU_GENERAL MIRRSTS: RTC_CTR (Bitfield-Mask: 0x01)          */
#define SCU_GENERAL_MIRRSTS_RTC_ATIM0_Pos     (1UL)                     /*!< SCU_GENERAL MIRRSTS: RTC_ATIM0 (Bit 1)                      */
#define SCU_GENERAL_MIRRSTS_RTC_ATIM0_Msk     (0x2UL)                   /*!< SCU_GENERAL MIRRSTS: RTC_ATIM0 (Bitfield-Mask: 0x01)        */
#define SCU_GENERAL_MIRRSTS_RTC_ATIM1_Pos     (2UL)                     /*!< SCU_GENERAL MIRRSTS: RTC_ATIM1 (Bit 2)                      */
#define SCU_GENERAL_MIRRSTS_RTC_ATIM1_Msk     (0x4UL)                   /*!< SCU_GENERAL MIRRSTS: RTC_ATIM1 (Bitfield-Mask: 0x01)        */
#define SCU_GENERAL_MIRRSTS_RTC_TIM0_Pos      (3UL)                     /*!< SCU_GENERAL MIRRSTS: RTC_TIM0 (Bit 3)                       */
#define SCU_GENERAL_MIRRSTS_RTC_TIM0_Msk      (0x8UL)                   /*!< SCU_GENERAL MIRRSTS: RTC_TIM0 (Bitfield-Mask: 0x01)         */
#define SCU_GENERAL_MIRRSTS_RTC_TIM1_Pos      (4UL)                     /*!< SCU_GENERAL MIRRSTS: RTC_TIM1 (Bit 4)                       */
#define SCU_GENERAL_MIRRSTS_RTC_TIM1_Msk      (0x10UL)                  /*!< SCU_GENERAL MIRRSTS: RTC_TIM1 (Bitfield-Mask: 0x01)         */

/* ------------------------------  SCU_GENERAL_PMTSR  ----------------------------- */
#define SCU_GENERAL_PMTSR_MTENS_Pos           (0UL)                     /*!< SCU_GENERAL PMTSR: MTENS (Bit 0)                            */
#define SCU_GENERAL_PMTSR_MTENS_Msk           (0x1UL)                   /*!< SCU_GENERAL PMTSR: MTENS (Bitfield-Mask: 0x01)              */


/* ================================================================================ */
/* ================     struct 'SCU_INTERRUPT' Position & Mask     ================ */
/* ================================================================================ */


/* -----------------------------  SCU_INTERRUPT_SRRAW  ---------------------------- */
#define SCU_INTERRUPT_SRRAW_PRWARN_Pos        (0UL)                     /*!< SCU_INTERRUPT SRRAW: PRWARN (Bit 0)                         */
#define SCU_INTERRUPT_SRRAW_PRWARN_Msk        (0x1UL)                   /*!< SCU_INTERRUPT SRRAW: PRWARN (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRRAW_PI_Pos            (1UL)                     /*!< SCU_INTERRUPT SRRAW: PI (Bit 1)                             */
#define SCU_INTERRUPT_SRRAW_PI_Msk            (0x2UL)                   /*!< SCU_INTERRUPT SRRAW: PI (Bitfield-Mask: 0x01)               */
#define SCU_INTERRUPT_SRRAW_AI_Pos            (2UL)                     /*!< SCU_INTERRUPT SRRAW: AI (Bit 2)                             */
#define SCU_INTERRUPT_SRRAW_AI_Msk            (0x4UL)                   /*!< SCU_INTERRUPT SRRAW: AI (Bitfield-Mask: 0x01)               */
#define SCU_INTERRUPT_SRRAW_VDDPI_Pos         (3UL)                     /*!< SCU_INTERRUPT SRRAW: VDDPI (Bit 3)                          */
#define SCU_INTERRUPT_SRRAW_VDDPI_Msk         (0x8UL)                   /*!< SCU_INTERRUPT SRRAW: VDDPI (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRRAW_VDROPI_Pos        (7UL)                     /*!< SCU_INTERRUPT SRRAW: VDROPI (Bit 7)                         */
#define SCU_INTERRUPT_SRRAW_VDROPI_Msk        (0x80UL)                  /*!< SCU_INTERRUPT SRRAW: VDROPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRRAW_LOCI_Pos          (16UL)                    /*!< SCU_INTERRUPT SRRAW: LOCI (Bit 16)                          */
#define SCU_INTERRUPT_SRRAW_LOCI_Msk          (0x10000UL)               /*!< SCU_INTERRUPT SRRAW: LOCI (Bitfield-Mask: 0x01)             */
#define SCU_INTERRUPT_SRRAW_PESRAMI_Pos       (17UL)                    /*!< SCU_INTERRUPT SRRAW: PESRAMI (Bit 17)                       */
#define SCU_INTERRUPT_SRRAW_PESRAMI_Msk       (0x20000UL)               /*!< SCU_INTERRUPT SRRAW: PESRAMI (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRRAW_PEU0I_Pos         (18UL)                    /*!< SCU_INTERRUPT SRRAW: PEU0I (Bit 18)                         */
#define SCU_INTERRUPT_SRRAW_PEU0I_Msk         (0x40000UL)               /*!< SCU_INTERRUPT SRRAW: PEU0I (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRRAW_FLECC2I_Pos       (19UL)                    /*!< SCU_INTERRUPT SRRAW: FLECC2I (Bit 19)                       */
#define SCU_INTERRUPT_SRRAW_FLECC2I_Msk       (0x80000UL)               /*!< SCU_INTERRUPT SRRAW: FLECC2I (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRRAW_FLCMPLTI_Pos      (20UL)                    /*!< SCU_INTERRUPT SRRAW: FLCMPLTI (Bit 20)                      */
#define SCU_INTERRUPT_SRRAW_FLCMPLTI_Msk      (0x100000UL)              /*!< SCU_INTERRUPT SRRAW: FLCMPLTI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRRAW_VCLIPI_Pos        (21UL)                    /*!< SCU_INTERRUPT SRRAW: VCLIPI (Bit 21)                        */
#define SCU_INTERRUPT_SRRAW_VCLIPI_Msk        (0x200000UL)              /*!< SCU_INTERRUPT SRRAW: VCLIPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRRAW_SBYCLKFI_Pos      (22UL)                    /*!< SCU_INTERRUPT SRRAW: SBYCLKFI (Bit 22)                      */
#define SCU_INTERRUPT_SRRAW_SBYCLKFI_Msk      (0x400000UL)              /*!< SCU_INTERRUPT SRRAW: SBYCLKFI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRRAW_RTC_CTR_Pos       (24UL)                    /*!< SCU_INTERRUPT SRRAW: RTC_CTR (Bit 24)                       */
#define SCU_INTERRUPT_SRRAW_RTC_CTR_Msk       (0x1000000UL)             /*!< SCU_INTERRUPT SRRAW: RTC_CTR (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRRAW_RTC_ATIM0_Pos     (25UL)                    /*!< SCU_INTERRUPT SRRAW: RTC_ATIM0 (Bit 25)                     */
#define SCU_INTERRUPT_SRRAW_RTC_ATIM0_Msk     (0x2000000UL)             /*!< SCU_INTERRUPT SRRAW: RTC_ATIM0 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRRAW_RTC_ATIM1_Pos     (26UL)                    /*!< SCU_INTERRUPT SRRAW: RTC_ATIM1 (Bit 26)                     */
#define SCU_INTERRUPT_SRRAW_RTC_ATIM1_Msk     (0x4000000UL)             /*!< SCU_INTERRUPT SRRAW: RTC_ATIM1 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRRAW_RTC_TIM0_Pos      (27UL)                    /*!< SCU_INTERRUPT SRRAW: RTC_TIM0 (Bit 27)                      */
#define SCU_INTERRUPT_SRRAW_RTC_TIM0_Msk      (0x8000000UL)             /*!< SCU_INTERRUPT SRRAW: RTC_TIM0 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRRAW_RTC_TIM1_Pos      (28UL)                    /*!< SCU_INTERRUPT SRRAW: RTC_TIM1 (Bit 28)                      */
#define SCU_INTERRUPT_SRRAW_RTC_TIM1_Msk      (0x10000000UL)            /*!< SCU_INTERRUPT SRRAW: RTC_TIM1 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRRAW_TSE_DONE_Pos      (29UL)                    /*!< SCU_INTERRUPT SRRAW: TSE_DONE (Bit 29)                      */
#define SCU_INTERRUPT_SRRAW_TSE_DONE_Msk      (0x20000000UL)            /*!< SCU_INTERRUPT SRRAW: TSE_DONE (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRRAW_TSE_HIGH_Pos      (30UL)                    /*!< SCU_INTERRUPT SRRAW: TSE_HIGH (Bit 30)                      */
#define SCU_INTERRUPT_SRRAW_TSE_HIGH_Msk      (0x40000000UL)            /*!< SCU_INTERRUPT SRRAW: TSE_HIGH (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRRAW_TSE_LOW_Pos       (31UL)                    /*!< SCU_INTERRUPT SRRAW: TSE_LOW (Bit 31)                       */
#define SCU_INTERRUPT_SRRAW_TSE_LOW_Msk       (0x80000000UL)            /*!< SCU_INTERRUPT SRRAW: TSE_LOW (Bitfield-Mask: 0x01)          */

/* -----------------------------  SCU_INTERRUPT_SRMSK  ---------------------------- */
#define SCU_INTERRUPT_SRMSK_PRWARN_Pos        (0UL)                     /*!< SCU_INTERRUPT SRMSK: PRWARN (Bit 0)                         */
#define SCU_INTERRUPT_SRMSK_PRWARN_Msk        (0x1UL)                   /*!< SCU_INTERRUPT SRMSK: PRWARN (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRMSK_VDDPI_Pos         (3UL)                     /*!< SCU_INTERRUPT SRMSK: VDDPI (Bit 3)                          */
#define SCU_INTERRUPT_SRMSK_VDDPI_Msk         (0x8UL)                   /*!< SCU_INTERRUPT SRMSK: VDDPI (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRMSK_VDROPI_Pos        (7UL)                     /*!< SCU_INTERRUPT SRMSK: VDROPI (Bit 7)                         */
#define SCU_INTERRUPT_SRMSK_VDROPI_Msk        (0x80UL)                  /*!< SCU_INTERRUPT SRMSK: VDROPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRMSK_LOCI_Pos          (16UL)                    /*!< SCU_INTERRUPT SRMSK: LOCI (Bit 16)                          */
#define SCU_INTERRUPT_SRMSK_LOCI_Msk          (0x10000UL)               /*!< SCU_INTERRUPT SRMSK: LOCI (Bitfield-Mask: 0x01)             */
#define SCU_INTERRUPT_SRMSK_PESRAMI_Pos       (17UL)                    /*!< SCU_INTERRUPT SRMSK: PESRAMI (Bit 17)                       */
#define SCU_INTERRUPT_SRMSK_PESRAMI_Msk       (0x20000UL)               /*!< SCU_INTERRUPT SRMSK: PESRAMI (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRMSK_PEU0I_Pos         (18UL)                    /*!< SCU_INTERRUPT SRMSK: PEU0I (Bit 18)                         */
#define SCU_INTERRUPT_SRMSK_PEU0I_Msk         (0x40000UL)               /*!< SCU_INTERRUPT SRMSK: PEU0I (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRMSK_FLECC2I_Pos       (19UL)                    /*!< SCU_INTERRUPT SRMSK: FLECC2I (Bit 19)                       */
#define SCU_INTERRUPT_SRMSK_FLECC2I_Msk       (0x80000UL)               /*!< SCU_INTERRUPT SRMSK: FLECC2I (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRMSK_VCLIPI_Pos        (21UL)                    /*!< SCU_INTERRUPT SRMSK: VCLIPI (Bit 21)                        */
#define SCU_INTERRUPT_SRMSK_VCLIPI_Msk        (0x200000UL)              /*!< SCU_INTERRUPT SRMSK: VCLIPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRMSK_SBYCLKFI_Pos      (22UL)                    /*!< SCU_INTERRUPT SRMSK: SBYCLKFI (Bit 22)                      */
#define SCU_INTERRUPT_SRMSK_SBYCLKFI_Msk      (0x400000UL)              /*!< SCU_INTERRUPT SRMSK: SBYCLKFI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRMSK_RTC_CTR_Pos       (24UL)                    /*!< SCU_INTERRUPT SRMSK: RTC_CTR (Bit 24)                       */
#define SCU_INTERRUPT_SRMSK_RTC_CTR_Msk       (0x1000000UL)             /*!< SCU_INTERRUPT SRMSK: RTC_CTR (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRMSK_RTC_ATIM0_Pos     (25UL)                    /*!< SCU_INTERRUPT SRMSK: RTC_ATIM0 (Bit 25)                     */
#define SCU_INTERRUPT_SRMSK_RTC_ATIM0_Msk     (0x2000000UL)             /*!< SCU_INTERRUPT SRMSK: RTC_ATIM0 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRMSK_RTC_ATIM1_Pos     (26UL)                    /*!< SCU_INTERRUPT SRMSK: RTC_ATIM1 (Bit 26)                     */
#define SCU_INTERRUPT_SRMSK_RTC_ATIM1_Msk     (0x4000000UL)             /*!< SCU_INTERRUPT SRMSK: RTC_ATIM1 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRMSK_RTC_TIM0_Pos      (27UL)                    /*!< SCU_INTERRUPT SRMSK: RTC_TIM0 (Bit 27)                      */
#define SCU_INTERRUPT_SRMSK_RTC_TIM0_Msk      (0x8000000UL)             /*!< SCU_INTERRUPT SRMSK: RTC_TIM0 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRMSK_RTC_TIM1_Pos      (28UL)                    /*!< SCU_INTERRUPT SRMSK: RTC_TIM1 (Bit 28)                      */
#define SCU_INTERRUPT_SRMSK_RTC_TIM1_Msk      (0x10000000UL)            /*!< SCU_INTERRUPT SRMSK: RTC_TIM1 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRMSK_TSE_DONE_Pos      (29UL)                    /*!< SCU_INTERRUPT SRMSK: TSE_DONE (Bit 29)                      */
#define SCU_INTERRUPT_SRMSK_TSE_DONE_Msk      (0x20000000UL)            /*!< SCU_INTERRUPT SRMSK: TSE_DONE (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRMSK_TSE_HIGH_Pos      (30UL)                    /*!< SCU_INTERRUPT SRMSK: TSE_HIGH (Bit 30)                      */
#define SCU_INTERRUPT_SRMSK_TSE_HIGH_Msk      (0x40000000UL)            /*!< SCU_INTERRUPT SRMSK: TSE_HIGH (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRMSK_TSE_LOW_Pos       (31UL)                    /*!< SCU_INTERRUPT SRMSK: TSE_LOW (Bit 31)                       */
#define SCU_INTERRUPT_SRMSK_TSE_LOW_Msk       (0x80000000UL)            /*!< SCU_INTERRUPT SRMSK: TSE_LOW (Bitfield-Mask: 0x01)          */

/* -----------------------------  SCU_INTERRUPT_SRCLR  ---------------------------- */
#define SCU_INTERRUPT_SRCLR_PRWARN_Pos        (0UL)                     /*!< SCU_INTERRUPT SRCLR: PRWARN (Bit 0)                         */
#define SCU_INTERRUPT_SRCLR_PRWARN_Msk        (0x1UL)                   /*!< SCU_INTERRUPT SRCLR: PRWARN (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRCLR_PI_Pos            (1UL)                     /*!< SCU_INTERRUPT SRCLR: PI (Bit 1)                             */
#define SCU_INTERRUPT_SRCLR_PI_Msk            (0x2UL)                   /*!< SCU_INTERRUPT SRCLR: PI (Bitfield-Mask: 0x01)               */
#define SCU_INTERRUPT_SRCLR_AI_Pos            (2UL)                     /*!< SCU_INTERRUPT SRCLR: AI (Bit 2)                             */
#define SCU_INTERRUPT_SRCLR_AI_Msk            (0x4UL)                   /*!< SCU_INTERRUPT SRCLR: AI (Bitfield-Mask: 0x01)               */
#define SCU_INTERRUPT_SRCLR_VDDPI_Pos         (3UL)                     /*!< SCU_INTERRUPT SRCLR: VDDPI (Bit 3)                          */
#define SCU_INTERRUPT_SRCLR_VDDPI_Msk         (0x8UL)                   /*!< SCU_INTERRUPT SRCLR: VDDPI (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRCLR_VDROPI_Pos        (7UL)                     /*!< SCU_INTERRUPT SRCLR: VDROPI (Bit 7)                         */
#define SCU_INTERRUPT_SRCLR_VDROPI_Msk        (0x80UL)                  /*!< SCU_INTERRUPT SRCLR: VDROPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRCLR_LOCI_Pos          (16UL)                    /*!< SCU_INTERRUPT SRCLR: LOCI (Bit 16)                          */
#define SCU_INTERRUPT_SRCLR_LOCI_Msk          (0x10000UL)               /*!< SCU_INTERRUPT SRCLR: LOCI (Bitfield-Mask: 0x01)             */
#define SCU_INTERRUPT_SRCLR_PESRAMI_Pos       (17UL)                    /*!< SCU_INTERRUPT SRCLR: PESRAMI (Bit 17)                       */
#define SCU_INTERRUPT_SRCLR_PESRAMI_Msk       (0x20000UL)               /*!< SCU_INTERRUPT SRCLR: PESRAMI (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRCLR_PEU0I_Pos         (18UL)                    /*!< SCU_INTERRUPT SRCLR: PEU0I (Bit 18)                         */
#define SCU_INTERRUPT_SRCLR_PEU0I_Msk         (0x40000UL)               /*!< SCU_INTERRUPT SRCLR: PEU0I (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRCLR_FLECC2I_Pos       (19UL)                    /*!< SCU_INTERRUPT SRCLR: FLECC2I (Bit 19)                       */
#define SCU_INTERRUPT_SRCLR_FLECC2I_Msk       (0x80000UL)               /*!< SCU_INTERRUPT SRCLR: FLECC2I (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRCLR_FLCMPLTI_Pos      (20UL)                    /*!< SCU_INTERRUPT SRCLR: FLCMPLTI (Bit 20)                      */
#define SCU_INTERRUPT_SRCLR_FLCMPLTI_Msk      (0x100000UL)              /*!< SCU_INTERRUPT SRCLR: FLCMPLTI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRCLR_VCLIPI_Pos        (21UL)                    /*!< SCU_INTERRUPT SRCLR: VCLIPI (Bit 21)                        */
#define SCU_INTERRUPT_SRCLR_VCLIPI_Msk        (0x200000UL)              /*!< SCU_INTERRUPT SRCLR: VCLIPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRCLR_SBYCLKFI_Pos      (22UL)                    /*!< SCU_INTERRUPT SRCLR: SBYCLKFI (Bit 22)                      */
#define SCU_INTERRUPT_SRCLR_SBYCLKFI_Msk      (0x400000UL)              /*!< SCU_INTERRUPT SRCLR: SBYCLKFI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRCLR_RTC_CTR_Pos       (24UL)                    /*!< SCU_INTERRUPT SRCLR: RTC_CTR (Bit 24)                       */
#define SCU_INTERRUPT_SRCLR_RTC_CTR_Msk       (0x1000000UL)             /*!< SCU_INTERRUPT SRCLR: RTC_CTR (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRCLR_RTC_ATIM0_Pos     (25UL)                    /*!< SCU_INTERRUPT SRCLR: RTC_ATIM0 (Bit 25)                     */
#define SCU_INTERRUPT_SRCLR_RTC_ATIM0_Msk     (0x2000000UL)             /*!< SCU_INTERRUPT SRCLR: RTC_ATIM0 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRCLR_RTC_ATIM1_Pos     (26UL)                    /*!< SCU_INTERRUPT SRCLR: RTC_ATIM1 (Bit 26)                     */
#define SCU_INTERRUPT_SRCLR_RTC_ATIM1_Msk     (0x4000000UL)             /*!< SCU_INTERRUPT SRCLR: RTC_ATIM1 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRCLR_RTC_TIM0_Pos      (27UL)                    /*!< SCU_INTERRUPT SRCLR: RTC_TIM0 (Bit 27)                      */
#define SCU_INTERRUPT_SRCLR_RTC_TIM0_Msk      (0x8000000UL)             /*!< SCU_INTERRUPT SRCLR: RTC_TIM0 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRCLR_RTC_TIM1_Pos      (28UL)                    /*!< SCU_INTERRUPT SRCLR: RTC_TIM1 (Bit 28)                      */
#define SCU_INTERRUPT_SRCLR_RTC_TIM1_Msk      (0x10000000UL)            /*!< SCU_INTERRUPT SRCLR: RTC_TIM1 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRCLR_TSE_DONE_Pos      (29UL)                    /*!< SCU_INTERRUPT SRCLR: TSE_DONE (Bit 29)                      */
#define SCU_INTERRUPT_SRCLR_TSE_DONE_Msk      (0x20000000UL)            /*!< SCU_INTERRUPT SRCLR: TSE_DONE (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRCLR_TSE_HIGH_Pos      (30UL)                    /*!< SCU_INTERRUPT SRCLR: TSE_HIGH (Bit 30)                      */
#define SCU_INTERRUPT_SRCLR_TSE_HIGH_Msk      (0x40000000UL)            /*!< SCU_INTERRUPT SRCLR: TSE_HIGH (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRCLR_TSE_LOW_Pos       (31UL)                    /*!< SCU_INTERRUPT SRCLR: TSE_LOW (Bit 31)                       */
#define SCU_INTERRUPT_SRCLR_TSE_LOW_Msk       (0x80000000UL)            /*!< SCU_INTERRUPT SRCLR: TSE_LOW (Bitfield-Mask: 0x01)          */

/* -----------------------------  SCU_INTERRUPT_SRSET  ---------------------------- */
#define SCU_INTERRUPT_SRSET_PRWARN_Pos        (0UL)                     /*!< SCU_INTERRUPT SRSET: PRWARN (Bit 0)                         */
#define SCU_INTERRUPT_SRSET_PRWARN_Msk        (0x1UL)                   /*!< SCU_INTERRUPT SRSET: PRWARN (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRSET_PI_Pos            (1UL)                     /*!< SCU_INTERRUPT SRSET: PI (Bit 1)                             */
#define SCU_INTERRUPT_SRSET_PI_Msk            (0x2UL)                   /*!< SCU_INTERRUPT SRSET: PI (Bitfield-Mask: 0x01)               */
#define SCU_INTERRUPT_SRSET_AI_Pos            (2UL)                     /*!< SCU_INTERRUPT SRSET: AI (Bit 2)                             */
#define SCU_INTERRUPT_SRSET_AI_Msk            (0x4UL)                   /*!< SCU_INTERRUPT SRSET: AI (Bitfield-Mask: 0x01)               */
#define SCU_INTERRUPT_SRSET_VDDPI_Pos         (3UL)                     /*!< SCU_INTERRUPT SRSET: VDDPI (Bit 3)                          */
#define SCU_INTERRUPT_SRSET_VDDPI_Msk         (0x8UL)                   /*!< SCU_INTERRUPT SRSET: VDDPI (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRSET_VDROPI_Pos        (7UL)                     /*!< SCU_INTERRUPT SRSET: VDROPI (Bit 7)                         */
#define SCU_INTERRUPT_SRSET_VDROPI_Msk        (0x80UL)                  /*!< SCU_INTERRUPT SRSET: VDROPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRSET_LOCI_Pos          (16UL)                    /*!< SCU_INTERRUPT SRSET: LOCI (Bit 16)                          */
#define SCU_INTERRUPT_SRSET_LOCI_Msk          (0x10000UL)               /*!< SCU_INTERRUPT SRSET: LOCI (Bitfield-Mask: 0x01)             */
#define SCU_INTERRUPT_SRSET_PESRAMI_Pos       (17UL)                    /*!< SCU_INTERRUPT SRSET: PESRAMI (Bit 17)                       */
#define SCU_INTERRUPT_SRSET_PESRAMI_Msk       (0x20000UL)               /*!< SCU_INTERRUPT SRSET: PESRAMI (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRSET_PEU0I_Pos         (18UL)                    /*!< SCU_INTERRUPT SRSET: PEU0I (Bit 18)                         */
#define SCU_INTERRUPT_SRSET_PEU0I_Msk         (0x40000UL)               /*!< SCU_INTERRUPT SRSET: PEU0I (Bitfield-Mask: 0x01)            */
#define SCU_INTERRUPT_SRSET_FLECC2I_Pos       (19UL)                    /*!< SCU_INTERRUPT SRSET: FLECC2I (Bit 19)                       */
#define SCU_INTERRUPT_SRSET_FLECC2I_Msk       (0x80000UL)               /*!< SCU_INTERRUPT SRSET: FLECC2I (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRSET_FLCMPLTI_Pos      (20UL)                    /*!< SCU_INTERRUPT SRSET: FLCMPLTI (Bit 20)                      */
#define SCU_INTERRUPT_SRSET_FLCMPLTI_Msk      (0x100000UL)              /*!< SCU_INTERRUPT SRSET: FLCMPLTI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRSET_VCLIPI_Pos        (21UL)                    /*!< SCU_INTERRUPT SRSET: VCLIPI (Bit 21)                        */
#define SCU_INTERRUPT_SRSET_VCLIPI_Msk        (0x200000UL)              /*!< SCU_INTERRUPT SRSET: VCLIPI (Bitfield-Mask: 0x01)           */
#define SCU_INTERRUPT_SRSET_SBYCLKFI_Pos      (22UL)                    /*!< SCU_INTERRUPT SRSET: SBYCLKFI (Bit 22)                      */
#define SCU_INTERRUPT_SRSET_SBYCLKFI_Msk      (0x400000UL)              /*!< SCU_INTERRUPT SRSET: SBYCLKFI (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRSET_RTC_CTR_Pos       (24UL)                    /*!< SCU_INTERRUPT SRSET: RTC_CTR (Bit 24)                       */
#define SCU_INTERRUPT_SRSET_RTC_CTR_Msk       (0x1000000UL)             /*!< SCU_INTERRUPT SRSET: RTC_CTR (Bitfield-Mask: 0x01)          */
#define SCU_INTERRUPT_SRSET_RTC_ATIM0_Pos     (25UL)                    /*!< SCU_INTERRUPT SRSET: RTC_ATIM0 (Bit 25)                     */
#define SCU_INTERRUPT_SRSET_RTC_ATIM0_Msk     (0x2000000UL)             /*!< SCU_INTERRUPT SRSET: RTC_ATIM0 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRSET_RTC_ATIM1_Pos     (26UL)                    /*!< SCU_INTERRUPT SRSET: RTC_ATIM1 (Bit 26)                     */
#define SCU_INTERRUPT_SRSET_RTC_ATIM1_Msk     (0x4000000UL)             /*!< SCU_INTERRUPT SRSET: RTC_ATIM1 (Bitfield-Mask: 0x01)        */
#define SCU_INTERRUPT_SRSET_RTC_TIM0_Pos      (27UL)                    /*!< SCU_INTERRUPT SRSET: RTC_TIM0 (Bit 27)                      */
#define SCU_INTERRUPT_SRSET_RTC_TIM0_Msk      (0x8000000UL)             /*!< SCU_INTERRUPT SRSET: RTC_TIM0 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRSET_RTC_TIM1_Pos      (28UL)                    /*!< SCU_INTERRUPT SRSET: RTC_TIM1 (Bit 28)                      */
#define SCU_INTERRUPT_SRSET_RTC_TIM1_Msk      (0x10000000UL)            /*!< SCU_INTERRUPT SRSET: RTC_TIM1 (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRSET_TSE_DONE_Pos      (29UL)                    /*!< SCU_INTERRUPT SRSET: TSE_DONE (Bit 29)                      */
#define SCU_INTERRUPT_SRSET_TSE_DONE_Msk      (0x20000000UL)            /*!< SCU_INTERRUPT SRSET: TSE_DONE (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRSET_TSE_HIGH_Pos      (30UL)                    /*!< SCU_INTERRUPT SRSET: TSE_HIGH (Bit 30)                      */
#define SCU_INTERRUPT_SRSET_TSE_HIGH_Msk      (0x40000000UL)            /*!< SCU_INTERRUPT SRSET: TSE_HIGH (Bitfield-Mask: 0x01)         */
#define SCU_INTERRUPT_SRSET_TSE_LOW_Pos       (31UL)                    /*!< SCU_INTERRUPT SRSET: TSE_LOW (Bit 31)                       */
#define SCU_INTERRUPT_SRSET_TSE_LOW_Msk       (0x80000000UL)            /*!< SCU_INTERRUPT SRSET: TSE_LOW (Bitfield-Mask: 0x01)          */


/* ================================================================================ */
/* ================       struct 'SCU_POWER' Position & Mask       ================ */
/* ================================================================================ */


/* -------------------------------  SCU_POWER_VDESR  ------------------------------ */
#define SCU_POWER_VDESR_VCLIP_Pos             (0UL)                     /*!< SCU_POWER VDESR: VCLIP (Bit 0)                              */
#define SCU_POWER_VDESR_VCLIP_Msk             (0x1UL)                   /*!< SCU_POWER VDESR: VCLIP (Bitfield-Mask: 0x01)                */
#define SCU_POWER_VDESR_VDDPPW_Pos            (1UL)                     /*!< SCU_POWER VDESR: VDDPPW (Bit 1)                             */
#define SCU_POWER_VDESR_VDDPPW_Msk            (0x2UL)                   /*!< SCU_POWER VDESR: VDDPPW (Bitfield-Mask: 0x01)               */


/* ================================================================================ */
/* ================        struct 'SCU_CLK' Position & Mask        ================ */
/* ================================================================================ */


/* --------------------------------  SCU_CLK_CLKCR  ------------------------------- */
#define SCU_CLK_CLKCR_FDIV_Pos                (0UL)                     /*!< SCU_CLK CLKCR: FDIV (Bit 0)                                 */
#define SCU_CLK_CLKCR_FDIV_Msk                (0xffUL)                  /*!< SCU_CLK CLKCR: FDIV (Bitfield-Mask: 0xff)                   */
#define SCU_CLK_CLKCR_IDIV_Pos                (8UL)                     /*!< SCU_CLK CLKCR: IDIV (Bit 8)                                 */
#define SCU_CLK_CLKCR_IDIV_Msk                (0xff00UL)                /*!< SCU_CLK CLKCR: IDIV (Bitfield-Mask: 0xff)                   */
#define SCU_CLK_CLKCR_PCLKSEL_Pos             (16UL)                    /*!< SCU_CLK CLKCR: PCLKSEL (Bit 16)                             */
#define SCU_CLK_CLKCR_PCLKSEL_Msk             (0x10000UL)               /*!< SCU_CLK CLKCR: PCLKSEL (Bitfield-Mask: 0x01)                */
#define SCU_CLK_CLKCR_RTCCLKSEL_Pos           (17UL)                    /*!< SCU_CLK CLKCR: RTCCLKSEL (Bit 17)                           */
#define SCU_CLK_CLKCR_RTCCLKSEL_Msk           (0xe0000UL)               /*!< SCU_CLK CLKCR: RTCCLKSEL (Bitfield-Mask: 0x07)              */
#define SCU_CLK_CLKCR_CNTADJ_Pos              (20UL)                    /*!< SCU_CLK CLKCR: CNTADJ (Bit 20)                              */
#define SCU_CLK_CLKCR_CNTADJ_Msk              (0x3ff00000UL)            /*!< SCU_CLK CLKCR: CNTADJ (Bitfield-Mask: 0x3ff)                */
#define SCU_CLK_CLKCR_VDDC2LOW_Pos            (30UL)                    /*!< SCU_CLK CLKCR: VDDC2LOW (Bit 30)                            */
#define SCU_CLK_CLKCR_VDDC2LOW_Msk            (0x40000000UL)            /*!< SCU_CLK CLKCR: VDDC2LOW (Bitfield-Mask: 0x01)               */
#define SCU_CLK_CLKCR_VDDC2HIGH_Pos           (31UL)                    /*!< SCU_CLK CLKCR: VDDC2HIGH (Bit 31)                           */
#define SCU_CLK_CLKCR_VDDC2HIGH_Msk           (0x80000000UL)            /*!< SCU_CLK CLKCR: VDDC2HIGH (Bitfield-Mask: 0x01)              */

/* -------------------------------  SCU_CLK_PWRSVCR  ------------------------------ */
#define SCU_CLK_PWRSVCR_FPD_Pos               (0UL)                     /*!< SCU_CLK PWRSVCR: FPD (Bit 0)                                */
#define SCU_CLK_PWRSVCR_FPD_Msk               (0x1UL)                   /*!< SCU_CLK PWRSVCR: FPD (Bitfield-Mask: 0x01)                  */

/* ------------------------------  SCU_CLK_CGATSTAT0  ----------------------------- */
#define SCU_CLK_CGATSTAT0_VADC_Pos            (0UL)                     /*!< SCU_CLK CGATSTAT0: VADC (Bit 0)                             */
#define SCU_CLK_CGATSTAT0_VADC_Msk            (0x1UL)                   /*!< SCU_CLK CGATSTAT0: VADC (Bitfield-Mask: 0x01)               */
#define SCU_CLK_CGATSTAT0_CCU40_Pos           (2UL)                     /*!< SCU_CLK CGATSTAT0: CCU40 (Bit 2)                            */
#define SCU_CLK_CGATSTAT0_CCU40_Msk           (0x4UL)                   /*!< SCU_CLK CGATSTAT0: CCU40 (Bitfield-Mask: 0x01)              */
#define SCU_CLK_CGATSTAT0_USIC0_Pos           (3UL)                     /*!< SCU_CLK CGATSTAT0: USIC0 (Bit 3)                            */
#define SCU_CLK_CGATSTAT0_USIC0_Msk           (0x8UL)                   /*!< SCU_CLK CGATSTAT0: USIC0 (Bitfield-Mask: 0x01)              */
#define SCU_CLK_CGATSTAT0_WDT_Pos             (9UL)                     /*!< SCU_CLK CGATSTAT0: WDT (Bit 9)                              */
#define SCU_CLK_CGATSTAT0_WDT_Msk             (0x200UL)                 /*!< SCU_CLK CGATSTAT0: WDT (Bitfield-Mask: 0x01)                */
#define SCU_CLK_CGATSTAT0_RTC_Pos             (10UL)                    /*!< SCU_CLK CGATSTAT0: RTC (Bit 10)                             */
#define SCU_CLK_CGATSTAT0_RTC_Msk             (0x400UL)                 /*!< SCU_CLK CGATSTAT0: RTC (Bitfield-Mask: 0x01)                */

/* ------------------------------  SCU_CLK_CGATSET0  ------------------------------ */
#define SCU_CLK_CGATSET0_VADC_Pos             (0UL)                     /*!< SCU_CLK CGATSET0: VADC (Bit 0)                              */
#define SCU_CLK_CGATSET0_VADC_Msk             (0x1UL)                   /*!< SCU_CLK CGATSET0: VADC (Bitfield-Mask: 0x01)                */
#define SCU_CLK_CGATSET0_CCU40_Pos            (2UL)                     /*!< SCU_CLK CGATSET0: CCU40 (Bit 2)                             */
#define SCU_CLK_CGATSET0_CCU40_Msk            (0x4UL)                   /*!< SCU_CLK CGATSET0: CCU40 (Bitfield-Mask: 0x01)               */
#define SCU_CLK_CGATSET0_USIC0_Pos            (3UL)                     /*!< SCU_CLK CGATSET0: USIC0 (Bit 3)                             */
#define SCU_CLK_CGATSET0_USIC0_Msk            (0x8UL)                   /*!< SCU_CLK CGATSET0: USIC0 (Bitfield-Mask: 0x01)               */
#define SCU_CLK_CGATSET0_WDT_Pos              (9UL)                     /*!< SCU_CLK CGATSET0: WDT (Bit 9)                               */
#define SCU_CLK_CGATSET0_WDT_Msk              (0x200UL)                 /*!< SCU_CLK CGATSET0: WDT (Bitfield-Mask: 0x01)                 */
#define SCU_CLK_CGATSET0_RTC_Pos              (10UL)                    /*!< SCU_CLK CGATSET0: RTC (Bit 10)                              */
#define SCU_CLK_CGATSET0_RTC_Msk              (0x400UL)                 /*!< SCU_CLK CGATSET0: RTC (Bitfield-Mask: 0x01)                 */

/* ------------------------------  SCU_CLK_CGATCLR0  ------------------------------ */
#define SCU_CLK_CGATCLR0_VADC_Pos             (0UL)                     /*!< SCU_CLK CGATCLR0: VADC (Bit 0)                              */
#define SCU_CLK_CGATCLR0_VADC_Msk             (0x1UL)                   /*!< SCU_CLK CGATCLR0: VADC (Bitfield-Mask: 0x01)                */
#define SCU_CLK_CGATCLR0_CCU40_Pos            (2UL)                     /*!< SCU_CLK CGATCLR0: CCU40 (Bit 2)                             */
#define SCU_CLK_CGATCLR0_CCU40_Msk            (0x4UL)                   /*!< SCU_CLK CGATCLR0: CCU40 (Bitfield-Mask: 0x01)               */
#define SCU_CLK_CGATCLR0_USIC0_Pos            (3UL)                     /*!< SCU_CLK CGATCLR0: USIC0 (Bit 3)                             */
#define SCU_CLK_CGATCLR0_USIC0_Msk            (0x8UL)                   /*!< SCU_CLK CGATCLR0: USIC0 (Bitfield-Mask: 0x01)               */
#define SCU_CLK_CGATCLR0_WDT_Pos              (9UL)                     /*!< SCU_CLK CGATCLR0: WDT (Bit 9)                               */
#define SCU_CLK_CGATCLR0_WDT_Msk              (0x200UL)                 /*!< SCU_CLK CGATCLR0: WDT (Bitfield-Mask: 0x01)                 */
#define SCU_CLK_CGATCLR0_RTC_Pos              (10UL)                    /*!< SCU_CLK CGATCLR0: RTC (Bit 10)                              */
#define SCU_CLK_CGATCLR0_RTC_Msk              (0x400UL)                 /*!< SCU_CLK CGATCLR0: RTC (Bitfield-Mask: 0x01)                 */

/* -------------------------------  SCU_CLK_OSCCSR  ------------------------------- */
#define SCU_CLK_OSCCSR_OSC2L_Pos              (0UL)                     /*!< SCU_CLK OSCCSR: OSC2L (Bit 0)                               */
#define SCU_CLK_OSCCSR_OSC2L_Msk              (0x1UL)                   /*!< SCU_CLK OSCCSR: OSC2L (Bitfield-Mask: 0x01)                 */
#define SCU_CLK_OSCCSR_OSC2H_Pos              (1UL)                     /*!< SCU_CLK OSCCSR: OSC2H (Bit 1)                               */
#define SCU_CLK_OSCCSR_OSC2H_Msk              (0x2UL)                   /*!< SCU_CLK OSCCSR: OSC2H (Bitfield-Mask: 0x01)                 */
#define SCU_CLK_OSCCSR_OWDRES_Pos             (16UL)                    /*!< SCU_CLK OSCCSR: OWDRES (Bit 16)                             */
#define SCU_CLK_OSCCSR_OWDRES_Msk             (0x10000UL)               /*!< SCU_CLK OSCCSR: OWDRES (Bitfield-Mask: 0x01)                */
#define SCU_CLK_OSCCSR_OWDEN_Pos              (17UL)                    /*!< SCU_CLK OSCCSR: OWDEN (Bit 17)                              */
#define SCU_CLK_OSCCSR_OWDEN_Msk              (0x20000UL)               /*!< SCU_CLK OSCCSR: OWDEN (Bitfield-Mask: 0x01)                 */


/* ================================================================================ */
/* ================       struct 'SCU_RESET' Position & Mask       ================ */
/* ================================================================================ */


/* ------------------------------  SCU_RESET_RSTSTAT  ----------------------------- */
#define SCU_RESET_RSTSTAT_RSTSTAT_Pos         (0UL)                     /*!< SCU_RESET RSTSTAT: RSTSTAT (Bit 0)                          */
#define SCU_RESET_RSTSTAT_RSTSTAT_Msk         (0x3ffUL)                 /*!< SCU_RESET RSTSTAT: RSTSTAT (Bitfield-Mask: 0x3ff)           */
#define SCU_RESET_RSTSTAT_LCKEN_Pos           (10UL)                    /*!< SCU_RESET RSTSTAT: LCKEN (Bit 10)                           */
#define SCU_RESET_RSTSTAT_LCKEN_Msk           (0x400UL)                 /*!< SCU_RESET RSTSTAT: LCKEN (Bitfield-Mask: 0x01)              */

/* ------------------------------  SCU_RESET_RSTSET  ------------------------------ */
#define SCU_RESET_RSTSET_LCKEN_Pos            (10UL)                    /*!< SCU_RESET RSTSET: LCKEN (Bit 10)                            */
#define SCU_RESET_RSTSET_LCKEN_Msk            (0x400UL)                 /*!< SCU_RESET RSTSET: LCKEN (Bitfield-Mask: 0x01)               */

/* ------------------------------  SCU_RESET_RSTCLR  ------------------------------ */
#define SCU_RESET_RSTCLR_RSCLR_Pos            (0UL)                     /*!< SCU_RESET RSTCLR: RSCLR (Bit 0)                             */
#define SCU_RESET_RSTCLR_RSCLR_Msk            (0x1UL)                   /*!< SCU_RESET RSTCLR: RSCLR (Bitfield-Mask: 0x01)               */
#define SCU_RESET_RSTCLR_LCKEN_Pos            (10UL)                    /*!< SCU_RESET RSTCLR: LCKEN (Bit 10)                            */
#define SCU_RESET_RSTCLR_LCKEN_Msk            (0x400UL)                 /*!< SCU_RESET RSTCLR: LCKEN (Bitfield-Mask: 0x01)               */

/* ------------------------------  SCU_RESET_RSTCON  ------------------------------ */
#define SCU_RESET_RSTCON_ECCRSTEN_Pos         (0UL)                     /*!< SCU_RESET RSTCON: ECCRSTEN (Bit 0)                          */
#define SCU_RESET_RSTCON_ECCRSTEN_Msk         (0x1UL)                   /*!< SCU_RESET RSTCON: ECCRSTEN (Bitfield-Mask: 0x01)            */
#define SCU_RESET_RSTCON_LOCRSTEN_Pos         (1UL)                     /*!< SCU_RESET RSTCON: LOCRSTEN (Bit 1)                          */
#define SCU_RESET_RSTCON_LOCRSTEN_Msk         (0x2UL)                   /*!< SCU_RESET RSTCON: LOCRSTEN (Bitfield-Mask: 0x01)            */
#define SCU_RESET_RSTCON_SPERSTEN_Pos         (2UL)                     /*!< SCU_RESET RSTCON: SPERSTEN (Bit 2)                          */
#define SCU_RESET_RSTCON_SPERSTEN_Msk         (0x4UL)                   /*!< SCU_RESET RSTCON: SPERSTEN (Bitfield-Mask: 0x01)            */
#define SCU_RESET_RSTCON_U0PERSTEN_Pos        (3UL)                     /*!< SCU_RESET RSTCON: U0PERSTEN (Bit 3)                         */
#define SCU_RESET_RSTCON_U0PERSTEN_Msk        (0x8UL)                   /*!< SCU_RESET RSTCON: U0PERSTEN (Bitfield-Mask: 0x01)           */
#define SCU_RESET_RSTCON_MRSTEN_Pos           (16UL)                    /*!< SCU_RESET RSTCON: MRSTEN (Bit 16)                           */
#define SCU_RESET_RSTCON_MRSTEN_Msk           (0x10000UL)               /*!< SCU_RESET RSTCON: MRSTEN (Bitfield-Mask: 0x01)              */


/* ================================================================================ */
/* ================       struct 'SCU_ANALOG' Position & Mask      ================ */
/* ================================================================================ */


/* ----------------------------  SCU_ANALOG_ANATSECTRL  --------------------------- */
#define SCU_ANALOG_ANATSECTRL_TSE_EN_Pos      (0UL)                     /*!< SCU_ANALOG ANATSECTRL: TSE_EN (Bit 0)                       */
#define SCU_ANALOG_ANATSECTRL_TSE_EN_Msk      (0x1UL)                   /*!< SCU_ANALOG ANATSECTRL: TSE_EN (Bitfield-Mask: 0x01)         */

/* -----------------------------  SCU_ANALOG_ANATSEIH  ---------------------------- */
#define SCU_ANALOG_ANATSEIH_TSE_IH_Pos        (0UL)                     /*!< SCU_ANALOG ANATSEIH: TSE_IH (Bit 0)                         */
#define SCU_ANALOG_ANATSEIH_TSE_IH_Msk        (0xffffUL)                /*!< SCU_ANALOG ANATSEIH: TSE_IH (Bitfield-Mask: 0xffff)         */

/* -----------------------------  SCU_ANALOG_ANATSEIL  ---------------------------- */
#define SCU_ANALOG_ANATSEIL_TSE_IL_Pos        (0UL)                     /*!< SCU_ANALOG ANATSEIL: TSE_IL (Bit 0)                         */
#define SCU_ANALOG_ANATSEIL_TSE_IL_Msk        (0xffffUL)                /*!< SCU_ANALOG ANATSEIL: TSE_IL (Bitfield-Mask: 0xffff)         */

/* ----------------------------  SCU_ANALOG_ANATSEMON  ---------------------------- */
#define SCU_ANALOG_ANATSEMON_TSE_MON_Pos      (0UL)                     /*!< SCU_ANALOG ANATSEMON: TSE_MON (Bit 0)                       */
#define SCU_ANALOG_ANATSEMON_TSE_MON_Msk      (0xffffUL)                /*!< SCU_ANALOG ANATSEMON: TSE_MON (Bitfield-Mask: 0xffff)       */

/* -----------------------------  SCU_ANALOG_ANAVDEL  ----------------------------- */
#define SCU_ANALOG_ANAVDEL_VDEL_SELECT_Pos    (0UL)                     /*!< SCU_ANALOG ANAVDEL: VDEL_SELECT (Bit 0)                     */
#define SCU_ANALOG_ANAVDEL_VDEL_SELECT_Msk    (0x3UL)                   /*!< SCU_ANALOG ANAVDEL: VDEL_SELECT (Bitfield-Mask: 0x03)       */
#define SCU_ANALOG_ANAVDEL_VDEL_TIM_ADJ_Pos   (2UL)                     /*!< SCU_ANALOG ANAVDEL: VDEL_TIM_ADJ (Bit 2)                    */
#define SCU_ANALOG_ANAVDEL_VDEL_TIM_ADJ_Msk   (0xcUL)                   /*!< SCU_ANALOG ANAVDEL: VDEL_TIM_ADJ (Bitfield-Mask: 0x03)      */
#define SCU_ANALOG_ANAVDEL_VDEL_EN_Pos        (4UL)                     /*!< SCU_ANALOG ANAVDEL: VDEL_EN (Bit 4)                         */
#define SCU_ANALOG_ANAVDEL_VDEL_EN_Msk        (0x10UL)                  /*!< SCU_ANALOG ANAVDEL: VDEL_EN (Bitfield-Mask: 0x01)           */

/* ----------------------------  SCU_ANALOG_ANAOFFSET  ---------------------------- */
#define SCU_ANALOG_ANAOFFSET_ADJL_OFFSET_Pos  (0UL)                     /*!< SCU_ANALOG ANAOFFSET: ADJL_OFFSET (Bit 0)                   */
#define SCU_ANALOG_ANAOFFSET_ADJL_OFFSET_Msk  (0xfUL)                   /*!< SCU_ANALOG ANAOFFSET: ADJL_OFFSET (Bitfield-Mask: 0x0f)     */


/* ================================================================================ */
/* ================          Group 'CCU4' Position & Mask          ================ */
/* ================================================================================ */


/* ---------------------------------  CCU4_GCTRL  --------------------------------- */
#define CCU4_GCTRL_PRBC_Pos                   (0UL)                     /*!< CCU4 GCTRL: PRBC (Bit 0)                                    */
#define CCU4_GCTRL_PRBC_Msk                   (0x7UL)                   /*!< CCU4 GCTRL: PRBC (Bitfield-Mask: 0x07)                      */
#define CCU4_GCTRL_PCIS_Pos                   (4UL)                     /*!< CCU4 GCTRL: PCIS (Bit 4)                                    */
#define CCU4_GCTRL_PCIS_Msk                   (0x30UL)                  /*!< CCU4 GCTRL: PCIS (Bitfield-Mask: 0x03)                      */
#define CCU4_GCTRL_SUSCFG_Pos                 (8UL)                     /*!< CCU4 GCTRL: SUSCFG (Bit 8)                                  */
#define CCU4_GCTRL_SUSCFG_Msk                 (0x300UL)                 /*!< CCU4 GCTRL: SUSCFG (Bitfield-Mask: 0x03)                    */
#define CCU4_GCTRL_MSE0_Pos                   (10UL)                    /*!< CCU4 GCTRL: MSE0 (Bit 10)                                   */
#define CCU4_GCTRL_MSE0_Msk                   (0x400UL)                 /*!< CCU4 GCTRL: MSE0 (Bitfield-Mask: 0x01)                      */
#define CCU4_GCTRL_MSE1_Pos                   (11UL)                    /*!< CCU4 GCTRL: MSE1 (Bit 11)                                   */
#define CCU4_GCTRL_MSE1_Msk                   (0x800UL)                 /*!< CCU4 GCTRL: MSE1 (Bitfield-Mask: 0x01)                      */
#define CCU4_GCTRL_MSE2_Pos                   (12UL)                    /*!< CCU4 GCTRL: MSE2 (Bit 12)                                   */
#define CCU4_GCTRL_MSE2_Msk                   (0x1000UL)                /*!< CCU4 GCTRL: MSE2 (Bitfield-Mask: 0x01)                      */
#define CCU4_GCTRL_MSE3_Pos                   (13UL)                    /*!< CCU4 GCTRL: MSE3 (Bit 13)                                   */
#define CCU4_GCTRL_MSE3_Msk                   (0x2000UL)                /*!< CCU4 GCTRL: MSE3 (Bitfield-Mask: 0x01)                      */
#define CCU4_GCTRL_MSDE_Pos                   (14UL)                    /*!< CCU4 GCTRL: MSDE (Bit 14)                                   */
#define CCU4_GCTRL_MSDE_Msk                   (0xc000UL)                /*!< CCU4 GCTRL: MSDE (Bitfield-Mask: 0x03)                      */

/* ---------------------------------  CCU4_GSTAT  --------------------------------- */
#define CCU4_GSTAT_S0I_Pos                    (0UL)                     /*!< CCU4 GSTAT: S0I (Bit 0)                                     */
#define CCU4_GSTAT_S0I_Msk                    (0x1UL)                   /*!< CCU4 GSTAT: S0I (Bitfield-Mask: 0x01)                       */
#define CCU4_GSTAT_S1I_Pos                    (1UL)                     /*!< CCU4 GSTAT: S1I (Bit 1)                                     */
#define CCU4_GSTAT_S1I_Msk                    (0x2UL)                   /*!< CCU4 GSTAT: S1I (Bitfield-Mask: 0x01)                       */
#define CCU4_GSTAT_S2I_Pos                    (2UL)                     /*!< CCU4 GSTAT: S2I (Bit 2)                                     */
#define CCU4_GSTAT_S2I_Msk                    (0x4UL)                   /*!< CCU4 GSTAT: S2I (Bitfield-Mask: 0x01)                       */
#define CCU4_GSTAT_S3I_Pos                    (3UL)                     /*!< CCU4 GSTAT: S3I (Bit 3)                                     */
#define CCU4_GSTAT_S3I_Msk                    (0x8UL)                   /*!< CCU4 GSTAT: S3I (Bitfield-Mask: 0x01)                       */
#define CCU4_GSTAT_PRB_Pos                    (8UL)                     /*!< CCU4 GSTAT: PRB (Bit 8)                                     */
#define CCU4_GSTAT_PRB_Msk                    (0x100UL)                 /*!< CCU4 GSTAT: PRB (Bitfield-Mask: 0x01)                       */

/* ---------------------------------  CCU4_GIDLS  --------------------------------- */
#define CCU4_GIDLS_SS0I_Pos                   (0UL)                     /*!< CCU4 GIDLS: SS0I (Bit 0)                                    */
#define CCU4_GIDLS_SS0I_Msk                   (0x1UL)                   /*!< CCU4 GIDLS: SS0I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLS_SS1I_Pos                   (1UL)                     /*!< CCU4 GIDLS: SS1I (Bit 1)                                    */
#define CCU4_GIDLS_SS1I_Msk                   (0x2UL)                   /*!< CCU4 GIDLS: SS1I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLS_SS2I_Pos                   (2UL)                     /*!< CCU4 GIDLS: SS2I (Bit 2)                                    */
#define CCU4_GIDLS_SS2I_Msk                   (0x4UL)                   /*!< CCU4 GIDLS: SS2I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLS_SS3I_Pos                   (3UL)                     /*!< CCU4 GIDLS: SS3I (Bit 3)                                    */
#define CCU4_GIDLS_SS3I_Msk                   (0x8UL)                   /*!< CCU4 GIDLS: SS3I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLS_CPRB_Pos                   (8UL)                     /*!< CCU4 GIDLS: CPRB (Bit 8)                                    */
#define CCU4_GIDLS_CPRB_Msk                   (0x100UL)                 /*!< CCU4 GIDLS: CPRB (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLS_PSIC_Pos                   (9UL)                     /*!< CCU4 GIDLS: PSIC (Bit 9)                                    */
#define CCU4_GIDLS_PSIC_Msk                   (0x200UL)                 /*!< CCU4 GIDLS: PSIC (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  CCU4_GIDLC  --------------------------------- */
#define CCU4_GIDLC_CS0I_Pos                   (0UL)                     /*!< CCU4 GIDLC: CS0I (Bit 0)                                    */
#define CCU4_GIDLC_CS0I_Msk                   (0x1UL)                   /*!< CCU4 GIDLC: CS0I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLC_CS1I_Pos                   (1UL)                     /*!< CCU4 GIDLC: CS1I (Bit 1)                                    */
#define CCU4_GIDLC_CS1I_Msk                   (0x2UL)                   /*!< CCU4 GIDLC: CS1I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLC_CS2I_Pos                   (2UL)                     /*!< CCU4 GIDLC: CS2I (Bit 2)                                    */
#define CCU4_GIDLC_CS2I_Msk                   (0x4UL)                   /*!< CCU4 GIDLC: CS2I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLC_CS3I_Pos                   (3UL)                     /*!< CCU4 GIDLC: CS3I (Bit 3)                                    */
#define CCU4_GIDLC_CS3I_Msk                   (0x8UL)                   /*!< CCU4 GIDLC: CS3I (Bitfield-Mask: 0x01)                      */
#define CCU4_GIDLC_SPRB_Pos                   (8UL)                     /*!< CCU4 GIDLC: SPRB (Bit 8)                                    */
#define CCU4_GIDLC_SPRB_Msk                   (0x100UL)                 /*!< CCU4 GIDLC: SPRB (Bitfield-Mask: 0x01)                      */

/* ----------------------------------  CCU4_GCSS  --------------------------------- */
#define CCU4_GCSS_S0SE_Pos                    (0UL)                     /*!< CCU4 GCSS: S0SE (Bit 0)                                     */
#define CCU4_GCSS_S0SE_Msk                    (0x1UL)                   /*!< CCU4 GCSS: S0SE (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSS_S0DSE_Pos                   (1UL)                     /*!< CCU4 GCSS: S0DSE (Bit 1)                                    */
#define CCU4_GCSS_S0DSE_Msk                   (0x2UL)                   /*!< CCU4 GCSS: S0DSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S0PSE_Pos                   (2UL)                     /*!< CCU4 GCSS: S0PSE (Bit 2)                                    */
#define CCU4_GCSS_S0PSE_Msk                   (0x4UL)                   /*!< CCU4 GCSS: S0PSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S1SE_Pos                    (4UL)                     /*!< CCU4 GCSS: S1SE (Bit 4)                                     */
#define CCU4_GCSS_S1SE_Msk                    (0x10UL)                  /*!< CCU4 GCSS: S1SE (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSS_S1DSE_Pos                   (5UL)                     /*!< CCU4 GCSS: S1DSE (Bit 5)                                    */
#define CCU4_GCSS_S1DSE_Msk                   (0x20UL)                  /*!< CCU4 GCSS: S1DSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S1PSE_Pos                   (6UL)                     /*!< CCU4 GCSS: S1PSE (Bit 6)                                    */
#define CCU4_GCSS_S1PSE_Msk                   (0x40UL)                  /*!< CCU4 GCSS: S1PSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S2SE_Pos                    (8UL)                     /*!< CCU4 GCSS: S2SE (Bit 8)                                     */
#define CCU4_GCSS_S2SE_Msk                    (0x100UL)                 /*!< CCU4 GCSS: S2SE (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSS_S2DSE_Pos                   (9UL)                     /*!< CCU4 GCSS: S2DSE (Bit 9)                                    */
#define CCU4_GCSS_S2DSE_Msk                   (0x200UL)                 /*!< CCU4 GCSS: S2DSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S2PSE_Pos                   (10UL)                    /*!< CCU4 GCSS: S2PSE (Bit 10)                                   */
#define CCU4_GCSS_S2PSE_Msk                   (0x400UL)                 /*!< CCU4 GCSS: S2PSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S3SE_Pos                    (12UL)                    /*!< CCU4 GCSS: S3SE (Bit 12)                                    */
#define CCU4_GCSS_S3SE_Msk                    (0x1000UL)                /*!< CCU4 GCSS: S3SE (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSS_S3DSE_Pos                   (13UL)                    /*!< CCU4 GCSS: S3DSE (Bit 13)                                   */
#define CCU4_GCSS_S3DSE_Msk                   (0x2000UL)                /*!< CCU4 GCSS: S3DSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S3PSE_Pos                   (14UL)                    /*!< CCU4 GCSS: S3PSE (Bit 14)                                   */
#define CCU4_GCSS_S3PSE_Msk                   (0x4000UL)                /*!< CCU4 GCSS: S3PSE (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S0STS_Pos                   (16UL)                    /*!< CCU4 GCSS: S0STS (Bit 16)                                   */
#define CCU4_GCSS_S0STS_Msk                   (0x10000UL)               /*!< CCU4 GCSS: S0STS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S1STS_Pos                   (17UL)                    /*!< CCU4 GCSS: S1STS (Bit 17)                                   */
#define CCU4_GCSS_S1STS_Msk                   (0x20000UL)               /*!< CCU4 GCSS: S1STS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S2STS_Pos                   (18UL)                    /*!< CCU4 GCSS: S2STS (Bit 18)                                   */
#define CCU4_GCSS_S2STS_Msk                   (0x40000UL)               /*!< CCU4 GCSS: S2STS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSS_S3STS_Pos                   (19UL)                    /*!< CCU4 GCSS: S3STS (Bit 19)                                   */
#define CCU4_GCSS_S3STS_Msk                   (0x80000UL)               /*!< CCU4 GCSS: S3STS (Bitfield-Mask: 0x01)                      */

/* ----------------------------------  CCU4_GCSC  --------------------------------- */
#define CCU4_GCSC_S0SC_Pos                    (0UL)                     /*!< CCU4 GCSC: S0SC (Bit 0)                                     */
#define CCU4_GCSC_S0SC_Msk                    (0x1UL)                   /*!< CCU4 GCSC: S0SC (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSC_S0DSC_Pos                   (1UL)                     /*!< CCU4 GCSC: S0DSC (Bit 1)                                    */
#define CCU4_GCSC_S0DSC_Msk                   (0x2UL)                   /*!< CCU4 GCSC: S0DSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S0PSC_Pos                   (2UL)                     /*!< CCU4 GCSC: S0PSC (Bit 2)                                    */
#define CCU4_GCSC_S0PSC_Msk                   (0x4UL)                   /*!< CCU4 GCSC: S0PSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S1SC_Pos                    (4UL)                     /*!< CCU4 GCSC: S1SC (Bit 4)                                     */
#define CCU4_GCSC_S1SC_Msk                    (0x10UL)                  /*!< CCU4 GCSC: S1SC (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSC_S1DSC_Pos                   (5UL)                     /*!< CCU4 GCSC: S1DSC (Bit 5)                                    */
#define CCU4_GCSC_S1DSC_Msk                   (0x20UL)                  /*!< CCU4 GCSC: S1DSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S1PSC_Pos                   (6UL)                     /*!< CCU4 GCSC: S1PSC (Bit 6)                                    */
#define CCU4_GCSC_S1PSC_Msk                   (0x40UL)                  /*!< CCU4 GCSC: S1PSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S2SC_Pos                    (8UL)                     /*!< CCU4 GCSC: S2SC (Bit 8)                                     */
#define CCU4_GCSC_S2SC_Msk                    (0x100UL)                 /*!< CCU4 GCSC: S2SC (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSC_S2DSC_Pos                   (9UL)                     /*!< CCU4 GCSC: S2DSC (Bit 9)                                    */
#define CCU4_GCSC_S2DSC_Msk                   (0x200UL)                 /*!< CCU4 GCSC: S2DSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S2PSC_Pos                   (10UL)                    /*!< CCU4 GCSC: S2PSC (Bit 10)                                   */
#define CCU4_GCSC_S2PSC_Msk                   (0x400UL)                 /*!< CCU4 GCSC: S2PSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S3SC_Pos                    (12UL)                    /*!< CCU4 GCSC: S3SC (Bit 12)                                    */
#define CCU4_GCSC_S3SC_Msk                    (0x1000UL)                /*!< CCU4 GCSC: S3SC (Bitfield-Mask: 0x01)                       */
#define CCU4_GCSC_S3DSC_Pos                   (13UL)                    /*!< CCU4 GCSC: S3DSC (Bit 13)                                   */
#define CCU4_GCSC_S3DSC_Msk                   (0x2000UL)                /*!< CCU4 GCSC: S3DSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S3PSC_Pos                   (14UL)                    /*!< CCU4 GCSC: S3PSC (Bit 14)                                   */
#define CCU4_GCSC_S3PSC_Msk                   (0x4000UL)                /*!< CCU4 GCSC: S3PSC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S0STC_Pos                   (16UL)                    /*!< CCU4 GCSC: S0STC (Bit 16)                                   */
#define CCU4_GCSC_S0STC_Msk                   (0x10000UL)               /*!< CCU4 GCSC: S0STC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S1STC_Pos                   (17UL)                    /*!< CCU4 GCSC: S1STC (Bit 17)                                   */
#define CCU4_GCSC_S1STC_Msk                   (0x20000UL)               /*!< CCU4 GCSC: S1STC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S2STC_Pos                   (18UL)                    /*!< CCU4 GCSC: S2STC (Bit 18)                                   */
#define CCU4_GCSC_S2STC_Msk                   (0x40000UL)               /*!< CCU4 GCSC: S2STC (Bitfield-Mask: 0x01)                      */
#define CCU4_GCSC_S3STC_Pos                   (19UL)                    /*!< CCU4 GCSC: S3STC (Bit 19)                                   */
#define CCU4_GCSC_S3STC_Msk                   (0x80000UL)               /*!< CCU4 GCSC: S3STC (Bitfield-Mask: 0x01)                      */

/* ----------------------------------  CCU4_GCST  --------------------------------- */
#define CCU4_GCST_S0SS_Pos                    (0UL)                     /*!< CCU4 GCST: S0SS (Bit 0)                                     */
#define CCU4_GCST_S0SS_Msk                    (0x1UL)                   /*!< CCU4 GCST: S0SS (Bitfield-Mask: 0x01)                       */
#define CCU4_GCST_S0DSS_Pos                   (1UL)                     /*!< CCU4 GCST: S0DSS (Bit 1)                                    */
#define CCU4_GCST_S0DSS_Msk                   (0x2UL)                   /*!< CCU4 GCST: S0DSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S0PSS_Pos                   (2UL)                     /*!< CCU4 GCST: S0PSS (Bit 2)                                    */
#define CCU4_GCST_S0PSS_Msk                   (0x4UL)                   /*!< CCU4 GCST: S0PSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S1SS_Pos                    (4UL)                     /*!< CCU4 GCST: S1SS (Bit 4)                                     */
#define CCU4_GCST_S1SS_Msk                    (0x10UL)                  /*!< CCU4 GCST: S1SS (Bitfield-Mask: 0x01)                       */
#define CCU4_GCST_S1DSS_Pos                   (5UL)                     /*!< CCU4 GCST: S1DSS (Bit 5)                                    */
#define CCU4_GCST_S1DSS_Msk                   (0x20UL)                  /*!< CCU4 GCST: S1DSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S1PSS_Pos                   (6UL)                     /*!< CCU4 GCST: S1PSS (Bit 6)                                    */
#define CCU4_GCST_S1PSS_Msk                   (0x40UL)                  /*!< CCU4 GCST: S1PSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S2SS_Pos                    (8UL)                     /*!< CCU4 GCST: S2SS (Bit 8)                                     */
#define CCU4_GCST_S2SS_Msk                    (0x100UL)                 /*!< CCU4 GCST: S2SS (Bitfield-Mask: 0x01)                       */
#define CCU4_GCST_S2DSS_Pos                   (9UL)                     /*!< CCU4 GCST: S2DSS (Bit 9)                                    */
#define CCU4_GCST_S2DSS_Msk                   (0x200UL)                 /*!< CCU4 GCST: S2DSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S2PSS_Pos                   (10UL)                    /*!< CCU4 GCST: S2PSS (Bit 10)                                   */
#define CCU4_GCST_S2PSS_Msk                   (0x400UL)                 /*!< CCU4 GCST: S2PSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S3SS_Pos                    (12UL)                    /*!< CCU4 GCST: S3SS (Bit 12)                                    */
#define CCU4_GCST_S3SS_Msk                    (0x1000UL)                /*!< CCU4 GCST: S3SS (Bitfield-Mask: 0x01)                       */
#define CCU4_GCST_S3DSS_Pos                   (13UL)                    /*!< CCU4 GCST: S3DSS (Bit 13)                                   */
#define CCU4_GCST_S3DSS_Msk                   (0x2000UL)                /*!< CCU4 GCST: S3DSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_S3PSS_Pos                   (14UL)                    /*!< CCU4 GCST: S3PSS (Bit 14)                                   */
#define CCU4_GCST_S3PSS_Msk                   (0x4000UL)                /*!< CCU4 GCST: S3PSS (Bitfield-Mask: 0x01)                      */
#define CCU4_GCST_CC40ST_Pos                  (16UL)                    /*!< CCU4 GCST: CC40ST (Bit 16)                                  */
#define CCU4_GCST_CC40ST_Msk                  (0x10000UL)               /*!< CCU4 GCST: CC40ST (Bitfield-Mask: 0x01)                     */
#define CCU4_GCST_CC41ST_Pos                  (17UL)                    /*!< CCU4 GCST: CC41ST (Bit 17)                                  */
#define CCU4_GCST_CC41ST_Msk                  (0x20000UL)               /*!< CCU4 GCST: CC41ST (Bitfield-Mask: 0x01)                     */
#define CCU4_GCST_CC42ST_Pos                  (18UL)                    /*!< CCU4 GCST: CC42ST (Bit 18)                                  */
#define CCU4_GCST_CC42ST_Msk                  (0x40000UL)               /*!< CCU4 GCST: CC42ST (Bitfield-Mask: 0x01)                     */
#define CCU4_GCST_CC43ST_Pos                  (19UL)                    /*!< CCU4 GCST: CC43ST (Bit 19)                                  */
#define CCU4_GCST_CC43ST_Msk                  (0x80000UL)               /*!< CCU4 GCST: CC43ST (Bitfield-Mask: 0x01)                     */

/* ----------------------------------  CCU4_MIDR  --------------------------------- */
#define CCU4_MIDR_MODR_Pos                    (0UL)                     /*!< CCU4 MIDR: MODR (Bit 0)                                     */
#define CCU4_MIDR_MODR_Msk                    (0xffUL)                  /*!< CCU4 MIDR: MODR (Bitfield-Mask: 0xff)                       */
#define CCU4_MIDR_MODT_Pos                    (8UL)                     /*!< CCU4 MIDR: MODT (Bit 8)                                     */
#define CCU4_MIDR_MODT_Msk                    (0xff00UL)                /*!< CCU4 MIDR: MODT (Bitfield-Mask: 0xff)                       */
#define CCU4_MIDR_MODN_Pos                    (16UL)                    /*!< CCU4 MIDR: MODN (Bit 16)                                    */
#define CCU4_MIDR_MODN_Msk                    (0xffff0000UL)            /*!< CCU4 MIDR: MODN (Bitfield-Mask: 0xffff)                     */


/* ================================================================================ */
/* ================        Group 'CCU4_CC4' Position & Mask        ================ */
/* ================================================================================ */


/* --------------------------------  CCU4_CC4_INS  -------------------------------- */
#define CCU4_CC4_INS_EV0IS_Pos                (0UL)                     /*!< CCU4_CC4 INS: EV0IS (Bit 0)                                 */
#define CCU4_CC4_INS_EV0IS_Msk                (0xfUL)                   /*!< CCU4_CC4 INS: EV0IS (Bitfield-Mask: 0x0f)                   */
#define CCU4_CC4_INS_EV1IS_Pos                (4UL)                     /*!< CCU4_CC4 INS: EV1IS (Bit 4)                                 */
#define CCU4_CC4_INS_EV1IS_Msk                (0xf0UL)                  /*!< CCU4_CC4 INS: EV1IS (Bitfield-Mask: 0x0f)                   */
#define CCU4_CC4_INS_EV2IS_Pos                (8UL)                     /*!< CCU4_CC4 INS: EV2IS (Bit 8)                                 */
#define CCU4_CC4_INS_EV2IS_Msk                (0xf00UL)                 /*!< CCU4_CC4 INS: EV2IS (Bitfield-Mask: 0x0f)                   */
#define CCU4_CC4_INS_EV0EM_Pos                (16UL)                    /*!< CCU4_CC4 INS: EV0EM (Bit 16)                                */
#define CCU4_CC4_INS_EV0EM_Msk                (0x30000UL)               /*!< CCU4_CC4 INS: EV0EM (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_INS_EV1EM_Pos                (18UL)                    /*!< CCU4_CC4 INS: EV1EM (Bit 18)                                */
#define CCU4_CC4_INS_EV1EM_Msk                (0xc0000UL)               /*!< CCU4_CC4 INS: EV1EM (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_INS_EV2EM_Pos                (20UL)                    /*!< CCU4_CC4 INS: EV2EM (Bit 20)                                */
#define CCU4_CC4_INS_EV2EM_Msk                (0x300000UL)              /*!< CCU4_CC4 INS: EV2EM (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_INS_EV0LM_Pos                (22UL)                    /*!< CCU4_CC4 INS: EV0LM (Bit 22)                                */
#define CCU4_CC4_INS_EV0LM_Msk                (0x400000UL)              /*!< CCU4_CC4 INS: EV0LM (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INS_EV1LM_Pos                (23UL)                    /*!< CCU4_CC4 INS: EV1LM (Bit 23)                                */
#define CCU4_CC4_INS_EV1LM_Msk                (0x800000UL)              /*!< CCU4_CC4 INS: EV1LM (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INS_EV2LM_Pos                (24UL)                    /*!< CCU4_CC4 INS: EV2LM (Bit 24)                                */
#define CCU4_CC4_INS_EV2LM_Msk                (0x1000000UL)             /*!< CCU4_CC4 INS: EV2LM (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INS_LPF0M_Pos                (25UL)                    /*!< CCU4_CC4 INS: LPF0M (Bit 25)                                */
#define CCU4_CC4_INS_LPF0M_Msk                (0x6000000UL)             /*!< CCU4_CC4 INS: LPF0M (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_INS_LPF1M_Pos                (27UL)                    /*!< CCU4_CC4 INS: LPF1M (Bit 27)                                */
#define CCU4_CC4_INS_LPF1M_Msk                (0x18000000UL)            /*!< CCU4_CC4 INS: LPF1M (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_INS_LPF2M_Pos                (29UL)                    /*!< CCU4_CC4 INS: LPF2M (Bit 29)                                */
#define CCU4_CC4_INS_LPF2M_Msk                (0x60000000UL)            /*!< CCU4_CC4 INS: LPF2M (Bitfield-Mask: 0x03)                   */

/* --------------------------------  CCU4_CC4_CMC  -------------------------------- */
#define CCU4_CC4_CMC_STRTS_Pos                (0UL)                     /*!< CCU4_CC4 CMC: STRTS (Bit 0)                                 */
#define CCU4_CC4_CMC_STRTS_Msk                (0x3UL)                   /*!< CCU4_CC4 CMC: STRTS (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_CMC_ENDS_Pos                 (2UL)                     /*!< CCU4_CC4 CMC: ENDS (Bit 2)                                  */
#define CCU4_CC4_CMC_ENDS_Msk                 (0xcUL)                   /*!< CCU4_CC4 CMC: ENDS (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_CMC_CAP0S_Pos                (4UL)                     /*!< CCU4_CC4 CMC: CAP0S (Bit 4)                                 */
#define CCU4_CC4_CMC_CAP0S_Msk                (0x30UL)                  /*!< CCU4_CC4 CMC: CAP0S (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_CMC_CAP1S_Pos                (6UL)                     /*!< CCU4_CC4 CMC: CAP1S (Bit 6)                                 */
#define CCU4_CC4_CMC_CAP1S_Msk                (0xc0UL)                  /*!< CCU4_CC4 CMC: CAP1S (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_CMC_GATES_Pos                (8UL)                     /*!< CCU4_CC4 CMC: GATES (Bit 8)                                 */
#define CCU4_CC4_CMC_GATES_Msk                (0x300UL)                 /*!< CCU4_CC4 CMC: GATES (Bitfield-Mask: 0x03)                   */
#define CCU4_CC4_CMC_UDS_Pos                  (10UL)                    /*!< CCU4_CC4 CMC: UDS (Bit 10)                                  */
#define CCU4_CC4_CMC_UDS_Msk                  (0xc00UL)                 /*!< CCU4_CC4 CMC: UDS (Bitfield-Mask: 0x03)                     */
#define CCU4_CC4_CMC_LDS_Pos                  (12UL)                    /*!< CCU4_CC4 CMC: LDS (Bit 12)                                  */
#define CCU4_CC4_CMC_LDS_Msk                  (0x3000UL)                /*!< CCU4_CC4 CMC: LDS (Bitfield-Mask: 0x03)                     */
#define CCU4_CC4_CMC_CNTS_Pos                 (14UL)                    /*!< CCU4_CC4 CMC: CNTS (Bit 14)                                 */
#define CCU4_CC4_CMC_CNTS_Msk                 (0xc000UL)                /*!< CCU4_CC4 CMC: CNTS (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_CMC_OFS_Pos                  (16UL)                    /*!< CCU4_CC4 CMC: OFS (Bit 16)                                  */
#define CCU4_CC4_CMC_OFS_Msk                  (0x10000UL)               /*!< CCU4_CC4 CMC: OFS (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_CMC_TS_Pos                   (17UL)                    /*!< CCU4_CC4 CMC: TS (Bit 17)                                   */
#define CCU4_CC4_CMC_TS_Msk                   (0x20000UL)               /*!< CCU4_CC4 CMC: TS (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_CMC_MOS_Pos                  (18UL)                    /*!< CCU4_CC4 CMC: MOS (Bit 18)                                  */
#define CCU4_CC4_CMC_MOS_Msk                  (0xc0000UL)               /*!< CCU4_CC4 CMC: MOS (Bitfield-Mask: 0x03)                     */
#define CCU4_CC4_CMC_TCE_Pos                  (20UL)                    /*!< CCU4_CC4 CMC: TCE (Bit 20)                                  */
#define CCU4_CC4_CMC_TCE_Msk                  (0x100000UL)              /*!< CCU4_CC4 CMC: TCE (Bitfield-Mask: 0x01)                     */

/* --------------------------------  CCU4_CC4_TCST  ------------------------------- */
#define CCU4_CC4_TCST_TRB_Pos                 (0UL)                     /*!< CCU4_CC4 TCST: TRB (Bit 0)                                  */
#define CCU4_CC4_TCST_TRB_Msk                 (0x1UL)                   /*!< CCU4_CC4 TCST: TRB (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_TCST_CDIR_Pos                (1UL)                     /*!< CCU4_CC4 TCST: CDIR (Bit 1)                                 */
#define CCU4_CC4_TCST_CDIR_Msk                (0x2UL)                   /*!< CCU4_CC4 TCST: CDIR (Bitfield-Mask: 0x01)                   */

/* -------------------------------  CCU4_CC4_TCSET  ------------------------------- */
#define CCU4_CC4_TCSET_TRBS_Pos               (0UL)                     /*!< CCU4_CC4 TCSET: TRBS (Bit 0)                                */
#define CCU4_CC4_TCSET_TRBS_Msk               (0x1UL)                   /*!< CCU4_CC4 TCSET: TRBS (Bitfield-Mask: 0x01)                  */

/* -------------------------------  CCU4_CC4_TCCLR  ------------------------------- */
#define CCU4_CC4_TCCLR_TRBC_Pos               (0UL)                     /*!< CCU4_CC4 TCCLR: TRBC (Bit 0)                                */
#define CCU4_CC4_TCCLR_TRBC_Msk               (0x1UL)                   /*!< CCU4_CC4 TCCLR: TRBC (Bitfield-Mask: 0x01)                  */
#define CCU4_CC4_TCCLR_TCC_Pos                (1UL)                     /*!< CCU4_CC4 TCCLR: TCC (Bit 1)                                 */
#define CCU4_CC4_TCCLR_TCC_Msk                (0x2UL)                   /*!< CCU4_CC4 TCCLR: TCC (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_TCCLR_DITC_Pos               (2UL)                     /*!< CCU4_CC4 TCCLR: DITC (Bit 2)                                */
#define CCU4_CC4_TCCLR_DITC_Msk               (0x4UL)                   /*!< CCU4_CC4 TCCLR: DITC (Bitfield-Mask: 0x01)                  */

/* ---------------------------------  CCU4_CC4_TC  -------------------------------- */
#define CCU4_CC4_TC_TCM_Pos                   (0UL)                     /*!< CCU4_CC4 TC: TCM (Bit 0)                                    */
#define CCU4_CC4_TC_TCM_Msk                   (0x1UL)                   /*!< CCU4_CC4 TC: TCM (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_TSSM_Pos                  (1UL)                     /*!< CCU4_CC4 TC: TSSM (Bit 1)                                   */
#define CCU4_CC4_TC_TSSM_Msk                  (0x2UL)                   /*!< CCU4_CC4 TC: TSSM (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_TC_CLST_Pos                  (2UL)                     /*!< CCU4_CC4 TC: CLST (Bit 2)                                   */
#define CCU4_CC4_TC_CLST_Msk                  (0x4UL)                   /*!< CCU4_CC4 TC: CLST (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_TC_CMOD_Pos                  (3UL)                     /*!< CCU4_CC4 TC: CMOD (Bit 3)                                   */
#define CCU4_CC4_TC_CMOD_Msk                  (0x8UL)                   /*!< CCU4_CC4 TC: CMOD (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_TC_ECM_Pos                   (4UL)                     /*!< CCU4_CC4 TC: ECM (Bit 4)                                    */
#define CCU4_CC4_TC_ECM_Msk                   (0x10UL)                  /*!< CCU4_CC4 TC: ECM (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_CAPC_Pos                  (5UL)                     /*!< CCU4_CC4 TC: CAPC (Bit 5)                                   */
#define CCU4_CC4_TC_CAPC_Msk                  (0x60UL)                  /*!< CCU4_CC4 TC: CAPC (Bitfield-Mask: 0x03)                     */
#define CCU4_CC4_TC_ENDM_Pos                  (8UL)                     /*!< CCU4_CC4 TC: ENDM (Bit 8)                                   */
#define CCU4_CC4_TC_ENDM_Msk                  (0x300UL)                 /*!< CCU4_CC4 TC: ENDM (Bitfield-Mask: 0x03)                     */
#define CCU4_CC4_TC_STRM_Pos                  (10UL)                    /*!< CCU4_CC4 TC: STRM (Bit 10)                                  */
#define CCU4_CC4_TC_STRM_Msk                  (0x400UL)                 /*!< CCU4_CC4 TC: STRM (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_TC_SCE_Pos                   (11UL)                    /*!< CCU4_CC4 TC: SCE (Bit 11)                                   */
#define CCU4_CC4_TC_SCE_Msk                   (0x800UL)                 /*!< CCU4_CC4 TC: SCE (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_CCS_Pos                   (12UL)                    /*!< CCU4_CC4 TC: CCS (Bit 12)                                   */
#define CCU4_CC4_TC_CCS_Msk                   (0x1000UL)                /*!< CCU4_CC4 TC: CCS (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_DITHE_Pos                 (13UL)                    /*!< CCU4_CC4 TC: DITHE (Bit 13)                                 */
#define CCU4_CC4_TC_DITHE_Msk                 (0x6000UL)                /*!< CCU4_CC4 TC: DITHE (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_TC_DIM_Pos                   (15UL)                    /*!< CCU4_CC4 TC: DIM (Bit 15)                                   */
#define CCU4_CC4_TC_DIM_Msk                   (0x8000UL)                /*!< CCU4_CC4 TC: DIM (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_FPE_Pos                   (16UL)                    /*!< CCU4_CC4 TC: FPE (Bit 16)                                   */
#define CCU4_CC4_TC_FPE_Msk                   (0x10000UL)               /*!< CCU4_CC4 TC: FPE (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_TRAPE_Pos                 (17UL)                    /*!< CCU4_CC4 TC: TRAPE (Bit 17)                                 */
#define CCU4_CC4_TC_TRAPE_Msk                 (0x20000UL)               /*!< CCU4_CC4 TC: TRAPE (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_TC_TRPSE_Pos                 (21UL)                    /*!< CCU4_CC4 TC: TRPSE (Bit 21)                                 */
#define CCU4_CC4_TC_TRPSE_Msk                 (0x200000UL)              /*!< CCU4_CC4 TC: TRPSE (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_TC_TRPSW_Pos                 (22UL)                    /*!< CCU4_CC4 TC: TRPSW (Bit 22)                                 */
#define CCU4_CC4_TC_TRPSW_Msk                 (0x400000UL)              /*!< CCU4_CC4 TC: TRPSW (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_TC_EMS_Pos                   (23UL)                    /*!< CCU4_CC4 TC: EMS (Bit 23)                                   */
#define CCU4_CC4_TC_EMS_Msk                   (0x800000UL)              /*!< CCU4_CC4 TC: EMS (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_EMT_Pos                   (24UL)                    /*!< CCU4_CC4 TC: EMT (Bit 24)                                   */
#define CCU4_CC4_TC_EMT_Msk                   (0x1000000UL)             /*!< CCU4_CC4 TC: EMT (Bitfield-Mask: 0x01)                      */
#define CCU4_CC4_TC_MCME_Pos                  (25UL)                    /*!< CCU4_CC4 TC: MCME (Bit 25)                                  */
#define CCU4_CC4_TC_MCME_Msk                  (0x2000000UL)             /*!< CCU4_CC4 TC: MCME (Bitfield-Mask: 0x01)                     */

/* --------------------------------  CCU4_CC4_PSL  -------------------------------- */
#define CCU4_CC4_PSL_PSL_Pos                  (0UL)                     /*!< CCU4_CC4 PSL: PSL (Bit 0)                                   */
#define CCU4_CC4_PSL_PSL_Msk                  (0x1UL)                   /*!< CCU4_CC4 PSL: PSL (Bitfield-Mask: 0x01)                     */

/* --------------------------------  CCU4_CC4_DIT  -------------------------------- */
#define CCU4_CC4_DIT_DCV_Pos                  (0UL)                     /*!< CCU4_CC4 DIT: DCV (Bit 0)                                   */
#define CCU4_CC4_DIT_DCV_Msk                  (0xfUL)                   /*!< CCU4_CC4 DIT: DCV (Bitfield-Mask: 0x0f)                     */
#define CCU4_CC4_DIT_DCNT_Pos                 (8UL)                     /*!< CCU4_CC4 DIT: DCNT (Bit 8)                                  */
#define CCU4_CC4_DIT_DCNT_Msk                 (0xf00UL)                 /*!< CCU4_CC4 DIT: DCNT (Bitfield-Mask: 0x0f)                    */

/* --------------------------------  CCU4_CC4_DITS  ------------------------------- */
#define CCU4_CC4_DITS_DCVS_Pos                (0UL)                     /*!< CCU4_CC4 DITS: DCVS (Bit 0)                                 */
#define CCU4_CC4_DITS_DCVS_Msk                (0xfUL)                   /*!< CCU4_CC4 DITS: DCVS (Bitfield-Mask: 0x0f)                   */

/* --------------------------------  CCU4_CC4_PSC  -------------------------------- */
#define CCU4_CC4_PSC_PSIV_Pos                 (0UL)                     /*!< CCU4_CC4 PSC: PSIV (Bit 0)                                  */
#define CCU4_CC4_PSC_PSIV_Msk                 (0xfUL)                   /*!< CCU4_CC4 PSC: PSIV (Bitfield-Mask: 0x0f)                    */

/* --------------------------------  CCU4_CC4_FPC  -------------------------------- */
#define CCU4_CC4_FPC_PCMP_Pos                 (0UL)                     /*!< CCU4_CC4 FPC: PCMP (Bit 0)                                  */
#define CCU4_CC4_FPC_PCMP_Msk                 (0xfUL)                   /*!< CCU4_CC4 FPC: PCMP (Bitfield-Mask: 0x0f)                    */
#define CCU4_CC4_FPC_PVAL_Pos                 (8UL)                     /*!< CCU4_CC4 FPC: PVAL (Bit 8)                                  */
#define CCU4_CC4_FPC_PVAL_Msk                 (0xf00UL)                 /*!< CCU4_CC4 FPC: PVAL (Bitfield-Mask: 0x0f)                    */

/* --------------------------------  CCU4_CC4_FPCS  ------------------------------- */
#define CCU4_CC4_FPCS_PCMP_Pos                (0UL)                     /*!< CCU4_CC4 FPCS: PCMP (Bit 0)                                 */
#define CCU4_CC4_FPCS_PCMP_Msk                (0xfUL)                   /*!< CCU4_CC4 FPCS: PCMP (Bitfield-Mask: 0x0f)                   */

/* ---------------------------------  CCU4_CC4_PR  -------------------------------- */
#define CCU4_CC4_PR_PR_Pos                    (0UL)                     /*!< CCU4_CC4 PR: PR (Bit 0)                                     */
#define CCU4_CC4_PR_PR_Msk                    (0xffffUL)                /*!< CCU4_CC4 PR: PR (Bitfield-Mask: 0xffff)                     */

/* --------------------------------  CCU4_CC4_PRS  -------------------------------- */
#define CCU4_CC4_PRS_PRS_Pos                  (0UL)                     /*!< CCU4_CC4 PRS: PRS (Bit 0)                                   */
#define CCU4_CC4_PRS_PRS_Msk                  (0xffffUL)                /*!< CCU4_CC4 PRS: PRS (Bitfield-Mask: 0xffff)                   */

/* ---------------------------------  CCU4_CC4_CR  -------------------------------- */
#define CCU4_CC4_CR_CR_Pos                    (0UL)                     /*!< CCU4_CC4 CR: CR (Bit 0)                                     */
#define CCU4_CC4_CR_CR_Msk                    (0xffffUL)                /*!< CCU4_CC4 CR: CR (Bitfield-Mask: 0xffff)                     */

/* --------------------------------  CCU4_CC4_CRS  -------------------------------- */
#define CCU4_CC4_CRS_CRS_Pos                  (0UL)                     /*!< CCU4_CC4 CRS: CRS (Bit 0)                                   */
#define CCU4_CC4_CRS_CRS_Msk                  (0xffffUL)                /*!< CCU4_CC4 CRS: CRS (Bitfield-Mask: 0xffff)                   */

/* -------------------------------  CCU4_CC4_TIMER  ------------------------------- */
#define CCU4_CC4_TIMER_TVAL_Pos               (0UL)                     /*!< CCU4_CC4 TIMER: TVAL (Bit 0)                                */
#define CCU4_CC4_TIMER_TVAL_Msk               (0xffffUL)                /*!< CCU4_CC4 TIMER: TVAL (Bitfield-Mask: 0xffff)                */

/* ---------------------------------  CCU4_CC4_CV  -------------------------------- */
#define CCU4_CC4_CV_CAPTV_Pos                 (0UL)                     /*!< CCU4_CC4 CV: CAPTV (Bit 0)                                  */
#define CCU4_CC4_CV_CAPTV_Msk                 (0xffffUL)                /*!< CCU4_CC4 CV: CAPTV (Bitfield-Mask: 0xffff)                  */
#define CCU4_CC4_CV_FPCV_Pos                  (16UL)                    /*!< CCU4_CC4 CV: FPCV (Bit 16)                                  */
#define CCU4_CC4_CV_FPCV_Msk                  (0xf0000UL)               /*!< CCU4_CC4 CV: FPCV (Bitfield-Mask: 0x0f)                     */
#define CCU4_CC4_CV_FFL_Pos                   (20UL)                    /*!< CCU4_CC4 CV: FFL (Bit 20)                                   */
#define CCU4_CC4_CV_FFL_Msk                   (0x100000UL)              /*!< CCU4_CC4 CV: FFL (Bitfield-Mask: 0x01)                      */

/* --------------------------------  CCU4_CC4_INTS  ------------------------------- */
#define CCU4_CC4_INTS_PMUS_Pos                (0UL)                     /*!< CCU4_CC4 INTS: PMUS (Bit 0)                                 */
#define CCU4_CC4_INTS_PMUS_Msk                (0x1UL)                   /*!< CCU4_CC4 INTS: PMUS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_OMDS_Pos                (1UL)                     /*!< CCU4_CC4 INTS: OMDS (Bit 1)                                 */
#define CCU4_CC4_INTS_OMDS_Msk                (0x2UL)                   /*!< CCU4_CC4 INTS: OMDS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_CMUS_Pos                (2UL)                     /*!< CCU4_CC4 INTS: CMUS (Bit 2)                                 */
#define CCU4_CC4_INTS_CMUS_Msk                (0x4UL)                   /*!< CCU4_CC4 INTS: CMUS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_CMDS_Pos                (3UL)                     /*!< CCU4_CC4 INTS: CMDS (Bit 3)                                 */
#define CCU4_CC4_INTS_CMDS_Msk                (0x8UL)                   /*!< CCU4_CC4 INTS: CMDS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_E0AS_Pos                (8UL)                     /*!< CCU4_CC4 INTS: E0AS (Bit 8)                                 */
#define CCU4_CC4_INTS_E0AS_Msk                (0x100UL)                 /*!< CCU4_CC4 INTS: E0AS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_E1AS_Pos                (9UL)                     /*!< CCU4_CC4 INTS: E1AS (Bit 9)                                 */
#define CCU4_CC4_INTS_E1AS_Msk                (0x200UL)                 /*!< CCU4_CC4 INTS: E1AS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_E2AS_Pos                (10UL)                    /*!< CCU4_CC4 INTS: E2AS (Bit 10)                                */
#define CCU4_CC4_INTS_E2AS_Msk                (0x400UL)                 /*!< CCU4_CC4 INTS: E2AS (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTS_TRPF_Pos                (11UL)                    /*!< CCU4_CC4 INTS: TRPF (Bit 11)                                */
#define CCU4_CC4_INTS_TRPF_Msk                (0x800UL)                 /*!< CCU4_CC4 INTS: TRPF (Bitfield-Mask: 0x01)                   */

/* --------------------------------  CCU4_CC4_INTE  ------------------------------- */
#define CCU4_CC4_INTE_PME_Pos                 (0UL)                     /*!< CCU4_CC4 INTE: PME (Bit 0)                                  */
#define CCU4_CC4_INTE_PME_Msk                 (0x1UL)                   /*!< CCU4_CC4 INTE: PME (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_INTE_OME_Pos                 (1UL)                     /*!< CCU4_CC4 INTE: OME (Bit 1)                                  */
#define CCU4_CC4_INTE_OME_Msk                 (0x2UL)                   /*!< CCU4_CC4 INTE: OME (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_INTE_CMUE_Pos                (2UL)                     /*!< CCU4_CC4 INTE: CMUE (Bit 2)                                 */
#define CCU4_CC4_INTE_CMUE_Msk                (0x4UL)                   /*!< CCU4_CC4 INTE: CMUE (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTE_CMDE_Pos                (3UL)                     /*!< CCU4_CC4 INTE: CMDE (Bit 3)                                 */
#define CCU4_CC4_INTE_CMDE_Msk                (0x8UL)                   /*!< CCU4_CC4 INTE: CMDE (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTE_E0AE_Pos                (8UL)                     /*!< CCU4_CC4 INTE: E0AE (Bit 8)                                 */
#define CCU4_CC4_INTE_E0AE_Msk                (0x100UL)                 /*!< CCU4_CC4 INTE: E0AE (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTE_E1AE_Pos                (9UL)                     /*!< CCU4_CC4 INTE: E1AE (Bit 9)                                 */
#define CCU4_CC4_INTE_E1AE_Msk                (0x200UL)                 /*!< CCU4_CC4 INTE: E1AE (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_INTE_E2AE_Pos                (10UL)                    /*!< CCU4_CC4 INTE: E2AE (Bit 10)                                */
#define CCU4_CC4_INTE_E2AE_Msk                (0x400UL)                 /*!< CCU4_CC4 INTE: E2AE (Bitfield-Mask: 0x01)                   */

/* --------------------------------  CCU4_CC4_SRS  -------------------------------- */
#define CCU4_CC4_SRS_POSR_Pos                 (0UL)                     /*!< CCU4_CC4 SRS: POSR (Bit 0)                                  */
#define CCU4_CC4_SRS_POSR_Msk                 (0x3UL)                   /*!< CCU4_CC4 SRS: POSR (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_SRS_CMSR_Pos                 (2UL)                     /*!< CCU4_CC4 SRS: CMSR (Bit 2)                                  */
#define CCU4_CC4_SRS_CMSR_Msk                 (0xcUL)                   /*!< CCU4_CC4 SRS: CMSR (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_SRS_E0SR_Pos                 (8UL)                     /*!< CCU4_CC4 SRS: E0SR (Bit 8)                                  */
#define CCU4_CC4_SRS_E0SR_Msk                 (0x300UL)                 /*!< CCU4_CC4 SRS: E0SR (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_SRS_E1SR_Pos                 (10UL)                    /*!< CCU4_CC4 SRS: E1SR (Bit 10)                                 */
#define CCU4_CC4_SRS_E1SR_Msk                 (0xc00UL)                 /*!< CCU4_CC4 SRS: E1SR (Bitfield-Mask: 0x03)                    */
#define CCU4_CC4_SRS_E2SR_Pos                 (12UL)                    /*!< CCU4_CC4 SRS: E2SR (Bit 12)                                 */
#define CCU4_CC4_SRS_E2SR_Msk                 (0x3000UL)                /*!< CCU4_CC4 SRS: E2SR (Bitfield-Mask: 0x03)                    */

/* --------------------------------  CCU4_CC4_SWS  -------------------------------- */
#define CCU4_CC4_SWS_SPM_Pos                  (0UL)                     /*!< CCU4_CC4 SWS: SPM (Bit 0)                                   */
#define CCU4_CC4_SWS_SPM_Msk                  (0x1UL)                   /*!< CCU4_CC4 SWS: SPM (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_SWS_SOM_Pos                  (1UL)                     /*!< CCU4_CC4 SWS: SOM (Bit 1)                                   */
#define CCU4_CC4_SWS_SOM_Msk                  (0x2UL)                   /*!< CCU4_CC4 SWS: SOM (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_SWS_SCMU_Pos                 (2UL)                     /*!< CCU4_CC4 SWS: SCMU (Bit 2)                                  */
#define CCU4_CC4_SWS_SCMU_Msk                 (0x4UL)                   /*!< CCU4_CC4 SWS: SCMU (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWS_SCMD_Pos                 (3UL)                     /*!< CCU4_CC4 SWS: SCMD (Bit 3)                                  */
#define CCU4_CC4_SWS_SCMD_Msk                 (0x8UL)                   /*!< CCU4_CC4 SWS: SCMD (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWS_SE0A_Pos                 (8UL)                     /*!< CCU4_CC4 SWS: SE0A (Bit 8)                                  */
#define CCU4_CC4_SWS_SE0A_Msk                 (0x100UL)                 /*!< CCU4_CC4 SWS: SE0A (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWS_SE1A_Pos                 (9UL)                     /*!< CCU4_CC4 SWS: SE1A (Bit 9)                                  */
#define CCU4_CC4_SWS_SE1A_Msk                 (0x200UL)                 /*!< CCU4_CC4 SWS: SE1A (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWS_SE2A_Pos                 (10UL)                    /*!< CCU4_CC4 SWS: SE2A (Bit 10)                                 */
#define CCU4_CC4_SWS_SE2A_Msk                 (0x400UL)                 /*!< CCU4_CC4 SWS: SE2A (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWS_STRPF_Pos                (11UL)                    /*!< CCU4_CC4 SWS: STRPF (Bit 11)                                */
#define CCU4_CC4_SWS_STRPF_Msk                (0x800UL)                 /*!< CCU4_CC4 SWS: STRPF (Bitfield-Mask: 0x01)                   */

/* --------------------------------  CCU4_CC4_SWR  -------------------------------- */
#define CCU4_CC4_SWR_RPM_Pos                  (0UL)                     /*!< CCU4_CC4 SWR: RPM (Bit 0)                                   */
#define CCU4_CC4_SWR_RPM_Msk                  (0x1UL)                   /*!< CCU4_CC4 SWR: RPM (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_SWR_ROM_Pos                  (1UL)                     /*!< CCU4_CC4 SWR: ROM (Bit 1)                                   */
#define CCU4_CC4_SWR_ROM_Msk                  (0x2UL)                   /*!< CCU4_CC4 SWR: ROM (Bitfield-Mask: 0x01)                     */
#define CCU4_CC4_SWR_RCMU_Pos                 (2UL)                     /*!< CCU4_CC4 SWR: RCMU (Bit 2)                                  */
#define CCU4_CC4_SWR_RCMU_Msk                 (0x4UL)                   /*!< CCU4_CC4 SWR: RCMU (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWR_RCMD_Pos                 (3UL)                     /*!< CCU4_CC4 SWR: RCMD (Bit 3)                                  */
#define CCU4_CC4_SWR_RCMD_Msk                 (0x8UL)                   /*!< CCU4_CC4 SWR: RCMD (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWR_RE0A_Pos                 (8UL)                     /*!< CCU4_CC4 SWR: RE0A (Bit 8)                                  */
#define CCU4_CC4_SWR_RE0A_Msk                 (0x100UL)                 /*!< CCU4_CC4 SWR: RE0A (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWR_RE1A_Pos                 (9UL)                     /*!< CCU4_CC4 SWR: RE1A (Bit 9)                                  */
#define CCU4_CC4_SWR_RE1A_Msk                 (0x200UL)                 /*!< CCU4_CC4 SWR: RE1A (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWR_RE2A_Pos                 (10UL)                    /*!< CCU4_CC4 SWR: RE2A (Bit 10)                                 */
#define CCU4_CC4_SWR_RE2A_Msk                 (0x400UL)                 /*!< CCU4_CC4 SWR: RE2A (Bitfield-Mask: 0x01)                    */
#define CCU4_CC4_SWR_RTRPF_Pos                (11UL)                    /*!< CCU4_CC4 SWR: RTRPF (Bit 11)                                */
#define CCU4_CC4_SWR_RTRPF_Msk                (0x800UL)                 /*!< CCU4_CC4 SWR: RTRPF (Bitfield-Mask: 0x01)                   */

/* -------------------------------  CCU4_CC4_ECRD0  ------------------------------- */
#define CCU4_CC4_ECRD0_CAPV_Pos               (0UL)                     /*!< CCU4_CC4 ECRD0: CAPV (Bit 0)                                */
#define CCU4_CC4_ECRD0_CAPV_Msk               (0xffffUL)                /*!< CCU4_CC4 ECRD0: CAPV (Bitfield-Mask: 0xffff)                */
#define CCU4_CC4_ECRD0_FPCV_Pos               (16UL)                    /*!< CCU4_CC4 ECRD0: FPCV (Bit 16)                               */
#define CCU4_CC4_ECRD0_FPCV_Msk               (0xf0000UL)               /*!< CCU4_CC4 ECRD0: FPCV (Bitfield-Mask: 0x0f)                  */
#define CCU4_CC4_ECRD0_SPTR_Pos               (20UL)                    /*!< CCU4_CC4 ECRD0: SPTR (Bit 20)                               */
#define CCU4_CC4_ECRD0_SPTR_Msk               (0x300000UL)              /*!< CCU4_CC4 ECRD0: SPTR (Bitfield-Mask: 0x03)                  */
#define CCU4_CC4_ECRD0_VPTR_Pos               (22UL)                    /*!< CCU4_CC4 ECRD0: VPTR (Bit 22)                               */
#define CCU4_CC4_ECRD0_VPTR_Msk               (0xc00000UL)              /*!< CCU4_CC4 ECRD0: VPTR (Bitfield-Mask: 0x03)                  */
#define CCU4_CC4_ECRD0_FFL_Pos                (24UL)                    /*!< CCU4_CC4 ECRD0: FFL (Bit 24)                                */
#define CCU4_CC4_ECRD0_FFL_Msk                (0x1000000UL)             /*!< CCU4_CC4 ECRD0: FFL (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_ECRD0_LCV_Pos                (25UL)                    /*!< CCU4_CC4 ECRD0: LCV (Bit 25)                                */
#define CCU4_CC4_ECRD0_LCV_Msk                (0x2000000UL)             /*!< CCU4_CC4 ECRD0: LCV (Bitfield-Mask: 0x01)                   */

/* -------------------------------  CCU4_CC4_ECRD1  ------------------------------- */
#define CCU4_CC4_ECRD1_CAPV_Pos               (0UL)                     /*!< CCU4_CC4 ECRD1: CAPV (Bit 0)                                */
#define CCU4_CC4_ECRD1_CAPV_Msk               (0xffffUL)                /*!< CCU4_CC4 ECRD1: CAPV (Bitfield-Mask: 0xffff)                */
#define CCU4_CC4_ECRD1_FPCV_Pos               (16UL)                    /*!< CCU4_CC4 ECRD1: FPCV (Bit 16)                               */
#define CCU4_CC4_ECRD1_FPCV_Msk               (0xf0000UL)               /*!< CCU4_CC4 ECRD1: FPCV (Bitfield-Mask: 0x0f)                  */
#define CCU4_CC4_ECRD1_SPTR_Pos               (20UL)                    /*!< CCU4_CC4 ECRD1: SPTR (Bit 20)                               */
#define CCU4_CC4_ECRD1_SPTR_Msk               (0x300000UL)              /*!< CCU4_CC4 ECRD1: SPTR (Bitfield-Mask: 0x03)                  */
#define CCU4_CC4_ECRD1_VPTR_Pos               (22UL)                    /*!< CCU4_CC4 ECRD1: VPTR (Bit 22)                               */
#define CCU4_CC4_ECRD1_VPTR_Msk               (0xc00000UL)              /*!< CCU4_CC4 ECRD1: VPTR (Bitfield-Mask: 0x03)                  */
#define CCU4_CC4_ECRD1_FFL_Pos                (24UL)                    /*!< CCU4_CC4 ECRD1: FFL (Bit 24)                                */
#define CCU4_CC4_ECRD1_FFL_Msk                (0x1000000UL)             /*!< CCU4_CC4 ECRD1: FFL (Bitfield-Mask: 0x01)                   */
#define CCU4_CC4_ECRD1_LCV_Pos                (25UL)                    /*!< CCU4_CC4 ECRD1: LCV (Bit 25)                                */
#define CCU4_CC4_ECRD1_LCV_Msk                (0x2000000UL)             /*!< CCU4_CC4 ECRD1: LCV (Bitfield-Mask: 0x01)                   */


/* ================================================================================ */
/* ================          Group 'VADC' Position & Mask          ================ */
/* ================================================================================ */


/* ----------------------------------  VADC_CLC  ---------------------------------- */
#define VADC_CLC_DISR_Pos                     (0UL)                     /*!< VADC CLC: DISR (Bit 0)                                      */
#define VADC_CLC_DISR_Msk                     (0x1UL)                   /*!< VADC CLC: DISR (Bitfield-Mask: 0x01)                        */
#define VADC_CLC_DISS_Pos                     (1UL)                     /*!< VADC CLC: DISS (Bit 1)                                      */
#define VADC_CLC_DISS_Msk                     (0x2UL)                   /*!< VADC CLC: DISS (Bitfield-Mask: 0x01)                        */
#define VADC_CLC_EDIS_Pos                     (3UL)                     /*!< VADC CLC: EDIS (Bit 3)                                      */
#define VADC_CLC_EDIS_Msk                     (0x8UL)                   /*!< VADC CLC: EDIS (Bitfield-Mask: 0x01)                        */

/* -----------------------------------  VADC_ID  ---------------------------------- */
#define VADC_ID_MOD_REV_Pos                   (0UL)                     /*!< VADC ID: MOD_REV (Bit 0)                                    */
#define VADC_ID_MOD_REV_Msk                   (0xffUL)                  /*!< VADC ID: MOD_REV (Bitfield-Mask: 0xff)                      */
#define VADC_ID_MOD_TYPE_Pos                  (8UL)                     /*!< VADC ID: MOD_TYPE (Bit 8)                                   */
#define VADC_ID_MOD_TYPE_Msk                  (0xff00UL)                /*!< VADC ID: MOD_TYPE (Bitfield-Mask: 0xff)                     */
#define VADC_ID_MOD_NUMBER_Pos                (16UL)                    /*!< VADC ID: MOD_NUMBER (Bit 16)                                */
#define VADC_ID_MOD_NUMBER_Msk                (0xffff0000UL)            /*!< VADC ID: MOD_NUMBER (Bitfield-Mask: 0xffff)                 */

/* ----------------------------------  VADC_OCS  ---------------------------------- */
#define VADC_OCS_TGS_Pos                      (0UL)                     /*!< VADC OCS: TGS (Bit 0)                                       */
#define VADC_OCS_TGS_Msk                      (0x3UL)                   /*!< VADC OCS: TGS (Bitfield-Mask: 0x03)                         */
#define VADC_OCS_TGB_Pos                      (2UL)                     /*!< VADC OCS: TGB (Bit 2)                                       */
#define VADC_OCS_TGB_Msk                      (0x4UL)                   /*!< VADC OCS: TGB (Bitfield-Mask: 0x01)                         */
#define VADC_OCS_TG_P_Pos                     (3UL)                     /*!< VADC OCS: TG_P (Bit 3)                                      */
#define VADC_OCS_TG_P_Msk                     (0x8UL)                   /*!< VADC OCS: TG_P (Bitfield-Mask: 0x01)                        */
#define VADC_OCS_SUS_Pos                      (24UL)                    /*!< VADC OCS: SUS (Bit 24)                                      */
#define VADC_OCS_SUS_Msk                      (0xf000000UL)             /*!< VADC OCS: SUS (Bitfield-Mask: 0x0f)                         */
#define VADC_OCS_SUS_P_Pos                    (28UL)                    /*!< VADC OCS: SUS_P (Bit 28)                                    */
#define VADC_OCS_SUS_P_Msk                    (0x10000000UL)            /*!< VADC OCS: SUS_P (Bitfield-Mask: 0x01)                       */
#define VADC_OCS_SUSSTA_Pos                   (29UL)                    /*!< VADC OCS: SUSSTA (Bit 29)                                   */
#define VADC_OCS_SUSSTA_Msk                   (0x20000000UL)            /*!< VADC OCS: SUSSTA (Bitfield-Mask: 0x01)                      */

/* --------------------------------  VADC_GLOBCFG  -------------------------------- */
#define VADC_GLOBCFG_DIVA_Pos                 (0UL)                     /*!< VADC GLOBCFG: DIVA (Bit 0)                                  */
#define VADC_GLOBCFG_DIVA_Msk                 (0x1fUL)                  /*!< VADC GLOBCFG: DIVA (Bitfield-Mask: 0x1f)                    */
#define VADC_GLOBCFG_DCMSB_Pos                (7UL)                     /*!< VADC GLOBCFG: DCMSB (Bit 7)                                 */
#define VADC_GLOBCFG_DCMSB_Msk                (0x80UL)                  /*!< VADC GLOBCFG: DCMSB (Bitfield-Mask: 0x01)                   */
#define VADC_GLOBCFG_DIVD_Pos                 (8UL)                     /*!< VADC GLOBCFG: DIVD (Bit 8)                                  */
#define VADC_GLOBCFG_DIVD_Msk                 (0x300UL)                 /*!< VADC GLOBCFG: DIVD (Bitfield-Mask: 0x03)                    */
#define VADC_GLOBCFG_DIVWC_Pos                (15UL)                    /*!< VADC GLOBCFG: DIVWC (Bit 15)                                */
#define VADC_GLOBCFG_DIVWC_Msk                (0x8000UL)                /*!< VADC GLOBCFG: DIVWC (Bitfield-Mask: 0x01)                   */
#define VADC_GLOBCFG_DPCAL0_Pos               (16UL)                    /*!< VADC GLOBCFG: DPCAL0 (Bit 16)                               */
#define VADC_GLOBCFG_DPCAL0_Msk               (0x10000UL)               /*!< VADC GLOBCFG: DPCAL0 (Bitfield-Mask: 0x01)                  */
#define VADC_GLOBCFG_DPCAL1_Pos               (17UL)                    /*!< VADC GLOBCFG: DPCAL1 (Bit 17)                               */
#define VADC_GLOBCFG_DPCAL1_Msk               (0x20000UL)               /*!< VADC GLOBCFG: DPCAL1 (Bitfield-Mask: 0x01)                  */
#define VADC_GLOBCFG_SUCAL_Pos                (31UL)                    /*!< VADC GLOBCFG: SUCAL (Bit 31)                                */
#define VADC_GLOBCFG_SUCAL_Msk                (0x80000000UL)            /*!< VADC GLOBCFG: SUCAL (Bitfield-Mask: 0x01)                   */

/* -------------------------------  VADC_GLOBICLASS  ------------------------------ */
#define VADC_GLOBICLASS_STCS_Pos              (0UL)                     /*!< VADC GLOBICLASS: STCS (Bit 0)                               */
#define VADC_GLOBICLASS_STCS_Msk              (0x1fUL)                  /*!< VADC GLOBICLASS: STCS (Bitfield-Mask: 0x1f)                 */
#define VADC_GLOBICLASS_CMS_Pos               (8UL)                     /*!< VADC GLOBICLASS: CMS (Bit 8)                                */
#define VADC_GLOBICLASS_CMS_Msk               (0x700UL)                 /*!< VADC GLOBICLASS: CMS (Bitfield-Mask: 0x07)                  */

/* -------------------------------  VADC_GLOBEFLAG  ------------------------------- */
#define VADC_GLOBEFLAG_SEVGLB_Pos             (0UL)                     /*!< VADC GLOBEFLAG: SEVGLB (Bit 0)                              */
#define VADC_GLOBEFLAG_SEVGLB_Msk             (0x1UL)                   /*!< VADC GLOBEFLAG: SEVGLB (Bitfield-Mask: 0x01)                */
#define VADC_GLOBEFLAG_REVGLB_Pos             (8UL)                     /*!< VADC GLOBEFLAG: REVGLB (Bit 8)                              */
#define VADC_GLOBEFLAG_REVGLB_Msk             (0x100UL)                 /*!< VADC GLOBEFLAG: REVGLB (Bitfield-Mask: 0x01)                */
#define VADC_GLOBEFLAG_SEVGLBCLR_Pos          (16UL)                    /*!< VADC GLOBEFLAG: SEVGLBCLR (Bit 16)                          */
#define VADC_GLOBEFLAG_SEVGLBCLR_Msk          (0x10000UL)               /*!< VADC GLOBEFLAG: SEVGLBCLR (Bitfield-Mask: 0x01)             */
#define VADC_GLOBEFLAG_REVGLBCLR_Pos          (24UL)                    /*!< VADC GLOBEFLAG: REVGLBCLR (Bit 24)                          */
#define VADC_GLOBEFLAG_REVGLBCLR_Msk          (0x1000000UL)             /*!< VADC GLOBEFLAG: REVGLBCLR (Bitfield-Mask: 0x01)             */

/* --------------------------------  VADC_GLOBEVNP  ------------------------------- */
#define VADC_GLOBEVNP_SEV0NP_Pos              (0UL)                     /*!< VADC GLOBEVNP: SEV0NP (Bit 0)                               */
#define VADC_GLOBEVNP_SEV0NP_Msk              (0xfUL)                   /*!< VADC GLOBEVNP: SEV0NP (Bitfield-Mask: 0x0f)                 */
#define VADC_GLOBEVNP_REV0NP_Pos              (16UL)                    /*!< VADC GLOBEVNP: REV0NP (Bit 16)                              */
#define VADC_GLOBEVNP_REV0NP_Msk              (0xf0000UL)               /*!< VADC GLOBEVNP: REV0NP (Bitfield-Mask: 0x0f)                 */

/* ---------------------------------  VADC_BRSSEL  -------------------------------- */
#define VADC_BRSSEL_CHSELG0_Pos               (0UL)                     /*!< VADC BRSSEL: CHSELG0 (Bit 0)                                */
#define VADC_BRSSEL_CHSELG0_Msk               (0x1UL)                   /*!< VADC BRSSEL: CHSELG0 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG1_Pos               (1UL)                     /*!< VADC BRSSEL: CHSELG1 (Bit 1)                                */
#define VADC_BRSSEL_CHSELG1_Msk               (0x2UL)                   /*!< VADC BRSSEL: CHSELG1 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG2_Pos               (2UL)                     /*!< VADC BRSSEL: CHSELG2 (Bit 2)                                */
#define VADC_BRSSEL_CHSELG2_Msk               (0x4UL)                   /*!< VADC BRSSEL: CHSELG2 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG3_Pos               (3UL)                     /*!< VADC BRSSEL: CHSELG3 (Bit 3)                                */
#define VADC_BRSSEL_CHSELG3_Msk               (0x8UL)                   /*!< VADC BRSSEL: CHSELG3 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG4_Pos               (4UL)                     /*!< VADC BRSSEL: CHSELG4 (Bit 4)                                */
#define VADC_BRSSEL_CHSELG4_Msk               (0x10UL)                  /*!< VADC BRSSEL: CHSELG4 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG5_Pos               (5UL)                     /*!< VADC BRSSEL: CHSELG5 (Bit 5)                                */
#define VADC_BRSSEL_CHSELG5_Msk               (0x20UL)                  /*!< VADC BRSSEL: CHSELG5 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG6_Pos               (6UL)                     /*!< VADC BRSSEL: CHSELG6 (Bit 6)                                */
#define VADC_BRSSEL_CHSELG6_Msk               (0x40UL)                  /*!< VADC BRSSEL: CHSELG6 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSSEL_CHSELG7_Pos               (7UL)                     /*!< VADC BRSSEL: CHSELG7 (Bit 7)                                */
#define VADC_BRSSEL_CHSELG7_Msk               (0x80UL)                  /*!< VADC BRSSEL: CHSELG7 (Bitfield-Mask: 0x01)                  */

/* ---------------------------------  VADC_BRSPND  -------------------------------- */
#define VADC_BRSPND_CHPNDG0_Pos               (0UL)                     /*!< VADC BRSPND: CHPNDG0 (Bit 0)                                */
#define VADC_BRSPND_CHPNDG0_Msk               (0x1UL)                   /*!< VADC BRSPND: CHPNDG0 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG1_Pos               (1UL)                     /*!< VADC BRSPND: CHPNDG1 (Bit 1)                                */
#define VADC_BRSPND_CHPNDG1_Msk               (0x2UL)                   /*!< VADC BRSPND: CHPNDG1 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG2_Pos               (2UL)                     /*!< VADC BRSPND: CHPNDG2 (Bit 2)                                */
#define VADC_BRSPND_CHPNDG2_Msk               (0x4UL)                   /*!< VADC BRSPND: CHPNDG2 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG3_Pos               (3UL)                     /*!< VADC BRSPND: CHPNDG3 (Bit 3)                                */
#define VADC_BRSPND_CHPNDG3_Msk               (0x8UL)                   /*!< VADC BRSPND: CHPNDG3 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG4_Pos               (4UL)                     /*!< VADC BRSPND: CHPNDG4 (Bit 4)                                */
#define VADC_BRSPND_CHPNDG4_Msk               (0x10UL)                  /*!< VADC BRSPND: CHPNDG4 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG5_Pos               (5UL)                     /*!< VADC BRSPND: CHPNDG5 (Bit 5)                                */
#define VADC_BRSPND_CHPNDG5_Msk               (0x20UL)                  /*!< VADC BRSPND: CHPNDG5 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG6_Pos               (6UL)                     /*!< VADC BRSPND: CHPNDG6 (Bit 6)                                */
#define VADC_BRSPND_CHPNDG6_Msk               (0x40UL)                  /*!< VADC BRSPND: CHPNDG6 (Bitfield-Mask: 0x01)                  */
#define VADC_BRSPND_CHPNDG7_Pos               (7UL)                     /*!< VADC BRSPND: CHPNDG7 (Bit 7)                                */
#define VADC_BRSPND_CHPNDG7_Msk               (0x80UL)                  /*!< VADC BRSPND: CHPNDG7 (Bitfield-Mask: 0x01)                  */

/* --------------------------------  VADC_BRSCTRL  -------------------------------- */
#define VADC_BRSCTRL_SRCRESREG_Pos            (0UL)                     /*!< VADC BRSCTRL: SRCRESREG (Bit 0)                             */
#define VADC_BRSCTRL_SRCRESREG_Msk            (0xfUL)                   /*!< VADC BRSCTRL: SRCRESREG (Bitfield-Mask: 0x0f)               */
#define VADC_BRSCTRL_XTSEL_Pos                (8UL)                     /*!< VADC BRSCTRL: XTSEL (Bit 8)                                 */
#define VADC_BRSCTRL_XTSEL_Msk                (0xf00UL)                 /*!< VADC BRSCTRL: XTSEL (Bitfield-Mask: 0x0f)                   */
#define VADC_BRSCTRL_XTLVL_Pos                (12UL)                    /*!< VADC BRSCTRL: XTLVL (Bit 12)                                */
#define VADC_BRSCTRL_XTLVL_Msk                (0x1000UL)                /*!< VADC BRSCTRL: XTLVL (Bitfield-Mask: 0x01)                   */
#define VADC_BRSCTRL_XTMODE_Pos               (13UL)                    /*!< VADC BRSCTRL: XTMODE (Bit 13)                               */
#define VADC_BRSCTRL_XTMODE_Msk               (0x6000UL)                /*!< VADC BRSCTRL: XTMODE (Bitfield-Mask: 0x03)                  */
#define VADC_BRSCTRL_XTWC_Pos                 (15UL)                    /*!< VADC BRSCTRL: XTWC (Bit 15)                                 */
#define VADC_BRSCTRL_XTWC_Msk                 (0x8000UL)                /*!< VADC BRSCTRL: XTWC (Bitfield-Mask: 0x01)                    */
#define VADC_BRSCTRL_GTSEL_Pos                (16UL)                    /*!< VADC BRSCTRL: GTSEL (Bit 16)                                */
#define VADC_BRSCTRL_GTSEL_Msk                (0xf0000UL)               /*!< VADC BRSCTRL: GTSEL (Bitfield-Mask: 0x0f)                   */
#define VADC_BRSCTRL_GTLVL_Pos                (20UL)                    /*!< VADC BRSCTRL: GTLVL (Bit 20)                                */
#define VADC_BRSCTRL_GTLVL_Msk                (0x100000UL)              /*!< VADC BRSCTRL: GTLVL (Bitfield-Mask: 0x01)                   */
#define VADC_BRSCTRL_GTWC_Pos                 (23UL)                    /*!< VADC BRSCTRL: GTWC (Bit 23)                                 */
#define VADC_BRSCTRL_GTWC_Msk                 (0x800000UL)              /*!< VADC BRSCTRL: GTWC (Bitfield-Mask: 0x01)                    */

/* ---------------------------------  VADC_BRSMR  --------------------------------- */
#define VADC_BRSMR_ENGT_Pos                   (0UL)                     /*!< VADC BRSMR: ENGT (Bit 0)                                    */
#define VADC_BRSMR_ENGT_Msk                   (0x3UL)                   /*!< VADC BRSMR: ENGT (Bitfield-Mask: 0x03)                      */
#define VADC_BRSMR_ENTR_Pos                   (2UL)                     /*!< VADC BRSMR: ENTR (Bit 2)                                    */
#define VADC_BRSMR_ENTR_Msk                   (0x4UL)                   /*!< VADC BRSMR: ENTR (Bitfield-Mask: 0x01)                      */
#define VADC_BRSMR_ENSI_Pos                   (3UL)                     /*!< VADC BRSMR: ENSI (Bit 3)                                    */
#define VADC_BRSMR_ENSI_Msk                   (0x8UL)                   /*!< VADC BRSMR: ENSI (Bitfield-Mask: 0x01)                      */
#define VADC_BRSMR_SCAN_Pos                   (4UL)                     /*!< VADC BRSMR: SCAN (Bit 4)                                    */
#define VADC_BRSMR_SCAN_Msk                   (0x10UL)                  /*!< VADC BRSMR: SCAN (Bitfield-Mask: 0x01)                      */
#define VADC_BRSMR_LDM_Pos                    (5UL)                     /*!< VADC BRSMR: LDM (Bit 5)                                     */
#define VADC_BRSMR_LDM_Msk                    (0x20UL)                  /*!< VADC BRSMR: LDM (Bitfield-Mask: 0x01)                       */
#define VADC_BRSMR_REQGT_Pos                  (7UL)                     /*!< VADC BRSMR: REQGT (Bit 7)                                   */
#define VADC_BRSMR_REQGT_Msk                  (0x80UL)                  /*!< VADC BRSMR: REQGT (Bitfield-Mask: 0x01)                     */
#define VADC_BRSMR_CLRPND_Pos                 (8UL)                     /*!< VADC BRSMR: CLRPND (Bit 8)                                  */
#define VADC_BRSMR_CLRPND_Msk                 (0x100UL)                 /*!< VADC BRSMR: CLRPND (Bitfield-Mask: 0x01)                    */
#define VADC_BRSMR_LDEV_Pos                   (9UL)                     /*!< VADC BRSMR: LDEV (Bit 9)                                    */
#define VADC_BRSMR_LDEV_Msk                   (0x200UL)                 /*!< VADC BRSMR: LDEV (Bitfield-Mask: 0x01)                      */
#define VADC_BRSMR_RPTDIS_Pos                 (16UL)                    /*!< VADC BRSMR: RPTDIS (Bit 16)                                 */
#define VADC_BRSMR_RPTDIS_Msk                 (0x10000UL)               /*!< VADC BRSMR: RPTDIS (Bitfield-Mask: 0x01)                    */

/* --------------------------------  VADC_GLOBRCR  -------------------------------- */
#define VADC_GLOBRCR_DRCTR_Pos                (16UL)                    /*!< VADC GLOBRCR: DRCTR (Bit 16)                                */
#define VADC_GLOBRCR_DRCTR_Msk                (0xf0000UL)               /*!< VADC GLOBRCR: DRCTR (Bitfield-Mask: 0x0f)                   */
#define VADC_GLOBRCR_WFR_Pos                  (24UL)                    /*!< VADC GLOBRCR: WFR (Bit 24)                                  */
#define VADC_GLOBRCR_WFR_Msk                  (0x1000000UL)             /*!< VADC GLOBRCR: WFR (Bitfield-Mask: 0x01)                     */
#define VADC_GLOBRCR_SRGEN_Pos                (31UL)                    /*!< VADC GLOBRCR: SRGEN (Bit 31)                                */
#define VADC_GLOBRCR_SRGEN_Msk                (0x80000000UL)            /*!< VADC GLOBRCR: SRGEN (Bitfield-Mask: 0x01)                   */

/* --------------------------------  VADC_GLOBRES  -------------------------------- */
#define VADC_GLOBRES_RESULT_Pos               (0UL)                     /*!< VADC GLOBRES: RESULT (Bit 0)                                */
#define VADC_GLOBRES_RESULT_Msk               (0xffffUL)                /*!< VADC GLOBRES: RESULT (Bitfield-Mask: 0xffff)                */
#define VADC_GLOBRES_GNR_Pos                  (16UL)                    /*!< VADC GLOBRES: GNR (Bit 16)                                  */
#define VADC_GLOBRES_GNR_Msk                  (0xf0000UL)               /*!< VADC GLOBRES: GNR (Bitfield-Mask: 0x0f)                     */
#define VADC_GLOBRES_CHNR_Pos                 (20UL)                    /*!< VADC GLOBRES: CHNR (Bit 20)                                 */
#define VADC_GLOBRES_CHNR_Msk                 (0x1f00000UL)             /*!< VADC GLOBRES: CHNR (Bitfield-Mask: 0x1f)                    */
#define VADC_GLOBRES_EMUX_Pos                 (25UL)                    /*!< VADC GLOBRES: EMUX (Bit 25)                                 */
#define VADC_GLOBRES_EMUX_Msk                 (0xe000000UL)             /*!< VADC GLOBRES: EMUX (Bitfield-Mask: 0x07)                    */
#define VADC_GLOBRES_CRS_Pos                  (28UL)                    /*!< VADC GLOBRES: CRS (Bit 28)                                  */
#define VADC_GLOBRES_CRS_Msk                  (0x30000000UL)            /*!< VADC GLOBRES: CRS (Bitfield-Mask: 0x03)                     */
#define VADC_GLOBRES_FCR_Pos                  (30UL)                    /*!< VADC GLOBRES: FCR (Bit 30)                                  */
#define VADC_GLOBRES_FCR_Msk                  (0x40000000UL)            /*!< VADC GLOBRES: FCR (Bitfield-Mask: 0x01)                     */
#define VADC_GLOBRES_VF_Pos                   (31UL)                    /*!< VADC GLOBRES: VF (Bit 31)                                   */
#define VADC_GLOBRES_VF_Msk                   (0x80000000UL)            /*!< VADC GLOBRES: VF (Bitfield-Mask: 0x01)                      */

/* --------------------------------  VADC_GLOBRESD  ------------------------------- */
#define VADC_GLOBRESD_RESULT_Pos              (0UL)                     /*!< VADC GLOBRESD: RESULT (Bit 0)                               */
#define VADC_GLOBRESD_RESULT_Msk              (0xffffUL)                /*!< VADC GLOBRESD: RESULT (Bitfield-Mask: 0xffff)               */
#define VADC_GLOBRESD_GNR_Pos                 (16UL)                    /*!< VADC GLOBRESD: GNR (Bit 16)                                 */
#define VADC_GLOBRESD_GNR_Msk                 (0xf0000UL)               /*!< VADC GLOBRESD: GNR (Bitfield-Mask: 0x0f)                    */
#define VADC_GLOBRESD_CHNR_Pos                (20UL)                    /*!< VADC GLOBRESD: CHNR (Bit 20)                                */
#define VADC_GLOBRESD_CHNR_Msk                (0x1f00000UL)             /*!< VADC GLOBRESD: CHNR (Bitfield-Mask: 0x1f)                   */
#define VADC_GLOBRESD_EMUX_Pos                (25UL)                    /*!< VADC GLOBRESD: EMUX (Bit 25)                                */
#define VADC_GLOBRESD_EMUX_Msk                (0xe000000UL)             /*!< VADC GLOBRESD: EMUX (Bitfield-Mask: 0x07)                   */
#define VADC_GLOBRESD_CRS_Pos                 (28UL)                    /*!< VADC GLOBRESD: CRS (Bit 28)                                 */
#define VADC_GLOBRESD_CRS_Msk                 (0x30000000UL)            /*!< VADC GLOBRESD: CRS (Bitfield-Mask: 0x03)                    */
#define VADC_GLOBRESD_FCR_Pos                 (30UL)                    /*!< VADC GLOBRESD: FCR (Bit 30)                                 */
#define VADC_GLOBRESD_FCR_Msk                 (0x40000000UL)            /*!< VADC GLOBRESD: FCR (Bitfield-Mask: 0x01)                    */
#define VADC_GLOBRESD_VF_Pos                  (31UL)                    /*!< VADC GLOBRESD: VF (Bit 31)                                  */
#define VADC_GLOBRESD_VF_Msk                  (0x80000000UL)            /*!< VADC GLOBRESD: VF (Bitfield-Mask: 0x01)                     */


/* ================================================================================ */
/* ================           Group 'SHS' Position & Mask          ================ */
/* ================================================================================ */


/* -----------------------------------  SHS_ID  ----------------------------------- */
#define SHS_ID_MOD_REV_Pos                    (0UL)                     /*!< SHS ID: MOD_REV (Bit 0)                                     */
#define SHS_ID_MOD_REV_Msk                    (0xffUL)                  /*!< SHS ID: MOD_REV (Bitfield-Mask: 0xff)                       */
#define SHS_ID_MOD_TYPE_Pos                   (8UL)                     /*!< SHS ID: MOD_TYPE (Bit 8)                                    */
#define SHS_ID_MOD_TYPE_Msk                   (0xff00UL)                /*!< SHS ID: MOD_TYPE (Bitfield-Mask: 0xff)                      */
#define SHS_ID_MOD_NUMBER_Pos                 (16UL)                    /*!< SHS ID: MOD_NUMBER (Bit 16)                                 */
#define SHS_ID_MOD_NUMBER_Msk                 (0xffff0000UL)            /*!< SHS ID: MOD_NUMBER (Bitfield-Mask: 0xffff)                  */

/* ---------------------------------  SHS_SHSCFG  --------------------------------- */
#define SHS_SHSCFG_DIVS_Pos                   (0UL)                     /*!< SHS SHSCFG: DIVS (Bit 0)                                    */
#define SHS_SHSCFG_DIVS_Msk                   (0xfUL)                   /*!< SHS SHSCFG: DIVS (Bitfield-Mask: 0x0f)                      */
#define SHS_SHSCFG_AREF_Pos                   (10UL)                    /*!< SHS SHSCFG: AREF (Bit 10)                                   */
#define SHS_SHSCFG_AREF_Msk                   (0xc00UL)                 /*!< SHS SHSCFG: AREF (Bitfield-Mask: 0x03)                      */
#define SHS_SHSCFG_ANOFF_Pos                  (12UL)                    /*!< SHS SHSCFG: ANOFF (Bit 12)                                  */
#define SHS_SHSCFG_ANOFF_Msk                  (0x1000UL)                /*!< SHS SHSCFG: ANOFF (Bitfield-Mask: 0x01)                     */
#define SHS_SHSCFG_ANRDY_Pos                  (14UL)                    /*!< SHS SHSCFG: ANRDY (Bit 14)                                  */
#define SHS_SHSCFG_ANRDY_Msk                  (0x4000UL)                /*!< SHS SHSCFG: ANRDY (Bitfield-Mask: 0x01)                     */
#define SHS_SHSCFG_SCWC_Pos                   (15UL)                    /*!< SHS SHSCFG: SCWC (Bit 15)                                   */
#define SHS_SHSCFG_SCWC_Msk                   (0x8000UL)                /*!< SHS SHSCFG: SCWC (Bitfield-Mask: 0x01)                      */
#define SHS_SHSCFG_SP0_Pos                    (16UL)                    /*!< SHS SHSCFG: SP0 (Bit 16)                                    */
#define SHS_SHSCFG_SP0_Msk                    (0x10000UL)               /*!< SHS SHSCFG: SP0 (Bitfield-Mask: 0x01)                       */
#define SHS_SHSCFG_SP1_Pos                    (17UL)                    /*!< SHS SHSCFG: SP1 (Bit 17)                                    */
#define SHS_SHSCFG_SP1_Msk                    (0x20000UL)               /*!< SHS SHSCFG: SP1 (Bitfield-Mask: 0x01)                       */
#define SHS_SHSCFG_TC_Pos                     (24UL)                    /*!< SHS SHSCFG: TC (Bit 24)                                     */
#define SHS_SHSCFG_TC_Msk                     (0xf000000UL)             /*!< SHS SHSCFG: TC (Bitfield-Mask: 0x0f)                        */
#define SHS_SHSCFG_STATE_Pos                  (28UL)                    /*!< SHS SHSCFG: STATE (Bit 28)                                  */
#define SHS_SHSCFG_STATE_Msk                  (0xf0000000UL)            /*!< SHS SHSCFG: STATE (Bitfield-Mask: 0x0f)                     */

/* ---------------------------------  SHS_STEPCFG  -------------------------------- */
#define SHS_STEPCFG_KSEL0_Pos                 (0UL)                     /*!< SHS STEPCFG: KSEL0 (Bit 0)                                  */
#define SHS_STEPCFG_KSEL0_Msk                 (0x7UL)                   /*!< SHS STEPCFG: KSEL0 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN0_Pos                  (3UL)                     /*!< SHS STEPCFG: SEN0 (Bit 3)                                   */
#define SHS_STEPCFG_SEN0_Msk                  (0x8UL)                   /*!< SHS STEPCFG: SEN0 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL1_Pos                 (4UL)                     /*!< SHS STEPCFG: KSEL1 (Bit 4)                                  */
#define SHS_STEPCFG_KSEL1_Msk                 (0x70UL)                  /*!< SHS STEPCFG: KSEL1 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN1_Pos                  (7UL)                     /*!< SHS STEPCFG: SEN1 (Bit 7)                                   */
#define SHS_STEPCFG_SEN1_Msk                  (0x80UL)                  /*!< SHS STEPCFG: SEN1 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL2_Pos                 (8UL)                     /*!< SHS STEPCFG: KSEL2 (Bit 8)                                  */
#define SHS_STEPCFG_KSEL2_Msk                 (0x700UL)                 /*!< SHS STEPCFG: KSEL2 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN2_Pos                  (11UL)                    /*!< SHS STEPCFG: SEN2 (Bit 11)                                  */
#define SHS_STEPCFG_SEN2_Msk                  (0x800UL)                 /*!< SHS STEPCFG: SEN2 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL3_Pos                 (12UL)                    /*!< SHS STEPCFG: KSEL3 (Bit 12)                                 */
#define SHS_STEPCFG_KSEL3_Msk                 (0x7000UL)                /*!< SHS STEPCFG: KSEL3 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN3_Pos                  (15UL)                    /*!< SHS STEPCFG: SEN3 (Bit 15)                                  */
#define SHS_STEPCFG_SEN3_Msk                  (0x8000UL)                /*!< SHS STEPCFG: SEN3 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL4_Pos                 (16UL)                    /*!< SHS STEPCFG: KSEL4 (Bit 16)                                 */
#define SHS_STEPCFG_KSEL4_Msk                 (0x70000UL)               /*!< SHS STEPCFG: KSEL4 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN4_Pos                  (19UL)                    /*!< SHS STEPCFG: SEN4 (Bit 19)                                  */
#define SHS_STEPCFG_SEN4_Msk                  (0x80000UL)               /*!< SHS STEPCFG: SEN4 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL5_Pos                 (20UL)                    /*!< SHS STEPCFG: KSEL5 (Bit 20)                                 */
#define SHS_STEPCFG_KSEL5_Msk                 (0x700000UL)              /*!< SHS STEPCFG: KSEL5 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN5_Pos                  (23UL)                    /*!< SHS STEPCFG: SEN5 (Bit 23)                                  */
#define SHS_STEPCFG_SEN5_Msk                  (0x800000UL)              /*!< SHS STEPCFG: SEN5 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL6_Pos                 (24UL)                    /*!< SHS STEPCFG: KSEL6 (Bit 24)                                 */
#define SHS_STEPCFG_KSEL6_Msk                 (0x7000000UL)             /*!< SHS STEPCFG: KSEL6 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN6_Pos                  (27UL)                    /*!< SHS STEPCFG: SEN6 (Bit 27)                                  */
#define SHS_STEPCFG_SEN6_Msk                  (0x8000000UL)             /*!< SHS STEPCFG: SEN6 (Bitfield-Mask: 0x01)                     */
#define SHS_STEPCFG_KSEL7_Pos                 (28UL)                    /*!< SHS STEPCFG: KSEL7 (Bit 28)                                 */
#define SHS_STEPCFG_KSEL7_Msk                 (0x70000000UL)            /*!< SHS STEPCFG: KSEL7 (Bitfield-Mask: 0x07)                    */
#define SHS_STEPCFG_SEN7_Pos                  (31UL)                    /*!< SHS STEPCFG: SEN7 (Bit 31)                                  */
#define SHS_STEPCFG_SEN7_Msk                  (0x80000000UL)            /*!< SHS STEPCFG: SEN7 (Bitfield-Mask: 0x01)                     */

/* ----------------------------------  SHS_LOOP  ---------------------------------- */
#define SHS_LOOP_LPCH0_Pos                    (0UL)                     /*!< SHS LOOP: LPCH0 (Bit 0)                                     */
#define SHS_LOOP_LPCH0_Msk                    (0x1fUL)                  /*!< SHS LOOP: LPCH0 (Bitfield-Mask: 0x1f)                       */
#define SHS_LOOP_LPSH0_Pos                    (8UL)                     /*!< SHS LOOP: LPSH0 (Bit 8)                                     */
#define SHS_LOOP_LPSH0_Msk                    (0x100UL)                 /*!< SHS LOOP: LPSH0 (Bitfield-Mask: 0x01)                       */
#define SHS_LOOP_LPEN0_Pos                    (15UL)                    /*!< SHS LOOP: LPEN0 (Bit 15)                                    */
#define SHS_LOOP_LPEN0_Msk                    (0x8000UL)                /*!< SHS LOOP: LPEN0 (Bitfield-Mask: 0x01)                       */
#define SHS_LOOP_LPCH1_Pos                    (16UL)                    /*!< SHS LOOP: LPCH1 (Bit 16)                                    */
#define SHS_LOOP_LPCH1_Msk                    (0x1f0000UL)              /*!< SHS LOOP: LPCH1 (Bitfield-Mask: 0x1f)                       */
#define SHS_LOOP_LPSH1_Pos                    (24UL)                    /*!< SHS LOOP: LPSH1 (Bit 24)                                    */
#define SHS_LOOP_LPSH1_Msk                    (0x1000000UL)             /*!< SHS LOOP: LPSH1 (Bitfield-Mask: 0x01)                       */
#define SHS_LOOP_LPEN1_Pos                    (31UL)                    /*!< SHS LOOP: LPEN1 (Bit 31)                                    */
#define SHS_LOOP_LPEN1_Msk                    (0x80000000UL)            /*!< SHS LOOP: LPEN1 (Bitfield-Mask: 0x01)                       */

/* ---------------------------------  SHS_TIMCFG0  -------------------------------- */
#define SHS_TIMCFG0_AT_Pos                    (0UL)                     /*!< SHS TIMCFG0: AT (Bit 0)                                     */
#define SHS_TIMCFG0_AT_Msk                    (0x1UL)                   /*!< SHS TIMCFG0: AT (Bitfield-Mask: 0x01)                       */
#define SHS_TIMCFG0_FCRT_Pos                  (4UL)                     /*!< SHS TIMCFG0: FCRT (Bit 4)                                   */
#define SHS_TIMCFG0_FCRT_Msk                  (0xf0UL)                  /*!< SHS TIMCFG0: FCRT (Bitfield-Mask: 0x0f)                     */
#define SHS_TIMCFG0_SST_Pos                   (8UL)                     /*!< SHS TIMCFG0: SST (Bit 8)                                    */
#define SHS_TIMCFG0_SST_Msk                   (0x3f00UL)                /*!< SHS TIMCFG0: SST (Bitfield-Mask: 0x3f)                      */
#define SHS_TIMCFG0_TGEN_Pos                  (16UL)                    /*!< SHS TIMCFG0: TGEN (Bit 16)                                  */
#define SHS_TIMCFG0_TGEN_Msk                  (0x3fff0000UL)            /*!< SHS TIMCFG0: TGEN (Bitfield-Mask: 0x3fff)                   */

/* ---------------------------------  SHS_TIMCFG1  -------------------------------- */
#define SHS_TIMCFG1_AT_Pos                    (0UL)                     /*!< SHS TIMCFG1: AT (Bit 0)                                     */
#define SHS_TIMCFG1_AT_Msk                    (0x1UL)                   /*!< SHS TIMCFG1: AT (Bitfield-Mask: 0x01)                       */
#define SHS_TIMCFG1_FCRT_Pos                  (4UL)                     /*!< SHS TIMCFG1: FCRT (Bit 4)                                   */
#define SHS_TIMCFG1_FCRT_Msk                  (0xf0UL)                  /*!< SHS TIMCFG1: FCRT (Bitfield-Mask: 0x0f)                     */
#define SHS_TIMCFG1_SST_Pos                   (8UL)                     /*!< SHS TIMCFG1: SST (Bit 8)                                    */
#define SHS_TIMCFG1_SST_Msk                   (0x3f00UL)                /*!< SHS TIMCFG1: SST (Bitfield-Mask: 0x3f)                      */
#define SHS_TIMCFG1_TGEN_Pos                  (16UL)                    /*!< SHS TIMCFG1: TGEN (Bit 16)                                  */
#define SHS_TIMCFG1_TGEN_Msk                  (0x3fff0000UL)            /*!< SHS TIMCFG1: TGEN (Bitfield-Mask: 0x3fff)                   */

/* ---------------------------------  SHS_CALCTR  --------------------------------- */
#define SHS_CALCTR_CALORD_Pos                 (0UL)                     /*!< SHS CALCTR: CALORD (Bit 0)                                  */
#define SHS_CALCTR_CALORD_Msk                 (0x1UL)                   /*!< SHS CALCTR: CALORD (Bitfield-Mask: 0x01)                    */
#define SHS_CALCTR_CALGNSTC_Pos               (8UL)                     /*!< SHS CALCTR: CALGNSTC (Bit 8)                                */
#define SHS_CALCTR_CALGNSTC_Msk               (0x3f00UL)                /*!< SHS CALCTR: CALGNSTC (Bitfield-Mask: 0x3f)                  */
#define SHS_CALCTR_SUCALVAL_Pos               (16UL)                    /*!< SHS CALCTR: SUCALVAL (Bit 16)                               */
#define SHS_CALCTR_SUCALVAL_Msk               (0x7f0000UL)              /*!< SHS CALCTR: SUCALVAL (Bitfield-Mask: 0x7f)                  */
#define SHS_CALCTR_CALMAX_Pos                 (24UL)                    /*!< SHS CALCTR: CALMAX (Bit 24)                                 */
#define SHS_CALCTR_CALMAX_Msk                 (0x3f000000UL)            /*!< SHS CALCTR: CALMAX (Bitfield-Mask: 0x3f)                    */
#define SHS_CALCTR_SUCAL_Pos                  (31UL)                    /*!< SHS CALCTR: SUCAL (Bit 31)                                  */
#define SHS_CALCTR_SUCAL_Msk                  (0x80000000UL)            /*!< SHS CALCTR: SUCAL (Bitfield-Mask: 0x01)                     */

/* ---------------------------------  SHS_CALGC0  --------------------------------- */
#define SHS_CALGC0_CALGNVALS_Pos              (0UL)                     /*!< SHS CALGC0: CALGNVALS (Bit 0)                               */
#define SHS_CALGC0_CALGNVALS_Msk              (0x3fffUL)                /*!< SHS CALGC0: CALGNVALS (Bitfield-Mask: 0x3fff)               */
#define SHS_CALGC0_GNSWC_Pos                  (15UL)                    /*!< SHS CALGC0: GNSWC (Bit 15)                                  */
#define SHS_CALGC0_GNSWC_Msk                  (0x8000UL)                /*!< SHS CALGC0: GNSWC (Bitfield-Mask: 0x01)                     */
#define SHS_CALGC0_CALGNVALA_Pos              (16UL)                    /*!< SHS CALGC0: CALGNVALA (Bit 16)                              */
#define SHS_CALGC0_CALGNVALA_Msk              (0x3fff0000UL)            /*!< SHS CALGC0: CALGNVALA (Bitfield-Mask: 0x3fff)               */
#define SHS_CALGC0_GNAWC_Pos                  (31UL)                    /*!< SHS CALGC0: GNAWC (Bit 31)                                  */
#define SHS_CALGC0_GNAWC_Msk                  (0x80000000UL)            /*!< SHS CALGC0: GNAWC (Bitfield-Mask: 0x01)                     */

/* ---------------------------------  SHS_CALGC1  --------------------------------- */
#define SHS_CALGC1_CALGNVALS_Pos              (0UL)                     /*!< SHS CALGC1: CALGNVALS (Bit 0)                               */
#define SHS_CALGC1_CALGNVALS_Msk              (0x3fffUL)                /*!< SHS CALGC1: CALGNVALS (Bitfield-Mask: 0x3fff)               */
#define SHS_CALGC1_GNSWC_Pos                  (15UL)                    /*!< SHS CALGC1: GNSWC (Bit 15)                                  */
#define SHS_CALGC1_GNSWC_Msk                  (0x8000UL)                /*!< SHS CALGC1: GNSWC (Bitfield-Mask: 0x01)                     */
#define SHS_CALGC1_CALGNVALA_Pos              (16UL)                    /*!< SHS CALGC1: CALGNVALA (Bit 16)                              */
#define SHS_CALGC1_CALGNVALA_Msk              (0x3fff0000UL)            /*!< SHS CALGC1: CALGNVALA (Bitfield-Mask: 0x3fff)               */
#define SHS_CALGC1_GNAWC_Pos                  (31UL)                    /*!< SHS CALGC1: GNAWC (Bit 31)                                  */
#define SHS_CALGC1_GNAWC_Msk                  (0x80000000UL)            /*!< SHS CALGC1: GNAWC (Bitfield-Mask: 0x01)                     */

/* ---------------------------------  SHS_GNCTR00  -------------------------------- */
#define SHS_GNCTR00_GAIN0_Pos                 (0UL)                     /*!< SHS GNCTR00: GAIN0 (Bit 0)                                  */
#define SHS_GNCTR00_GAIN0_Msk                 (0xfUL)                   /*!< SHS GNCTR00: GAIN0 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN1_Pos                 (4UL)                     /*!< SHS GNCTR00: GAIN1 (Bit 4)                                  */
#define SHS_GNCTR00_GAIN1_Msk                 (0xf0UL)                  /*!< SHS GNCTR00: GAIN1 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN2_Pos                 (8UL)                     /*!< SHS GNCTR00: GAIN2 (Bit 8)                                  */
#define SHS_GNCTR00_GAIN2_Msk                 (0xf00UL)                 /*!< SHS GNCTR00: GAIN2 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN3_Pos                 (12UL)                    /*!< SHS GNCTR00: GAIN3 (Bit 12)                                 */
#define SHS_GNCTR00_GAIN3_Msk                 (0xf000UL)                /*!< SHS GNCTR00: GAIN3 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN4_Pos                 (16UL)                    /*!< SHS GNCTR00: GAIN4 (Bit 16)                                 */
#define SHS_GNCTR00_GAIN4_Msk                 (0xf0000UL)               /*!< SHS GNCTR00: GAIN4 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN5_Pos                 (20UL)                    /*!< SHS GNCTR00: GAIN5 (Bit 20)                                 */
#define SHS_GNCTR00_GAIN5_Msk                 (0xf00000UL)              /*!< SHS GNCTR00: GAIN5 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN6_Pos                 (24UL)                    /*!< SHS GNCTR00: GAIN6 (Bit 24)                                 */
#define SHS_GNCTR00_GAIN6_Msk                 (0xf000000UL)             /*!< SHS GNCTR00: GAIN6 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR00_GAIN7_Pos                 (28UL)                    /*!< SHS GNCTR00: GAIN7 (Bit 28)                                 */
#define SHS_GNCTR00_GAIN7_Msk                 (0xf0000000UL)            /*!< SHS GNCTR00: GAIN7 (Bitfield-Mask: 0x0f)                    */

/* ---------------------------------  SHS_GNCTR10  -------------------------------- */
#define SHS_GNCTR10_GAIN0_Pos                 (0UL)                     /*!< SHS GNCTR10: GAIN0 (Bit 0)                                  */
#define SHS_GNCTR10_GAIN0_Msk                 (0xfUL)                   /*!< SHS GNCTR10: GAIN0 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN1_Pos                 (4UL)                     /*!< SHS GNCTR10: GAIN1 (Bit 4)                                  */
#define SHS_GNCTR10_GAIN1_Msk                 (0xf0UL)                  /*!< SHS GNCTR10: GAIN1 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN2_Pos                 (8UL)                     /*!< SHS GNCTR10: GAIN2 (Bit 8)                                  */
#define SHS_GNCTR10_GAIN2_Msk                 (0xf00UL)                 /*!< SHS GNCTR10: GAIN2 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN3_Pos                 (12UL)                    /*!< SHS GNCTR10: GAIN3 (Bit 12)                                 */
#define SHS_GNCTR10_GAIN3_Msk                 (0xf000UL)                /*!< SHS GNCTR10: GAIN3 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN4_Pos                 (16UL)                    /*!< SHS GNCTR10: GAIN4 (Bit 16)                                 */
#define SHS_GNCTR10_GAIN4_Msk                 (0xf0000UL)               /*!< SHS GNCTR10: GAIN4 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN5_Pos                 (20UL)                    /*!< SHS GNCTR10: GAIN5 (Bit 20)                                 */
#define SHS_GNCTR10_GAIN5_Msk                 (0xf00000UL)              /*!< SHS GNCTR10: GAIN5 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN6_Pos                 (24UL)                    /*!< SHS GNCTR10: GAIN6 (Bit 24)                                 */
#define SHS_GNCTR10_GAIN6_Msk                 (0xf000000UL)             /*!< SHS GNCTR10: GAIN6 (Bitfield-Mask: 0x0f)                    */
#define SHS_GNCTR10_GAIN7_Pos                 (28UL)                    /*!< SHS GNCTR10: GAIN7 (Bit 28)                                 */
#define SHS_GNCTR10_GAIN7_Msk                 (0xf0000000UL)            /*!< SHS GNCTR10: GAIN7 (Bitfield-Mask: 0x0f)                    */


/* ================================================================================ */
/* ================         struct 'PORT0' Position & Mask         ================ */
/* ================================================================================ */


/* ----------------------------------  PORT0_OUT  --------------------------------- */
#define PORT0_OUT_P0_Pos                      (0UL)                     /*!< PORT0 OUT: P0 (Bit 0)                                       */
#define PORT0_OUT_P0_Msk                      (0x1UL)                   /*!< PORT0 OUT: P0 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P1_Pos                      (1UL)                     /*!< PORT0 OUT: P1 (Bit 1)                                       */
#define PORT0_OUT_P1_Msk                      (0x2UL)                   /*!< PORT0 OUT: P1 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P2_Pos                      (2UL)                     /*!< PORT0 OUT: P2 (Bit 2)                                       */
#define PORT0_OUT_P2_Msk                      (0x4UL)                   /*!< PORT0 OUT: P2 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P3_Pos                      (3UL)                     /*!< PORT0 OUT: P3 (Bit 3)                                       */
#define PORT0_OUT_P3_Msk                      (0x8UL)                   /*!< PORT0 OUT: P3 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P4_Pos                      (4UL)                     /*!< PORT0 OUT: P4 (Bit 4)                                       */
#define PORT0_OUT_P4_Msk                      (0x10UL)                  /*!< PORT0 OUT: P4 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P5_Pos                      (5UL)                     /*!< PORT0 OUT: P5 (Bit 5)                                       */
#define PORT0_OUT_P5_Msk                      (0x20UL)                  /*!< PORT0 OUT: P5 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P6_Pos                      (6UL)                     /*!< PORT0 OUT: P6 (Bit 6)                                       */
#define PORT0_OUT_P6_Msk                      (0x40UL)                  /*!< PORT0 OUT: P6 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P7_Pos                      (7UL)                     /*!< PORT0 OUT: P7 (Bit 7)                                       */
#define PORT0_OUT_P7_Msk                      (0x80UL)                  /*!< PORT0 OUT: P7 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P8_Pos                      (8UL)                     /*!< PORT0 OUT: P8 (Bit 8)                                       */
#define PORT0_OUT_P8_Msk                      (0x100UL)                 /*!< PORT0 OUT: P8 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P9_Pos                      (9UL)                     /*!< PORT0 OUT: P9 (Bit 9)                                       */
#define PORT0_OUT_P9_Msk                      (0x200UL)                 /*!< PORT0 OUT: P9 (Bitfield-Mask: 0x01)                         */
#define PORT0_OUT_P10_Pos                     (10UL)                    /*!< PORT0 OUT: P10 (Bit 10)                                     */
#define PORT0_OUT_P10_Msk                     (0x400UL)                 /*!< PORT0 OUT: P10 (Bitfield-Mask: 0x01)                        */
#define PORT0_OUT_P11_Pos                     (11UL)                    /*!< PORT0 OUT: P11 (Bit 11)                                     */
#define PORT0_OUT_P11_Msk                     (0x800UL)                 /*!< PORT0 OUT: P11 (Bitfield-Mask: 0x01)                        */
#define PORT0_OUT_P12_Pos                     (12UL)                    /*!< PORT0 OUT: P12 (Bit 12)                                     */
#define PORT0_OUT_P12_Msk                     (0x1000UL)                /*!< PORT0 OUT: P12 (Bitfield-Mask: 0x01)                        */
#define PORT0_OUT_P13_Pos                     (13UL)                    /*!< PORT0 OUT: P13 (Bit 13)                                     */
#define PORT0_OUT_P13_Msk                     (0x2000UL)                /*!< PORT0 OUT: P13 (Bitfield-Mask: 0x01)                        */
#define PORT0_OUT_P14_Pos                     (14UL)                    /*!< PORT0 OUT: P14 (Bit 14)                                     */
#define PORT0_OUT_P14_Msk                     (0x4000UL)                /*!< PORT0 OUT: P14 (Bitfield-Mask: 0x01)                        */
#define PORT0_OUT_P15_Pos                     (15UL)                    /*!< PORT0 OUT: P15 (Bit 15)                                     */
#define PORT0_OUT_P15_Msk                     (0x8000UL)                /*!< PORT0 OUT: P15 (Bitfield-Mask: 0x01)                        */

/* ----------------------------------  PORT0_OMR  --------------------------------- */
#define PORT0_OMR_PS0_Pos                     (0UL)                     /*!< PORT0 OMR: PS0 (Bit 0)                                      */
#define PORT0_OMR_PS0_Msk                     (0x1UL)                   /*!< PORT0 OMR: PS0 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS1_Pos                     (1UL)                     /*!< PORT0 OMR: PS1 (Bit 1)                                      */
#define PORT0_OMR_PS1_Msk                     (0x2UL)                   /*!< PORT0 OMR: PS1 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS2_Pos                     (2UL)                     /*!< PORT0 OMR: PS2 (Bit 2)                                      */
#define PORT0_OMR_PS2_Msk                     (0x4UL)                   /*!< PORT0 OMR: PS2 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS3_Pos                     (3UL)                     /*!< PORT0 OMR: PS3 (Bit 3)                                      */
#define PORT0_OMR_PS3_Msk                     (0x8UL)                   /*!< PORT0 OMR: PS3 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS4_Pos                     (4UL)                     /*!< PORT0 OMR: PS4 (Bit 4)                                      */
#define PORT0_OMR_PS4_Msk                     (0x10UL)                  /*!< PORT0 OMR: PS4 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS5_Pos                     (5UL)                     /*!< PORT0 OMR: PS5 (Bit 5)                                      */
#define PORT0_OMR_PS5_Msk                     (0x20UL)                  /*!< PORT0 OMR: PS5 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS6_Pos                     (6UL)                     /*!< PORT0 OMR: PS6 (Bit 6)                                      */
#define PORT0_OMR_PS6_Msk                     (0x40UL)                  /*!< PORT0 OMR: PS6 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS7_Pos                     (7UL)                     /*!< PORT0 OMR: PS7 (Bit 7)                                      */
#define PORT0_OMR_PS7_Msk                     (0x80UL)                  /*!< PORT0 OMR: PS7 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS8_Pos                     (8UL)                     /*!< PORT0 OMR: PS8 (Bit 8)                                      */
#define PORT0_OMR_PS8_Msk                     (0x100UL)                 /*!< PORT0 OMR: PS8 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS9_Pos                     (9UL)                     /*!< PORT0 OMR: PS9 (Bit 9)                                      */
#define PORT0_OMR_PS9_Msk                     (0x200UL)                 /*!< PORT0 OMR: PS9 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PS10_Pos                    (10UL)                    /*!< PORT0 OMR: PS10 (Bit 10)                                    */
#define PORT0_OMR_PS10_Msk                    (0x400UL)                 /*!< PORT0 OMR: PS10 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PS11_Pos                    (11UL)                    /*!< PORT0 OMR: PS11 (Bit 11)                                    */
#define PORT0_OMR_PS11_Msk                    (0x800UL)                 /*!< PORT0 OMR: PS11 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PS12_Pos                    (12UL)                    /*!< PORT0 OMR: PS12 (Bit 12)                                    */
#define PORT0_OMR_PS12_Msk                    (0x1000UL)                /*!< PORT0 OMR: PS12 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PS13_Pos                    (13UL)                    /*!< PORT0 OMR: PS13 (Bit 13)                                    */
#define PORT0_OMR_PS13_Msk                    (0x2000UL)                /*!< PORT0 OMR: PS13 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PS14_Pos                    (14UL)                    /*!< PORT0 OMR: PS14 (Bit 14)                                    */
#define PORT0_OMR_PS14_Msk                    (0x4000UL)                /*!< PORT0 OMR: PS14 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PS15_Pos                    (15UL)                    /*!< PORT0 OMR: PS15 (Bit 15)                                    */
#define PORT0_OMR_PS15_Msk                    (0x8000UL)                /*!< PORT0 OMR: PS15 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PR0_Pos                     (16UL)                    /*!< PORT0 OMR: PR0 (Bit 16)                                     */
#define PORT0_OMR_PR0_Msk                     (0x10000UL)               /*!< PORT0 OMR: PR0 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR1_Pos                     (17UL)                    /*!< PORT0 OMR: PR1 (Bit 17)                                     */
#define PORT0_OMR_PR1_Msk                     (0x20000UL)               /*!< PORT0 OMR: PR1 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR2_Pos                     (18UL)                    /*!< PORT0 OMR: PR2 (Bit 18)                                     */
#define PORT0_OMR_PR2_Msk                     (0x40000UL)               /*!< PORT0 OMR: PR2 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR3_Pos                     (19UL)                    /*!< PORT0 OMR: PR3 (Bit 19)                                     */
#define PORT0_OMR_PR3_Msk                     (0x80000UL)               /*!< PORT0 OMR: PR3 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR4_Pos                     (20UL)                    /*!< PORT0 OMR: PR4 (Bit 20)                                     */
#define PORT0_OMR_PR4_Msk                     (0x100000UL)              /*!< PORT0 OMR: PR4 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR5_Pos                     (21UL)                    /*!< PORT0 OMR: PR5 (Bit 21)                                     */
#define PORT0_OMR_PR5_Msk                     (0x200000UL)              /*!< PORT0 OMR: PR5 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR6_Pos                     (22UL)                    /*!< PORT0 OMR: PR6 (Bit 22)                                     */
#define PORT0_OMR_PR6_Msk                     (0x400000UL)              /*!< PORT0 OMR: PR6 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR7_Pos                     (23UL)                    /*!< PORT0 OMR: PR7 (Bit 23)                                     */
#define PORT0_OMR_PR7_Msk                     (0x800000UL)              /*!< PORT0 OMR: PR7 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR8_Pos                     (24UL)                    /*!< PORT0 OMR: PR8 (Bit 24)                                     */
#define PORT0_OMR_PR8_Msk                     (0x1000000UL)             /*!< PORT0 OMR: PR8 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR9_Pos                     (25UL)                    /*!< PORT0 OMR: PR9 (Bit 25)                                     */
#define PORT0_OMR_PR9_Msk                     (0x2000000UL)             /*!< PORT0 OMR: PR9 (Bitfield-Mask: 0x01)                        */
#define PORT0_OMR_PR10_Pos                    (26UL)                    /*!< PORT0 OMR: PR10 (Bit 26)                                    */
#define PORT0_OMR_PR10_Msk                    (0x4000000UL)             /*!< PORT0 OMR: PR10 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PR11_Pos                    (27UL)                    /*!< PORT0 OMR: PR11 (Bit 27)                                    */
#define PORT0_OMR_PR11_Msk                    (0x8000000UL)             /*!< PORT0 OMR: PR11 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PR12_Pos                    (28UL)                    /*!< PORT0 OMR: PR12 (Bit 28)                                    */
#define PORT0_OMR_PR12_Msk                    (0x10000000UL)            /*!< PORT0 OMR: PR12 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PR13_Pos                    (29UL)                    /*!< PORT0 OMR: PR13 (Bit 29)                                    */
#define PORT0_OMR_PR13_Msk                    (0x20000000UL)            /*!< PORT0 OMR: PR13 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PR14_Pos                    (30UL)                    /*!< PORT0 OMR: PR14 (Bit 30)                                    */
#define PORT0_OMR_PR14_Msk                    (0x40000000UL)            /*!< PORT0 OMR: PR14 (Bitfield-Mask: 0x01)                       */
#define PORT0_OMR_PR15_Pos                    (31UL)                    /*!< PORT0 OMR: PR15 (Bit 31)                                    */
#define PORT0_OMR_PR15_Msk                    (0x80000000UL)            /*!< PORT0 OMR: PR15 (Bitfield-Mask: 0x01)                       */

/* ---------------------------------  PORT0_IOCR0  -------------------------------- */
#define PORT0_IOCR0_PC0_Pos                   (3UL)                     /*!< PORT0 IOCR0: PC0 (Bit 3)                                    */
#define PORT0_IOCR0_PC0_Msk                   (0xf8UL)                  /*!< PORT0 IOCR0: PC0 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR0_PC1_Pos                   (11UL)                    /*!< PORT0 IOCR0: PC1 (Bit 11)                                   */
#define PORT0_IOCR0_PC1_Msk                   (0xf800UL)                /*!< PORT0 IOCR0: PC1 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR0_PC2_Pos                   (19UL)                    /*!< PORT0 IOCR0: PC2 (Bit 19)                                   */
#define PORT0_IOCR0_PC2_Msk                   (0xf80000UL)              /*!< PORT0 IOCR0: PC2 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR0_PC3_Pos                   (27UL)                    /*!< PORT0 IOCR0: PC3 (Bit 27)                                   */
#define PORT0_IOCR0_PC3_Msk                   (0xf8000000UL)            /*!< PORT0 IOCR0: PC3 (Bitfield-Mask: 0x1f)                      */

/* ---------------------------------  PORT0_IOCR4  -------------------------------- */
#define PORT0_IOCR4_PC4_Pos                   (3UL)                     /*!< PORT0 IOCR4: PC4 (Bit 3)                                    */
#define PORT0_IOCR4_PC4_Msk                   (0xf8UL)                  /*!< PORT0 IOCR4: PC4 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR4_PC5_Pos                   (11UL)                    /*!< PORT0 IOCR4: PC5 (Bit 11)                                   */
#define PORT0_IOCR4_PC5_Msk                   (0xf800UL)                /*!< PORT0 IOCR4: PC5 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR4_PC6_Pos                   (19UL)                    /*!< PORT0 IOCR4: PC6 (Bit 19)                                   */
#define PORT0_IOCR4_PC6_Msk                   (0xf80000UL)              /*!< PORT0 IOCR4: PC6 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR4_PC7_Pos                   (27UL)                    /*!< PORT0 IOCR4: PC7 (Bit 27)                                   */
#define PORT0_IOCR4_PC7_Msk                   (0xf8000000UL)            /*!< PORT0 IOCR4: PC7 (Bitfield-Mask: 0x1f)                      */

/* ---------------------------------  PORT0_IOCR8  -------------------------------- */
#define PORT0_IOCR8_PC8_Pos                   (3UL)                     /*!< PORT0 IOCR8: PC8 (Bit 3)                                    */
#define PORT0_IOCR8_PC8_Msk                   (0xf8UL)                  /*!< PORT0 IOCR8: PC8 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR8_PC9_Pos                   (11UL)                    /*!< PORT0 IOCR8: PC9 (Bit 11)                                   */
#define PORT0_IOCR8_PC9_Msk                   (0xf800UL)                /*!< PORT0 IOCR8: PC9 (Bitfield-Mask: 0x1f)                      */
#define PORT0_IOCR8_PC10_Pos                  (19UL)                    /*!< PORT0 IOCR8: PC10 (Bit 19)                                  */
#define PORT0_IOCR8_PC10_Msk                  (0xf80000UL)              /*!< PORT0 IOCR8: PC10 (Bitfield-Mask: 0x1f)                     */
#define PORT0_IOCR8_PC11_Pos                  (27UL)                    /*!< PORT0 IOCR8: PC11 (Bit 27)                                  */
#define PORT0_IOCR8_PC11_Msk                  (0xf8000000UL)            /*!< PORT0 IOCR8: PC11 (Bitfield-Mask: 0x1f)                     */

/* --------------------------------  PORT0_IOCR12  -------------------------------- */
#define PORT0_IOCR12_PC12_Pos                 (3UL)                     /*!< PORT0 IOCR12: PC12 (Bit 3)                                  */
#define PORT0_IOCR12_PC12_Msk                 (0xf8UL)                  /*!< PORT0 IOCR12: PC12 (Bitfield-Mask: 0x1f)                    */
#define PORT0_IOCR12_PC13_Pos                 (11UL)                    /*!< PORT0 IOCR12: PC13 (Bit 11)                                 */
#define PORT0_IOCR12_PC13_Msk                 (0xf800UL)                /*!< PORT0 IOCR12: PC13 (Bitfield-Mask: 0x1f)                    */
#define PORT0_IOCR12_PC14_Pos                 (19UL)                    /*!< PORT0 IOCR12: PC14 (Bit 19)                                 */
#define PORT0_IOCR12_PC14_Msk                 (0xf80000UL)              /*!< PORT0 IOCR12: PC14 (Bitfield-Mask: 0x1f)                    */
#define PORT0_IOCR12_PC15_Pos                 (27UL)                    /*!< PORT0 IOCR12: PC15 (Bit 27)                                 */
#define PORT0_IOCR12_PC15_Msk                 (0xf8000000UL)            /*!< PORT0 IOCR12: PC15 (Bitfield-Mask: 0x1f)                    */

/* ----------------------------------  PORT0_IN  ---------------------------------- */
#define PORT0_IN_P0_Pos                       (0UL)                     /*!< PORT0 IN: P0 (Bit 0)                                        */
#define PORT0_IN_P0_Msk                       (0x1UL)                   /*!< PORT0 IN: P0 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P1_Pos                       (1UL)                     /*!< PORT0 IN: P1 (Bit 1)                                        */
#define PORT0_IN_P1_Msk                       (0x2UL)                   /*!< PORT0 IN: P1 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P2_Pos                       (2UL)                     /*!< PORT0 IN: P2 (Bit 2)                                        */
#define PORT0_IN_P2_Msk                       (0x4UL)                   /*!< PORT0 IN: P2 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P3_Pos                       (3UL)                     /*!< PORT0 IN: P3 (Bit 3)                                        */
#define PORT0_IN_P3_Msk                       (0x8UL)                   /*!< PORT0 IN: P3 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P4_Pos                       (4UL)                     /*!< PORT0 IN: P4 (Bit 4)                                        */
#define PORT0_IN_P4_Msk                       (0x10UL)                  /*!< PORT0 IN: P4 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P5_Pos                       (5UL)                     /*!< PORT0 IN: P5 (Bit 5)                                        */
#define PORT0_IN_P5_Msk                       (0x20UL)                  /*!< PORT0 IN: P5 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P6_Pos                       (6UL)                     /*!< PORT0 IN: P6 (Bit 6)                                        */
#define PORT0_IN_P6_Msk                       (0x40UL)                  /*!< PORT0 IN: P6 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P7_Pos                       (7UL)                     /*!< PORT0 IN: P7 (Bit 7)                                        */
#define PORT0_IN_P7_Msk                       (0x80UL)                  /*!< PORT0 IN: P7 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P8_Pos                       (8UL)                     /*!< PORT0 IN: P8 (Bit 8)                                        */
#define PORT0_IN_P8_Msk                       (0x100UL)                 /*!< PORT0 IN: P8 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P9_Pos                       (9UL)                     /*!< PORT0 IN: P9 (Bit 9)                                        */
#define PORT0_IN_P9_Msk                       (0x200UL)                 /*!< PORT0 IN: P9 (Bitfield-Mask: 0x01)                          */
#define PORT0_IN_P10_Pos                      (10UL)                    /*!< PORT0 IN: P10 (Bit 10)                                      */
#define PORT0_IN_P10_Msk                      (0x400UL)                 /*!< PORT0 IN: P10 (Bitfield-Mask: 0x01)                         */
#define PORT0_IN_P11_Pos                      (11UL)                    /*!< PORT0 IN: P11 (Bit 11)                                      */
#define PORT0_IN_P11_Msk                      (0x800UL)                 /*!< PORT0 IN: P11 (Bitfield-Mask: 0x01)                         */
#define PORT0_IN_P12_Pos                      (12UL)                    /*!< PORT0 IN: P12 (Bit 12)                                      */
#define PORT0_IN_P12_Msk                      (0x1000UL)                /*!< PORT0 IN: P12 (Bitfield-Mask: 0x01)                         */
#define PORT0_IN_P13_Pos                      (13UL)                    /*!< PORT0 IN: P13 (Bit 13)                                      */
#define PORT0_IN_P13_Msk                      (0x2000UL)                /*!< PORT0 IN: P13 (Bitfield-Mask: 0x01)                         */
#define PORT0_IN_P14_Pos                      (14UL)                    /*!< PORT0 IN: P14 (Bit 14)                                      */
#define PORT0_IN_P14_Msk                      (0x4000UL)                /*!< PORT0 IN: P14 (Bitfield-Mask: 0x01)                         */
#define PORT0_IN_P15_Pos                      (15UL)                    /*!< PORT0 IN: P15 (Bit 15)                                      */
#define PORT0_IN_P15_Msk                      (0x8000UL)                /*!< PORT0 IN: P15 (Bitfield-Mask: 0x01)                         */

/* ---------------------------------  PORT0_PHCR0  -------------------------------- */
#define PORT0_PHCR0_PH0_Pos                   (2UL)                     /*!< PORT0 PHCR0: PH0 (Bit 2)                                    */
#define PORT0_PHCR0_PH0_Msk                   (0x4UL)                   /*!< PORT0 PHCR0: PH0 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH1_Pos                   (6UL)                     /*!< PORT0 PHCR0: PH1 (Bit 6)                                    */
#define PORT0_PHCR0_PH1_Msk                   (0x40UL)                  /*!< PORT0 PHCR0: PH1 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH2_Pos                   (10UL)                    /*!< PORT0 PHCR0: PH2 (Bit 10)                                   */
#define PORT0_PHCR0_PH2_Msk                   (0x400UL)                 /*!< PORT0 PHCR0: PH2 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH3_Pos                   (14UL)                    /*!< PORT0 PHCR0: PH3 (Bit 14)                                   */
#define PORT0_PHCR0_PH3_Msk                   (0x4000UL)                /*!< PORT0 PHCR0: PH3 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH4_Pos                   (18UL)                    /*!< PORT0 PHCR0: PH4 (Bit 18)                                   */
#define PORT0_PHCR0_PH4_Msk                   (0x40000UL)               /*!< PORT0 PHCR0: PH4 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH5_Pos                   (22UL)                    /*!< PORT0 PHCR0: PH5 (Bit 22)                                   */
#define PORT0_PHCR0_PH5_Msk                   (0x400000UL)              /*!< PORT0 PHCR0: PH5 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH6_Pos                   (26UL)                    /*!< PORT0 PHCR0: PH6 (Bit 26)                                   */
#define PORT0_PHCR0_PH6_Msk                   (0x4000000UL)             /*!< PORT0 PHCR0: PH6 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR0_PH7_Pos                   (30UL)                    /*!< PORT0 PHCR0: PH7 (Bit 30)                                   */
#define PORT0_PHCR0_PH7_Msk                   (0x40000000UL)            /*!< PORT0 PHCR0: PH7 (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  PORT0_PHCR1  -------------------------------- */
#define PORT0_PHCR1_PH8_Pos                   (2UL)                     /*!< PORT0 PHCR1: PH8 (Bit 2)                                    */
#define PORT0_PHCR1_PH8_Msk                   (0x4UL)                   /*!< PORT0 PHCR1: PH8 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR1_PH9_Pos                   (6UL)                     /*!< PORT0 PHCR1: PH9 (Bit 6)                                    */
#define PORT0_PHCR1_PH9_Msk                   (0x40UL)                  /*!< PORT0 PHCR1: PH9 (Bitfield-Mask: 0x01)                      */
#define PORT0_PHCR1_PH10_Pos                  (10UL)                    /*!< PORT0 PHCR1: PH10 (Bit 10)                                  */
#define PORT0_PHCR1_PH10_Msk                  (0x400UL)                 /*!< PORT0 PHCR1: PH10 (Bitfield-Mask: 0x01)                     */
#define PORT0_PHCR1_PH11_Pos                  (14UL)                    /*!< PORT0 PHCR1: PH11 (Bit 14)                                  */
#define PORT0_PHCR1_PH11_Msk                  (0x4000UL)                /*!< PORT0 PHCR1: PH11 (Bitfield-Mask: 0x01)                     */
#define PORT0_PHCR1_PH12_Pos                  (18UL)                    /*!< PORT0 PHCR1: PH12 (Bit 18)                                  */
#define PORT0_PHCR1_PH12_Msk                  (0x40000UL)               /*!< PORT0 PHCR1: PH12 (Bitfield-Mask: 0x01)                     */
#define PORT0_PHCR1_PH13_Pos                  (22UL)                    /*!< PORT0 PHCR1: PH13 (Bit 22)                                  */
#define PORT0_PHCR1_PH13_Msk                  (0x400000UL)              /*!< PORT0 PHCR1: PH13 (Bitfield-Mask: 0x01)                     */
#define PORT0_PHCR1_PH14_Pos                  (26UL)                    /*!< PORT0 PHCR1: PH14 (Bit 26)                                  */
#define PORT0_PHCR1_PH14_Msk                  (0x4000000UL)             /*!< PORT0 PHCR1: PH14 (Bitfield-Mask: 0x01)                     */
#define PORT0_PHCR1_PH15_Pos                  (30UL)                    /*!< PORT0 PHCR1: PH15 (Bit 30)                                  */
#define PORT0_PHCR1_PH15_Msk                  (0x40000000UL)            /*!< PORT0 PHCR1: PH15 (Bitfield-Mask: 0x01)                     */

/* ---------------------------------  PORT0_PDISC  -------------------------------- */
#define PORT0_PDISC_PDIS0_Pos                 (0UL)                     /*!< PORT0 PDISC: PDIS0 (Bit 0)                                  */
#define PORT0_PDISC_PDIS0_Msk                 (0x1UL)                   /*!< PORT0 PDISC: PDIS0 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS1_Pos                 (1UL)                     /*!< PORT0 PDISC: PDIS1 (Bit 1)                                  */
#define PORT0_PDISC_PDIS1_Msk                 (0x2UL)                   /*!< PORT0 PDISC: PDIS1 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS2_Pos                 (2UL)                     /*!< PORT0 PDISC: PDIS2 (Bit 2)                                  */
#define PORT0_PDISC_PDIS2_Msk                 (0x4UL)                   /*!< PORT0 PDISC: PDIS2 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS3_Pos                 (3UL)                     /*!< PORT0 PDISC: PDIS3 (Bit 3)                                  */
#define PORT0_PDISC_PDIS3_Msk                 (0x8UL)                   /*!< PORT0 PDISC: PDIS3 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS4_Pos                 (4UL)                     /*!< PORT0 PDISC: PDIS4 (Bit 4)                                  */
#define PORT0_PDISC_PDIS4_Msk                 (0x10UL)                  /*!< PORT0 PDISC: PDIS4 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS5_Pos                 (5UL)                     /*!< PORT0 PDISC: PDIS5 (Bit 5)                                  */
#define PORT0_PDISC_PDIS5_Msk                 (0x20UL)                  /*!< PORT0 PDISC: PDIS5 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS6_Pos                 (6UL)                     /*!< PORT0 PDISC: PDIS6 (Bit 6)                                  */
#define PORT0_PDISC_PDIS6_Msk                 (0x40UL)                  /*!< PORT0 PDISC: PDIS6 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS7_Pos                 (7UL)                     /*!< PORT0 PDISC: PDIS7 (Bit 7)                                  */
#define PORT0_PDISC_PDIS7_Msk                 (0x80UL)                  /*!< PORT0 PDISC: PDIS7 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS8_Pos                 (8UL)                     /*!< PORT0 PDISC: PDIS8 (Bit 8)                                  */
#define PORT0_PDISC_PDIS8_Msk                 (0x100UL)                 /*!< PORT0 PDISC: PDIS8 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS9_Pos                 (9UL)                     /*!< PORT0 PDISC: PDIS9 (Bit 9)                                  */
#define PORT0_PDISC_PDIS9_Msk                 (0x200UL)                 /*!< PORT0 PDISC: PDIS9 (Bitfield-Mask: 0x01)                    */
#define PORT0_PDISC_PDIS10_Pos                (10UL)                    /*!< PORT0 PDISC: PDIS10 (Bit 10)                                */
#define PORT0_PDISC_PDIS10_Msk                (0x400UL)                 /*!< PORT0 PDISC: PDIS10 (Bitfield-Mask: 0x01)                   */
#define PORT0_PDISC_PDIS11_Pos                (11UL)                    /*!< PORT0 PDISC: PDIS11 (Bit 11)                                */
#define PORT0_PDISC_PDIS11_Msk                (0x800UL)                 /*!< PORT0 PDISC: PDIS11 (Bitfield-Mask: 0x01)                   */
#define PORT0_PDISC_PDIS12_Pos                (12UL)                    /*!< PORT0 PDISC: PDIS12 (Bit 12)                                */
#define PORT0_PDISC_PDIS12_Msk                (0x1000UL)                /*!< PORT0 PDISC: PDIS12 (Bitfield-Mask: 0x01)                   */
#define PORT0_PDISC_PDIS13_Pos                (13UL)                    /*!< PORT0 PDISC: PDIS13 (Bit 13)                                */
#define PORT0_PDISC_PDIS13_Msk                (0x2000UL)                /*!< PORT0 PDISC: PDIS13 (Bitfield-Mask: 0x01)                   */
#define PORT0_PDISC_PDIS14_Pos                (14UL)                    /*!< PORT0 PDISC: PDIS14 (Bit 14)                                */
#define PORT0_PDISC_PDIS14_Msk                (0x4000UL)                /*!< PORT0 PDISC: PDIS14 (Bitfield-Mask: 0x01)                   */
#define PORT0_PDISC_PDIS15_Pos                (15UL)                    /*!< PORT0 PDISC: PDIS15 (Bit 15)                                */
#define PORT0_PDISC_PDIS15_Msk                (0x8000UL)                /*!< PORT0 PDISC: PDIS15 (Bitfield-Mask: 0x01)                   */

/* ----------------------------------  PORT0_PPS  --------------------------------- */
#define PORT0_PPS_PPS0_Pos                    (0UL)                     /*!< PORT0 PPS: PPS0 (Bit 0)                                     */
#define PORT0_PPS_PPS0_Msk                    (0x1UL)                   /*!< PORT0 PPS: PPS0 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS1_Pos                    (1UL)                     /*!< PORT0 PPS: PPS1 (Bit 1)                                     */
#define PORT0_PPS_PPS1_Msk                    (0x2UL)                   /*!< PORT0 PPS: PPS1 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS2_Pos                    (2UL)                     /*!< PORT0 PPS: PPS2 (Bit 2)                                     */
#define PORT0_PPS_PPS2_Msk                    (0x4UL)                   /*!< PORT0 PPS: PPS2 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS3_Pos                    (3UL)                     /*!< PORT0 PPS: PPS3 (Bit 3)                                     */
#define PORT0_PPS_PPS3_Msk                    (0x8UL)                   /*!< PORT0 PPS: PPS3 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS4_Pos                    (4UL)                     /*!< PORT0 PPS: PPS4 (Bit 4)                                     */
#define PORT0_PPS_PPS4_Msk                    (0x10UL)                  /*!< PORT0 PPS: PPS4 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS5_Pos                    (5UL)                     /*!< PORT0 PPS: PPS5 (Bit 5)                                     */
#define PORT0_PPS_PPS5_Msk                    (0x20UL)                  /*!< PORT0 PPS: PPS5 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS6_Pos                    (6UL)                     /*!< PORT0 PPS: PPS6 (Bit 6)                                     */
#define PORT0_PPS_PPS6_Msk                    (0x40UL)                  /*!< PORT0 PPS: PPS6 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS7_Pos                    (7UL)                     /*!< PORT0 PPS: PPS7 (Bit 7)                                     */
#define PORT0_PPS_PPS7_Msk                    (0x80UL)                  /*!< PORT0 PPS: PPS7 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS8_Pos                    (8UL)                     /*!< PORT0 PPS: PPS8 (Bit 8)                                     */
#define PORT0_PPS_PPS8_Msk                    (0x100UL)                 /*!< PORT0 PPS: PPS8 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS9_Pos                    (9UL)                     /*!< PORT0 PPS: PPS9 (Bit 9)                                     */
#define PORT0_PPS_PPS9_Msk                    (0x200UL)                 /*!< PORT0 PPS: PPS9 (Bitfield-Mask: 0x01)                       */
#define PORT0_PPS_PPS10_Pos                   (10UL)                    /*!< PORT0 PPS: PPS10 (Bit 10)                                   */
#define PORT0_PPS_PPS10_Msk                   (0x400UL)                 /*!< PORT0 PPS: PPS10 (Bitfield-Mask: 0x01)                      */
#define PORT0_PPS_PPS11_Pos                   (11UL)                    /*!< PORT0 PPS: PPS11 (Bit 11)                                   */
#define PORT0_PPS_PPS11_Msk                   (0x800UL)                 /*!< PORT0 PPS: PPS11 (Bitfield-Mask: 0x01)                      */
#define PORT0_PPS_PPS12_Pos                   (12UL)                    /*!< PORT0 PPS: PPS12 (Bit 12)                                   */
#define PORT0_PPS_PPS12_Msk                   (0x1000UL)                /*!< PORT0 PPS: PPS12 (Bitfield-Mask: 0x01)                      */
#define PORT0_PPS_PPS13_Pos                   (13UL)                    /*!< PORT0 PPS: PPS13 (Bit 13)                                   */
#define PORT0_PPS_PPS13_Msk                   (0x2000UL)                /*!< PORT0 PPS: PPS13 (Bitfield-Mask: 0x01)                      */
#define PORT0_PPS_PPS14_Pos                   (14UL)                    /*!< PORT0 PPS: PPS14 (Bit 14)                                   */
#define PORT0_PPS_PPS14_Msk                   (0x4000UL)                /*!< PORT0 PPS: PPS14 (Bitfield-Mask: 0x01)                      */
#define PORT0_PPS_PPS15_Pos                   (15UL)                    /*!< PORT0 PPS: PPS15 (Bit 15)                                   */
#define PORT0_PPS_PPS15_Msk                   (0x8000UL)                /*!< PORT0 PPS: PPS15 (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  PORT0_HWSEL  -------------------------------- */
#define PORT0_HWSEL_HW0_Pos                   (0UL)                     /*!< PORT0 HWSEL: HW0 (Bit 0)                                    */
#define PORT0_HWSEL_HW0_Msk                   (0x3UL)                   /*!< PORT0 HWSEL: HW0 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW1_Pos                   (2UL)                     /*!< PORT0 HWSEL: HW1 (Bit 2)                                    */
#define PORT0_HWSEL_HW1_Msk                   (0xcUL)                   /*!< PORT0 HWSEL: HW1 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW2_Pos                   (4UL)                     /*!< PORT0 HWSEL: HW2 (Bit 4)                                    */
#define PORT0_HWSEL_HW2_Msk                   (0x30UL)                  /*!< PORT0 HWSEL: HW2 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW3_Pos                   (6UL)                     /*!< PORT0 HWSEL: HW3 (Bit 6)                                    */
#define PORT0_HWSEL_HW3_Msk                   (0xc0UL)                  /*!< PORT0 HWSEL: HW3 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW4_Pos                   (8UL)                     /*!< PORT0 HWSEL: HW4 (Bit 8)                                    */
#define PORT0_HWSEL_HW4_Msk                   (0x300UL)                 /*!< PORT0 HWSEL: HW4 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW5_Pos                   (10UL)                    /*!< PORT0 HWSEL: HW5 (Bit 10)                                   */
#define PORT0_HWSEL_HW5_Msk                   (0xc00UL)                 /*!< PORT0 HWSEL: HW5 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW6_Pos                   (12UL)                    /*!< PORT0 HWSEL: HW6 (Bit 12)                                   */
#define PORT0_HWSEL_HW6_Msk                   (0x3000UL)                /*!< PORT0 HWSEL: HW6 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW7_Pos                   (14UL)                    /*!< PORT0 HWSEL: HW7 (Bit 14)                                   */
#define PORT0_HWSEL_HW7_Msk                   (0xc000UL)                /*!< PORT0 HWSEL: HW7 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW8_Pos                   (16UL)                    /*!< PORT0 HWSEL: HW8 (Bit 16)                                   */
#define PORT0_HWSEL_HW8_Msk                   (0x30000UL)               /*!< PORT0 HWSEL: HW8 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW9_Pos                   (18UL)                    /*!< PORT0 HWSEL: HW9 (Bit 18)                                   */
#define PORT0_HWSEL_HW9_Msk                   (0xc0000UL)               /*!< PORT0 HWSEL: HW9 (Bitfield-Mask: 0x03)                      */
#define PORT0_HWSEL_HW10_Pos                  (20UL)                    /*!< PORT0 HWSEL: HW10 (Bit 20)                                  */
#define PORT0_HWSEL_HW10_Msk                  (0x300000UL)              /*!< PORT0 HWSEL: HW10 (Bitfield-Mask: 0x03)                     */
#define PORT0_HWSEL_HW11_Pos                  (22UL)                    /*!< PORT0 HWSEL: HW11 (Bit 22)                                  */
#define PORT0_HWSEL_HW11_Msk                  (0xc00000UL)              /*!< PORT0 HWSEL: HW11 (Bitfield-Mask: 0x03)                     */
#define PORT0_HWSEL_HW12_Pos                  (24UL)                    /*!< PORT0 HWSEL: HW12 (Bit 24)                                  */
#define PORT0_HWSEL_HW12_Msk                  (0x3000000UL)             /*!< PORT0 HWSEL: HW12 (Bitfield-Mask: 0x03)                     */
#define PORT0_HWSEL_HW13_Pos                  (26UL)                    /*!< PORT0 HWSEL: HW13 (Bit 26)                                  */
#define PORT0_HWSEL_HW13_Msk                  (0xc000000UL)             /*!< PORT0 HWSEL: HW13 (Bitfield-Mask: 0x03)                     */
#define PORT0_HWSEL_HW14_Pos                  (28UL)                    /*!< PORT0 HWSEL: HW14 (Bit 28)                                  */
#define PORT0_HWSEL_HW14_Msk                  (0x30000000UL)            /*!< PORT0 HWSEL: HW14 (Bitfield-Mask: 0x03)                     */
#define PORT0_HWSEL_HW15_Pos                  (30UL)                    /*!< PORT0 HWSEL: HW15 (Bit 30)                                  */
#define PORT0_HWSEL_HW15_Msk                  (0xc0000000UL)            /*!< PORT0 HWSEL: HW15 (Bitfield-Mask: 0x03)                     */


/* ================================================================================ */
/* ================         struct 'PORT1' Position & Mask         ================ */
/* ================================================================================ */


/* ----------------------------------  PORT1_OUT  --------------------------------- */
#define PORT1_OUT_P0_Pos                      (0UL)                     /*!< PORT1 OUT: P0 (Bit 0)                                       */
#define PORT1_OUT_P0_Msk                      (0x1UL)                   /*!< PORT1 OUT: P0 (Bitfield-Mask: 0x01)                         */
#define PORT1_OUT_P1_Pos                      (1UL)                     /*!< PORT1 OUT: P1 (Bit 1)                                       */
#define PORT1_OUT_P1_Msk                      (0x2UL)                   /*!< PORT1 OUT: P1 (Bitfield-Mask: 0x01)                         */
#define PORT1_OUT_P2_Pos                      (2UL)                     /*!< PORT1 OUT: P2 (Bit 2)                                       */
#define PORT1_OUT_P2_Msk                      (0x4UL)                   /*!< PORT1 OUT: P2 (Bitfield-Mask: 0x01)                         */
#define PORT1_OUT_P3_Pos                      (3UL)                     /*!< PORT1 OUT: P3 (Bit 3)                                       */
#define PORT1_OUT_P3_Msk                      (0x8UL)                   /*!< PORT1 OUT: P3 (Bitfield-Mask: 0x01)                         */
#define PORT1_OUT_P4_Pos                      (4UL)                     /*!< PORT1 OUT: P4 (Bit 4)                                       */
#define PORT1_OUT_P4_Msk                      (0x10UL)                  /*!< PORT1 OUT: P4 (Bitfield-Mask: 0x01)                         */
#define PORT1_OUT_P5_Pos                      (5UL)                     /*!< PORT1 OUT: P5 (Bit 5)                                       */
#define PORT1_OUT_P5_Msk                      (0x20UL)                  /*!< PORT1 OUT: P5 (Bitfield-Mask: 0x01)                         */
#define PORT1_OUT_P6_Pos                      (6UL)                     /*!< PORT1 OUT: P6 (Bit 6)                                       */
#define PORT1_OUT_P6_Msk                      (0x40UL)                  /*!< PORT1 OUT: P6 (Bitfield-Mask: 0x01)                         */

/* ----------------------------------  PORT1_OMR  --------------------------------- */
#define PORT1_OMR_PS0_Pos                     (0UL)                     /*!< PORT1 OMR: PS0 (Bit 0)                                      */
#define PORT1_OMR_PS0_Msk                     (0x1UL)                   /*!< PORT1 OMR: PS0 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PS1_Pos                     (1UL)                     /*!< PORT1 OMR: PS1 (Bit 1)                                      */
#define PORT1_OMR_PS1_Msk                     (0x2UL)                   /*!< PORT1 OMR: PS1 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PS2_Pos                     (2UL)                     /*!< PORT1 OMR: PS2 (Bit 2)                                      */
#define PORT1_OMR_PS2_Msk                     (0x4UL)                   /*!< PORT1 OMR: PS2 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PS3_Pos                     (3UL)                     /*!< PORT1 OMR: PS3 (Bit 3)                                      */
#define PORT1_OMR_PS3_Msk                     (0x8UL)                   /*!< PORT1 OMR: PS3 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PS4_Pos                     (4UL)                     /*!< PORT1 OMR: PS4 (Bit 4)                                      */
#define PORT1_OMR_PS4_Msk                     (0x10UL)                  /*!< PORT1 OMR: PS4 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PS5_Pos                     (5UL)                     /*!< PORT1 OMR: PS5 (Bit 5)                                      */
#define PORT1_OMR_PS5_Msk                     (0x20UL)                  /*!< PORT1 OMR: PS5 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PS6_Pos                     (6UL)                     /*!< PORT1 OMR: PS6 (Bit 6)                                      */
#define PORT1_OMR_PS6_Msk                     (0x40UL)                  /*!< PORT1 OMR: PS6 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR0_Pos                     (16UL)                    /*!< PORT1 OMR: PR0 (Bit 16)                                     */
#define PORT1_OMR_PR0_Msk                     (0x10000UL)               /*!< PORT1 OMR: PR0 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR1_Pos                     (17UL)                    /*!< PORT1 OMR: PR1 (Bit 17)                                     */
#define PORT1_OMR_PR1_Msk                     (0x20000UL)               /*!< PORT1 OMR: PR1 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR2_Pos                     (18UL)                    /*!< PORT1 OMR: PR2 (Bit 18)                                     */
#define PORT1_OMR_PR2_Msk                     (0x40000UL)               /*!< PORT1 OMR: PR2 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR3_Pos                     (19UL)                    /*!< PORT1 OMR: PR3 (Bit 19)                                     */
#define PORT1_OMR_PR3_Msk                     (0x80000UL)               /*!< PORT1 OMR: PR3 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR4_Pos                     (20UL)                    /*!< PORT1 OMR: PR4 (Bit 20)                                     */
#define PORT1_OMR_PR4_Msk                     (0x100000UL)              /*!< PORT1 OMR: PR4 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR5_Pos                     (21UL)                    /*!< PORT1 OMR: PR5 (Bit 21)                                     */
#define PORT1_OMR_PR5_Msk                     (0x200000UL)              /*!< PORT1 OMR: PR5 (Bitfield-Mask: 0x01)                        */
#define PORT1_OMR_PR6_Pos                     (22UL)                    /*!< PORT1 OMR: PR6 (Bit 22)                                     */
#define PORT1_OMR_PR6_Msk                     (0x400000UL)              /*!< PORT1 OMR: PR6 (Bitfield-Mask: 0x01)                        */

/* ---------------------------------  PORT1_IOCR0  -------------------------------- */
#define PORT1_IOCR0_PC0_Pos                   (3UL)                     /*!< PORT1 IOCR0: PC0 (Bit 3)                                    */
#define PORT1_IOCR0_PC0_Msk                   (0xf8UL)                  /*!< PORT1 IOCR0: PC0 (Bitfield-Mask: 0x1f)                      */
#define PORT1_IOCR0_PC1_Pos                   (11UL)                    /*!< PORT1 IOCR0: PC1 (Bit 11)                                   */
#define PORT1_IOCR0_PC1_Msk                   (0xf800UL)                /*!< PORT1 IOCR0: PC1 (Bitfield-Mask: 0x1f)                      */
#define PORT1_IOCR0_PC2_Pos                   (19UL)                    /*!< PORT1 IOCR0: PC2 (Bit 19)                                   */
#define PORT1_IOCR0_PC2_Msk                   (0xf80000UL)              /*!< PORT1 IOCR0: PC2 (Bitfield-Mask: 0x1f)                      */
#define PORT1_IOCR0_PC3_Pos                   (27UL)                    /*!< PORT1 IOCR0: PC3 (Bit 27)                                   */
#define PORT1_IOCR0_PC3_Msk                   (0xf8000000UL)            /*!< PORT1 IOCR0: PC3 (Bitfield-Mask: 0x1f)                      */

/* ---------------------------------  PORT1_IOCR4  -------------------------------- */
#define PORT1_IOCR4_PC4_Pos                   (3UL)                     /*!< PORT1 IOCR4: PC4 (Bit 3)                                    */
#define PORT1_IOCR4_PC4_Msk                   (0xf8UL)                  /*!< PORT1 IOCR4: PC4 (Bitfield-Mask: 0x1f)                      */
#define PORT1_IOCR4_PC5_Pos                   (11UL)                    /*!< PORT1 IOCR4: PC5 (Bit 11)                                   */
#define PORT1_IOCR4_PC5_Msk                   (0xf800UL)                /*!< PORT1 IOCR4: PC5 (Bitfield-Mask: 0x1f)                      */
#define PORT1_IOCR4_PC6_Pos                   (19UL)                    /*!< PORT1 IOCR4: PC6 (Bit 19)                                   */
#define PORT1_IOCR4_PC6_Msk                   (0xf80000UL)              /*!< PORT1 IOCR4: PC6 (Bitfield-Mask: 0x1f)                      */

/* ----------------------------------  PORT1_IN  ---------------------------------- */
#define PORT1_IN_P0_Pos                       (0UL)                     /*!< PORT1 IN: P0 (Bit 0)                                        */
#define PORT1_IN_P0_Msk                       (0x1UL)                   /*!< PORT1 IN: P0 (Bitfield-Mask: 0x01)                          */
#define PORT1_IN_P1_Pos                       (1UL)                     /*!< PORT1 IN: P1 (Bit 1)                                        */
#define PORT1_IN_P1_Msk                       (0x2UL)                   /*!< PORT1 IN: P1 (Bitfield-Mask: 0x01)                          */
#define PORT1_IN_P2_Pos                       (2UL)                     /*!< PORT1 IN: P2 (Bit 2)                                        */
#define PORT1_IN_P2_Msk                       (0x4UL)                   /*!< PORT1 IN: P2 (Bitfield-Mask: 0x01)                          */
#define PORT1_IN_P3_Pos                       (3UL)                     /*!< PORT1 IN: P3 (Bit 3)                                        */
#define PORT1_IN_P3_Msk                       (0x8UL)                   /*!< PORT1 IN: P3 (Bitfield-Mask: 0x01)                          */
#define PORT1_IN_P4_Pos                       (4UL)                     /*!< PORT1 IN: P4 (Bit 4)                                        */
#define PORT1_IN_P4_Msk                       (0x10UL)                  /*!< PORT1 IN: P4 (Bitfield-Mask: 0x01)                          */
#define PORT1_IN_P5_Pos                       (5UL)                     /*!< PORT1 IN: P5 (Bit 5)                                        */
#define PORT1_IN_P5_Msk                       (0x20UL)                  /*!< PORT1 IN: P5 (Bitfield-Mask: 0x01)                          */
#define PORT1_IN_P6_Pos                       (6UL)                     /*!< PORT1 IN: P6 (Bit 6)                                        */
#define PORT1_IN_P6_Msk                       (0x40UL)                  /*!< PORT1 IN: P6 (Bitfield-Mask: 0x01)                          */

/* ---------------------------------  PORT1_PHCR0  -------------------------------- */
#define PORT1_PHCR0_PH0_Pos                   (2UL)                     /*!< PORT1 PHCR0: PH0 (Bit 2)                                    */
#define PORT1_PHCR0_PH0_Msk                   (0x4UL)                   /*!< PORT1 PHCR0: PH0 (Bitfield-Mask: 0x01)                      */
#define PORT1_PHCR0_PH1_Pos                   (6UL)                     /*!< PORT1 PHCR0: PH1 (Bit 6)                                    */
#define PORT1_PHCR0_PH1_Msk                   (0x40UL)                  /*!< PORT1 PHCR0: PH1 (Bitfield-Mask: 0x01)                      */
#define PORT1_PHCR0_PH2_Pos                   (10UL)                    /*!< PORT1 PHCR0: PH2 (Bit 10)                                   */
#define PORT1_PHCR0_PH2_Msk                   (0x400UL)                 /*!< PORT1 PHCR0: PH2 (Bitfield-Mask: 0x01)                      */
#define PORT1_PHCR0_PH3_Pos                   (14UL)                    /*!< PORT1 PHCR0: PH3 (Bit 14)                                   */
#define PORT1_PHCR0_PH3_Msk                   (0x4000UL)                /*!< PORT1 PHCR0: PH3 (Bitfield-Mask: 0x01)                      */
#define PORT1_PHCR0_PH4_Pos                   (18UL)                    /*!< PORT1 PHCR0: PH4 (Bit 18)                                   */
#define PORT1_PHCR0_PH4_Msk                   (0x40000UL)               /*!< PORT1 PHCR0: PH4 (Bitfield-Mask: 0x01)                      */
#define PORT1_PHCR0_PH5_Pos                   (22UL)                    /*!< PORT1 PHCR0: PH5 (Bit 22)                                   */
#define PORT1_PHCR0_PH5_Msk                   (0x400000UL)              /*!< PORT1 PHCR0: PH5 (Bitfield-Mask: 0x01)                      */
#define PORT1_PHCR0_PH6_Pos                   (26UL)                    /*!< PORT1 PHCR0: PH6 (Bit 26)                                   */
#define PORT1_PHCR0_PH6_Msk                   (0x4000000UL)             /*!< PORT1 PHCR0: PH6 (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  PORT1_PDISC  -------------------------------- */
#define PORT1_PDISC_PDIS0_Pos                 (0UL)                     /*!< PORT1 PDISC: PDIS0 (Bit 0)                                  */
#define PORT1_PDISC_PDIS0_Msk                 (0x1UL)                   /*!< PORT1 PDISC: PDIS0 (Bitfield-Mask: 0x01)                    */
#define PORT1_PDISC_PDIS1_Pos                 (1UL)                     /*!< PORT1 PDISC: PDIS1 (Bit 1)                                  */
#define PORT1_PDISC_PDIS1_Msk                 (0x2UL)                   /*!< PORT1 PDISC: PDIS1 (Bitfield-Mask: 0x01)                    */
#define PORT1_PDISC_PDIS2_Pos                 (2UL)                     /*!< PORT1 PDISC: PDIS2 (Bit 2)                                  */
#define PORT1_PDISC_PDIS2_Msk                 (0x4UL)                   /*!< PORT1 PDISC: PDIS2 (Bitfield-Mask: 0x01)                    */
#define PORT1_PDISC_PDIS3_Pos                 (3UL)                     /*!< PORT1 PDISC: PDIS3 (Bit 3)                                  */
#define PORT1_PDISC_PDIS3_Msk                 (0x8UL)                   /*!< PORT1 PDISC: PDIS3 (Bitfield-Mask: 0x01)                    */
#define PORT1_PDISC_PDIS4_Pos                 (4UL)                     /*!< PORT1 PDISC: PDIS4 (Bit 4)                                  */
#define PORT1_PDISC_PDIS4_Msk                 (0x10UL)                  /*!< PORT1 PDISC: PDIS4 (Bitfield-Mask: 0x01)                    */
#define PORT1_PDISC_PDIS5_Pos                 (5UL)                     /*!< PORT1 PDISC: PDIS5 (Bit 5)                                  */
#define PORT1_PDISC_PDIS5_Msk                 (0x20UL)                  /*!< PORT1 PDISC: PDIS5 (Bitfield-Mask: 0x01)                    */
#define PORT1_PDISC_PDIS6_Pos                 (6UL)                     /*!< PORT1 PDISC: PDIS6 (Bit 6)                                  */
#define PORT1_PDISC_PDIS6_Msk                 (0x40UL)                  /*!< PORT1 PDISC: PDIS6 (Bitfield-Mask: 0x01)                    */

/* ----------------------------------  PORT1_PPS  --------------------------------- */
#define PORT1_PPS_PPS0_Pos                    (0UL)                     /*!< PORT1 PPS: PPS0 (Bit 0)                                     */
#define PORT1_PPS_PPS0_Msk                    (0x1UL)                   /*!< PORT1 PPS: PPS0 (Bitfield-Mask: 0x01)                       */
#define PORT1_PPS_PPS1_Pos                    (1UL)                     /*!< PORT1 PPS: PPS1 (Bit 1)                                     */
#define PORT1_PPS_PPS1_Msk                    (0x2UL)                   /*!< PORT1 PPS: PPS1 (Bitfield-Mask: 0x01)                       */
#define PORT1_PPS_PPS2_Pos                    (2UL)                     /*!< PORT1 PPS: PPS2 (Bit 2)                                     */
#define PORT1_PPS_PPS2_Msk                    (0x4UL)                   /*!< PORT1 PPS: PPS2 (Bitfield-Mask: 0x01)                       */
#define PORT1_PPS_PPS3_Pos                    (3UL)                     /*!< PORT1 PPS: PPS3 (Bit 3)                                     */
#define PORT1_PPS_PPS3_Msk                    (0x8UL)                   /*!< PORT1 PPS: PPS3 (Bitfield-Mask: 0x01)                       */
#define PORT1_PPS_PPS4_Pos                    (4UL)                     /*!< PORT1 PPS: PPS4 (Bit 4)                                     */
#define PORT1_PPS_PPS4_Msk                    (0x10UL)                  /*!< PORT1 PPS: PPS4 (Bitfield-Mask: 0x01)                       */
#define PORT1_PPS_PPS5_Pos                    (5UL)                     /*!< PORT1 PPS: PPS5 (Bit 5)                                     */
#define PORT1_PPS_PPS5_Msk                    (0x20UL)                  /*!< PORT1 PPS: PPS5 (Bitfield-Mask: 0x01)                       */
#define PORT1_PPS_PPS6_Pos                    (6UL)                     /*!< PORT1 PPS: PPS6 (Bit 6)                                     */
#define PORT1_PPS_PPS6_Msk                    (0x40UL)                  /*!< PORT1 PPS: PPS6 (Bitfield-Mask: 0x01)                       */

/* ---------------------------------  PORT1_HWSEL  -------------------------------- */
#define PORT1_HWSEL_HW0_Pos                   (0UL)                     /*!< PORT1 HWSEL: HW0 (Bit 0)                                    */
#define PORT1_HWSEL_HW0_Msk                   (0x3UL)                   /*!< PORT1 HWSEL: HW0 (Bitfield-Mask: 0x03)                      */
#define PORT1_HWSEL_HW1_Pos                   (2UL)                     /*!< PORT1 HWSEL: HW1 (Bit 2)                                    */
#define PORT1_HWSEL_HW1_Msk                   (0xcUL)                   /*!< PORT1 HWSEL: HW1 (Bitfield-Mask: 0x03)                      */
#define PORT1_HWSEL_HW2_Pos                   (4UL)                     /*!< PORT1 HWSEL: HW2 (Bit 4)                                    */
#define PORT1_HWSEL_HW2_Msk                   (0x30UL)                  /*!< PORT1 HWSEL: HW2 (Bitfield-Mask: 0x03)                      */
#define PORT1_HWSEL_HW3_Pos                   (6UL)                     /*!< PORT1 HWSEL: HW3 (Bit 6)                                    */
#define PORT1_HWSEL_HW3_Msk                   (0xc0UL)                  /*!< PORT1 HWSEL: HW3 (Bitfield-Mask: 0x03)                      */
#define PORT1_HWSEL_HW4_Pos                   (8UL)                     /*!< PORT1 HWSEL: HW4 (Bit 8)                                    */
#define PORT1_HWSEL_HW4_Msk                   (0x300UL)                 /*!< PORT1 HWSEL: HW4 (Bitfield-Mask: 0x03)                      */
#define PORT1_HWSEL_HW5_Pos                   (10UL)                    /*!< PORT1 HWSEL: HW5 (Bit 10)                                   */
#define PORT1_HWSEL_HW5_Msk                   (0xc00UL)                 /*!< PORT1 HWSEL: HW5 (Bitfield-Mask: 0x03)                      */
#define PORT1_HWSEL_HW6_Pos                   (12UL)                    /*!< PORT1 HWSEL: HW6 (Bit 12)                                   */
#define PORT1_HWSEL_HW6_Msk                   (0x3000UL)                /*!< PORT1 HWSEL: HW6 (Bitfield-Mask: 0x03)                      */


/* ================================================================================ */
/* ================         struct 'PORT2' Position & Mask         ================ */
/* ================================================================================ */


/* ----------------------------------  PORT2_OUT  --------------------------------- */
#define PORT2_OUT_P0_Pos                      (0UL)                     /*!< PORT2 OUT: P0 (Bit 0)                                       */
#define PORT2_OUT_P0_Msk                      (0x1UL)                   /*!< PORT2 OUT: P0 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P1_Pos                      (1UL)                     /*!< PORT2 OUT: P1 (Bit 1)                                       */
#define PORT2_OUT_P1_Msk                      (0x2UL)                   /*!< PORT2 OUT: P1 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P2_Pos                      (2UL)                     /*!< PORT2 OUT: P2 (Bit 2)                                       */
#define PORT2_OUT_P2_Msk                      (0x4UL)                   /*!< PORT2 OUT: P2 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P3_Pos                      (3UL)                     /*!< PORT2 OUT: P3 (Bit 3)                                       */
#define PORT2_OUT_P3_Msk                      (0x8UL)                   /*!< PORT2 OUT: P3 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P4_Pos                      (4UL)                     /*!< PORT2 OUT: P4 (Bit 4)                                       */
#define PORT2_OUT_P4_Msk                      (0x10UL)                  /*!< PORT2 OUT: P4 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P5_Pos                      (5UL)                     /*!< PORT2 OUT: P5 (Bit 5)                                       */
#define PORT2_OUT_P5_Msk                      (0x20UL)                  /*!< PORT2 OUT: P5 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P6_Pos                      (6UL)                     /*!< PORT2 OUT: P6 (Bit 6)                                       */
#define PORT2_OUT_P6_Msk                      (0x40UL)                  /*!< PORT2 OUT: P6 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P7_Pos                      (7UL)                     /*!< PORT2 OUT: P7 (Bit 7)                                       */
#define PORT2_OUT_P7_Msk                      (0x80UL)                  /*!< PORT2 OUT: P7 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P8_Pos                      (8UL)                     /*!< PORT2 OUT: P8 (Bit 8)                                       */
#define PORT2_OUT_P8_Msk                      (0x100UL)                 /*!< PORT2 OUT: P8 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P9_Pos                      (9UL)                     /*!< PORT2 OUT: P9 (Bit 9)                                       */
#define PORT2_OUT_P9_Msk                      (0x200UL)                 /*!< PORT2 OUT: P9 (Bitfield-Mask: 0x01)                         */
#define PORT2_OUT_P10_Pos                     (10UL)                    /*!< PORT2 OUT: P10 (Bit 10)                                     */
#define PORT2_OUT_P10_Msk                     (0x400UL)                 /*!< PORT2 OUT: P10 (Bitfield-Mask: 0x01)                        */
#define PORT2_OUT_P11_Pos                     (11UL)                    /*!< PORT2 OUT: P11 (Bit 11)                                     */
#define PORT2_OUT_P11_Msk                     (0x800UL)                 /*!< PORT2 OUT: P11 (Bitfield-Mask: 0x01)                        */

/* ----------------------------------  PORT2_OMR  --------------------------------- */
#define PORT2_OMR_PS0_Pos                     (0UL)                     /*!< PORT2 OMR: PS0 (Bit 0)                                      */
#define PORT2_OMR_PS0_Msk                     (0x1UL)                   /*!< PORT2 OMR: PS0 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS1_Pos                     (1UL)                     /*!< PORT2 OMR: PS1 (Bit 1)                                      */
#define PORT2_OMR_PS1_Msk                     (0x2UL)                   /*!< PORT2 OMR: PS1 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS2_Pos                     (2UL)                     /*!< PORT2 OMR: PS2 (Bit 2)                                      */
#define PORT2_OMR_PS2_Msk                     (0x4UL)                   /*!< PORT2 OMR: PS2 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS3_Pos                     (3UL)                     /*!< PORT2 OMR: PS3 (Bit 3)                                      */
#define PORT2_OMR_PS3_Msk                     (0x8UL)                   /*!< PORT2 OMR: PS3 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS4_Pos                     (4UL)                     /*!< PORT2 OMR: PS4 (Bit 4)                                      */
#define PORT2_OMR_PS4_Msk                     (0x10UL)                  /*!< PORT2 OMR: PS4 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS5_Pos                     (5UL)                     /*!< PORT2 OMR: PS5 (Bit 5)                                      */
#define PORT2_OMR_PS5_Msk                     (0x20UL)                  /*!< PORT2 OMR: PS5 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS6_Pos                     (6UL)                     /*!< PORT2 OMR: PS6 (Bit 6)                                      */
#define PORT2_OMR_PS6_Msk                     (0x40UL)                  /*!< PORT2 OMR: PS6 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS7_Pos                     (7UL)                     /*!< PORT2 OMR: PS7 (Bit 7)                                      */
#define PORT2_OMR_PS7_Msk                     (0x80UL)                  /*!< PORT2 OMR: PS7 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS8_Pos                     (8UL)                     /*!< PORT2 OMR: PS8 (Bit 8)                                      */
#define PORT2_OMR_PS8_Msk                     (0x100UL)                 /*!< PORT2 OMR: PS8 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS9_Pos                     (9UL)                     /*!< PORT2 OMR: PS9 (Bit 9)                                      */
#define PORT2_OMR_PS9_Msk                     (0x200UL)                 /*!< PORT2 OMR: PS9 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PS10_Pos                    (10UL)                    /*!< PORT2 OMR: PS10 (Bit 10)                                    */
#define PORT2_OMR_PS10_Msk                    (0x400UL)                 /*!< PORT2 OMR: PS10 (Bitfield-Mask: 0x01)                       */
#define PORT2_OMR_PS11_Pos                    (11UL)                    /*!< PORT2 OMR: PS11 (Bit 11)                                    */
#define PORT2_OMR_PS11_Msk                    (0x800UL)                 /*!< PORT2 OMR: PS11 (Bitfield-Mask: 0x01)                       */
#define PORT2_OMR_PR0_Pos                     (16UL)                    /*!< PORT2 OMR: PR0 (Bit 16)                                     */
#define PORT2_OMR_PR0_Msk                     (0x10000UL)               /*!< PORT2 OMR: PR0 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR1_Pos                     (17UL)                    /*!< PORT2 OMR: PR1 (Bit 17)                                     */
#define PORT2_OMR_PR1_Msk                     (0x20000UL)               /*!< PORT2 OMR: PR1 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR2_Pos                     (18UL)                    /*!< PORT2 OMR: PR2 (Bit 18)                                     */
#define PORT2_OMR_PR2_Msk                     (0x40000UL)               /*!< PORT2 OMR: PR2 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR3_Pos                     (19UL)                    /*!< PORT2 OMR: PR3 (Bit 19)                                     */
#define PORT2_OMR_PR3_Msk                     (0x80000UL)               /*!< PORT2 OMR: PR3 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR4_Pos                     (20UL)                    /*!< PORT2 OMR: PR4 (Bit 20)                                     */
#define PORT2_OMR_PR4_Msk                     (0x100000UL)              /*!< PORT2 OMR: PR4 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR5_Pos                     (21UL)                    /*!< PORT2 OMR: PR5 (Bit 21)                                     */
#define PORT2_OMR_PR5_Msk                     (0x200000UL)              /*!< PORT2 OMR: PR5 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR6_Pos                     (22UL)                    /*!< PORT2 OMR: PR6 (Bit 22)                                     */
#define PORT2_OMR_PR6_Msk                     (0x400000UL)              /*!< PORT2 OMR: PR6 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR7_Pos                     (23UL)                    /*!< PORT2 OMR: PR7 (Bit 23)                                     */
#define PORT2_OMR_PR7_Msk                     (0x800000UL)              /*!< PORT2 OMR: PR7 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR8_Pos                     (24UL)                    /*!< PORT2 OMR: PR8 (Bit 24)                                     */
#define PORT2_OMR_PR8_Msk                     (0x1000000UL)             /*!< PORT2 OMR: PR8 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR9_Pos                     (25UL)                    /*!< PORT2 OMR: PR9 (Bit 25)                                     */
#define PORT2_OMR_PR9_Msk                     (0x2000000UL)             /*!< PORT2 OMR: PR9 (Bitfield-Mask: 0x01)                        */
#define PORT2_OMR_PR10_Pos                    (26UL)                    /*!< PORT2 OMR: PR10 (Bit 26)                                    */
#define PORT2_OMR_PR10_Msk                    (0x4000000UL)             /*!< PORT2 OMR: PR10 (Bitfield-Mask: 0x01)                       */
#define PORT2_OMR_PR11_Pos                    (27UL)                    /*!< PORT2 OMR: PR11 (Bit 27)                                    */
#define PORT2_OMR_PR11_Msk                    (0x8000000UL)             /*!< PORT2 OMR: PR11 (Bitfield-Mask: 0x01)                       */

/* ---------------------------------  PORT2_IOCR0  -------------------------------- */
#define PORT2_IOCR0_PC0_Pos                   (3UL)                     /*!< PORT2 IOCR0: PC0 (Bit 3)                                    */
#define PORT2_IOCR0_PC0_Msk                   (0xf8UL)                  /*!< PORT2 IOCR0: PC0 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR0_PC1_Pos                   (11UL)                    /*!< PORT2 IOCR0: PC1 (Bit 11)                                   */
#define PORT2_IOCR0_PC1_Msk                   (0xf800UL)                /*!< PORT2 IOCR0: PC1 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR0_PC2_Pos                   (19UL)                    /*!< PORT2 IOCR0: PC2 (Bit 19)                                   */
#define PORT2_IOCR0_PC2_Msk                   (0xf80000UL)              /*!< PORT2 IOCR0: PC2 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR0_PC3_Pos                   (27UL)                    /*!< PORT2 IOCR0: PC3 (Bit 27)                                   */
#define PORT2_IOCR0_PC3_Msk                   (0xf8000000UL)            /*!< PORT2 IOCR0: PC3 (Bitfield-Mask: 0x1f)                      */

/* ---------------------------------  PORT2_IOCR4  -------------------------------- */
#define PORT2_IOCR4_PC4_Pos                   (3UL)                     /*!< PORT2 IOCR4: PC4 (Bit 3)                                    */
#define PORT2_IOCR4_PC4_Msk                   (0xf8UL)                  /*!< PORT2 IOCR4: PC4 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR4_PC5_Pos                   (11UL)                    /*!< PORT2 IOCR4: PC5 (Bit 11)                                   */
#define PORT2_IOCR4_PC5_Msk                   (0xf800UL)                /*!< PORT2 IOCR4: PC5 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR4_PC6_Pos                   (19UL)                    /*!< PORT2 IOCR4: PC6 (Bit 19)                                   */
#define PORT2_IOCR4_PC6_Msk                   (0xf80000UL)              /*!< PORT2 IOCR4: PC6 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR4_PC7_Pos                   (27UL)                    /*!< PORT2 IOCR4: PC7 (Bit 27)                                   */
#define PORT2_IOCR4_PC7_Msk                   (0xf8000000UL)            /*!< PORT2 IOCR4: PC7 (Bitfield-Mask: 0x1f)                      */

/* ---------------------------------  PORT2_IOCR8  -------------------------------- */
#define PORT2_IOCR8_PC8_Pos                   (3UL)                     /*!< PORT2 IOCR8: PC8 (Bit 3)                                    */
#define PORT2_IOCR8_PC8_Msk                   (0xf8UL)                  /*!< PORT2 IOCR8: PC8 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR8_PC9_Pos                   (11UL)                    /*!< PORT2 IOCR8: PC9 (Bit 11)                                   */
#define PORT2_IOCR8_PC9_Msk                   (0xf800UL)                /*!< PORT2 IOCR8: PC9 (Bitfield-Mask: 0x1f)                      */
#define PORT2_IOCR8_PC10_Pos                  (19UL)                    /*!< PORT2 IOCR8: PC10 (Bit 19)                                  */
#define PORT2_IOCR8_PC10_Msk                  (0xf80000UL)              /*!< PORT2 IOCR8: PC10 (Bitfield-Mask: 0x1f)                     */
#define PORT2_IOCR8_PC11_Pos                  (27UL)                    /*!< PORT2 IOCR8: PC11 (Bit 27)                                  */
#define PORT2_IOCR8_PC11_Msk                  (0xf8000000UL)            /*!< PORT2 IOCR8: PC11 (Bitfield-Mask: 0x1f)                     */

/* ----------------------------------  PORT2_IN  ---------------------------------- */
#define PORT2_IN_P0_Pos                       (0UL)                     /*!< PORT2 IN: P0 (Bit 0)                                        */
#define PORT2_IN_P0_Msk                       (0x1UL)                   /*!< PORT2 IN: P0 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P1_Pos                       (1UL)                     /*!< PORT2 IN: P1 (Bit 1)                                        */
#define PORT2_IN_P1_Msk                       (0x2UL)                   /*!< PORT2 IN: P1 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P2_Pos                       (2UL)                     /*!< PORT2 IN: P2 (Bit 2)                                        */
#define PORT2_IN_P2_Msk                       (0x4UL)                   /*!< PORT2 IN: P2 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P3_Pos                       (3UL)                     /*!< PORT2 IN: P3 (Bit 3)                                        */
#define PORT2_IN_P3_Msk                       (0x8UL)                   /*!< PORT2 IN: P3 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P4_Pos                       (4UL)                     /*!< PORT2 IN: P4 (Bit 4)                                        */
#define PORT2_IN_P4_Msk                       (0x10UL)                  /*!< PORT2 IN: P4 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P5_Pos                       (5UL)                     /*!< PORT2 IN: P5 (Bit 5)                                        */
#define PORT2_IN_P5_Msk                       (0x20UL)                  /*!< PORT2 IN: P5 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P6_Pos                       (6UL)                     /*!< PORT2 IN: P6 (Bit 6)                                        */
#define PORT2_IN_P6_Msk                       (0x40UL)                  /*!< PORT2 IN: P6 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P7_Pos                       (7UL)                     /*!< PORT2 IN: P7 (Bit 7)                                        */
#define PORT2_IN_P7_Msk                       (0x80UL)                  /*!< PORT2 IN: P7 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P8_Pos                       (8UL)                     /*!< PORT2 IN: P8 (Bit 8)                                        */
#define PORT2_IN_P8_Msk                       (0x100UL)                 /*!< PORT2 IN: P8 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P9_Pos                       (9UL)                     /*!< PORT2 IN: P9 (Bit 9)                                        */
#define PORT2_IN_P9_Msk                       (0x200UL)                 /*!< PORT2 IN: P9 (Bitfield-Mask: 0x01)                          */
#define PORT2_IN_P10_Pos                      (10UL)                    /*!< PORT2 IN: P10 (Bit 10)                                      */
#define PORT2_IN_P10_Msk                      (0x400UL)                 /*!< PORT2 IN: P10 (Bitfield-Mask: 0x01)                         */
#define PORT2_IN_P11_Pos                      (11UL)                    /*!< PORT2 IN: P11 (Bit 11)                                      */
#define PORT2_IN_P11_Msk                      (0x800UL)                 /*!< PORT2 IN: P11 (Bitfield-Mask: 0x01)                         */

/* ---------------------------------  PORT2_PHCR0  -------------------------------- */
#define PORT2_PHCR0_PH0_Pos                   (2UL)                     /*!< PORT2 PHCR0: PH0 (Bit 2)                                    */
#define PORT2_PHCR0_PH0_Msk                   (0x4UL)                   /*!< PORT2 PHCR0: PH0 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH1_Pos                   (6UL)                     /*!< PORT2 PHCR0: PH1 (Bit 6)                                    */
#define PORT2_PHCR0_PH1_Msk                   (0x40UL)                  /*!< PORT2 PHCR0: PH1 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH2_Pos                   (10UL)                    /*!< PORT2 PHCR0: PH2 (Bit 10)                                   */
#define PORT2_PHCR0_PH2_Msk                   (0x400UL)                 /*!< PORT2 PHCR0: PH2 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH3_Pos                   (14UL)                    /*!< PORT2 PHCR0: PH3 (Bit 14)                                   */
#define PORT2_PHCR0_PH3_Msk                   (0x4000UL)                /*!< PORT2 PHCR0: PH3 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH4_Pos                   (18UL)                    /*!< PORT2 PHCR0: PH4 (Bit 18)                                   */
#define PORT2_PHCR0_PH4_Msk                   (0x40000UL)               /*!< PORT2 PHCR0: PH4 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH5_Pos                   (22UL)                    /*!< PORT2 PHCR0: PH5 (Bit 22)                                   */
#define PORT2_PHCR0_PH5_Msk                   (0x400000UL)              /*!< PORT2 PHCR0: PH5 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH6_Pos                   (26UL)                    /*!< PORT2 PHCR0: PH6 (Bit 26)                                   */
#define PORT2_PHCR0_PH6_Msk                   (0x4000000UL)             /*!< PORT2 PHCR0: PH6 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR0_PH7_Pos                   (30UL)                    /*!< PORT2 PHCR0: PH7 (Bit 30)                                   */
#define PORT2_PHCR0_PH7_Msk                   (0x40000000UL)            /*!< PORT2 PHCR0: PH7 (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  PORT2_PHCR1  -------------------------------- */
#define PORT2_PHCR1_PH8_Pos                   (2UL)                     /*!< PORT2 PHCR1: PH8 (Bit 2)                                    */
#define PORT2_PHCR1_PH8_Msk                   (0x4UL)                   /*!< PORT2 PHCR1: PH8 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR1_PH9_Pos                   (6UL)                     /*!< PORT2 PHCR1: PH9 (Bit 6)                                    */
#define PORT2_PHCR1_PH9_Msk                   (0x40UL)                  /*!< PORT2 PHCR1: PH9 (Bitfield-Mask: 0x01)                      */
#define PORT2_PHCR1_PH10_Pos                  (10UL)                    /*!< PORT2 PHCR1: PH10 (Bit 10)                                  */
#define PORT2_PHCR1_PH10_Msk                  (0x400UL)                 /*!< PORT2 PHCR1: PH10 (Bitfield-Mask: 0x01)                     */
#define PORT2_PHCR1_PH11_Pos                  (14UL)                    /*!< PORT2 PHCR1: PH11 (Bit 14)                                  */
#define PORT2_PHCR1_PH11_Msk                  (0x4000UL)                /*!< PORT2 PHCR1: PH11 (Bitfield-Mask: 0x01)                     */

/* ---------------------------------  PORT2_PDISC  -------------------------------- */
#define PORT2_PDISC_PDIS0_Pos                 (0UL)                     /*!< PORT2 PDISC: PDIS0 (Bit 0)                                  */
#define PORT2_PDISC_PDIS0_Msk                 (0x1UL)                   /*!< PORT2 PDISC: PDIS0 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS1_Pos                 (1UL)                     /*!< PORT2 PDISC: PDIS1 (Bit 1)                                  */
#define PORT2_PDISC_PDIS1_Msk                 (0x2UL)                   /*!< PORT2 PDISC: PDIS1 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS2_Pos                 (2UL)                     /*!< PORT2 PDISC: PDIS2 (Bit 2)                                  */
#define PORT2_PDISC_PDIS2_Msk                 (0x4UL)                   /*!< PORT2 PDISC: PDIS2 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS3_Pos                 (3UL)                     /*!< PORT2 PDISC: PDIS3 (Bit 3)                                  */
#define PORT2_PDISC_PDIS3_Msk                 (0x8UL)                   /*!< PORT2 PDISC: PDIS3 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS4_Pos                 (4UL)                     /*!< PORT2 PDISC: PDIS4 (Bit 4)                                  */
#define PORT2_PDISC_PDIS4_Msk                 (0x10UL)                  /*!< PORT2 PDISC: PDIS4 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS5_Pos                 (5UL)                     /*!< PORT2 PDISC: PDIS5 (Bit 5)                                  */
#define PORT2_PDISC_PDIS5_Msk                 (0x20UL)                  /*!< PORT2 PDISC: PDIS5 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS6_Pos                 (6UL)                     /*!< PORT2 PDISC: PDIS6 (Bit 6)                                  */
#define PORT2_PDISC_PDIS6_Msk                 (0x40UL)                  /*!< PORT2 PDISC: PDIS6 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS7_Pos                 (7UL)                     /*!< PORT2 PDISC: PDIS7 (Bit 7)                                  */
#define PORT2_PDISC_PDIS7_Msk                 (0x80UL)                  /*!< PORT2 PDISC: PDIS7 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS8_Pos                 (8UL)                     /*!< PORT2 PDISC: PDIS8 (Bit 8)                                  */
#define PORT2_PDISC_PDIS8_Msk                 (0x100UL)                 /*!< PORT2 PDISC: PDIS8 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS9_Pos                 (9UL)                     /*!< PORT2 PDISC: PDIS9 (Bit 9)                                  */
#define PORT2_PDISC_PDIS9_Msk                 (0x200UL)                 /*!< PORT2 PDISC: PDIS9 (Bitfield-Mask: 0x01)                    */
#define PORT2_PDISC_PDIS10_Pos                (10UL)                    /*!< PORT2 PDISC: PDIS10 (Bit 10)                                */
#define PORT2_PDISC_PDIS10_Msk                (0x400UL)                 /*!< PORT2 PDISC: PDIS10 (Bitfield-Mask: 0x01)                   */
#define PORT2_PDISC_PDIS11_Pos                (11UL)                    /*!< PORT2 PDISC: PDIS11 (Bit 11)                                */
#define PORT2_PDISC_PDIS11_Msk                (0x800UL)                 /*!< PORT2 PDISC: PDIS11 (Bitfield-Mask: 0x01)                   */

/* ----------------------------------  PORT2_PPS  --------------------------------- */
#define PORT2_PPS_PPS0_Pos                    (0UL)                     /*!< PORT2 PPS: PPS0 (Bit 0)                                     */
#define PORT2_PPS_PPS0_Msk                    (0x1UL)                   /*!< PORT2 PPS: PPS0 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS1_Pos                    (1UL)                     /*!< PORT2 PPS: PPS1 (Bit 1)                                     */
#define PORT2_PPS_PPS1_Msk                    (0x2UL)                   /*!< PORT2 PPS: PPS1 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS2_Pos                    (2UL)                     /*!< PORT2 PPS: PPS2 (Bit 2)                                     */
#define PORT2_PPS_PPS2_Msk                    (0x4UL)                   /*!< PORT2 PPS: PPS2 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS3_Pos                    (3UL)                     /*!< PORT2 PPS: PPS3 (Bit 3)                                     */
#define PORT2_PPS_PPS3_Msk                    (0x8UL)                   /*!< PORT2 PPS: PPS3 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS4_Pos                    (4UL)                     /*!< PORT2 PPS: PPS4 (Bit 4)                                     */
#define PORT2_PPS_PPS4_Msk                    (0x10UL)                  /*!< PORT2 PPS: PPS4 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS5_Pos                    (5UL)                     /*!< PORT2 PPS: PPS5 (Bit 5)                                     */
#define PORT2_PPS_PPS5_Msk                    (0x20UL)                  /*!< PORT2 PPS: PPS5 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS6_Pos                    (6UL)                     /*!< PORT2 PPS: PPS6 (Bit 6)                                     */
#define PORT2_PPS_PPS6_Msk                    (0x40UL)                  /*!< PORT2 PPS: PPS6 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS7_Pos                    (7UL)                     /*!< PORT2 PPS: PPS7 (Bit 7)                                     */
#define PORT2_PPS_PPS7_Msk                    (0x80UL)                  /*!< PORT2 PPS: PPS7 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS8_Pos                    (8UL)                     /*!< PORT2 PPS: PPS8 (Bit 8)                                     */
#define PORT2_PPS_PPS8_Msk                    (0x100UL)                 /*!< PORT2 PPS: PPS8 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS9_Pos                    (9UL)                     /*!< PORT2 PPS: PPS9 (Bit 9)                                     */
#define PORT2_PPS_PPS9_Msk                    (0x200UL)                 /*!< PORT2 PPS: PPS9 (Bitfield-Mask: 0x01)                       */
#define PORT2_PPS_PPS10_Pos                   (10UL)                    /*!< PORT2 PPS: PPS10 (Bit 10)                                   */
#define PORT2_PPS_PPS10_Msk                   (0x400UL)                 /*!< PORT2 PPS: PPS10 (Bitfield-Mask: 0x01)                      */
#define PORT2_PPS_PPS11_Pos                   (11UL)                    /*!< PORT2 PPS: PPS11 (Bit 11)                                   */
#define PORT2_PPS_PPS11_Msk                   (0x800UL)                 /*!< PORT2 PPS: PPS11 (Bitfield-Mask: 0x01)                      */

/* ---------------------------------  PORT2_HWSEL  -------------------------------- */
#define PORT2_HWSEL_HW0_Pos                   (0UL)                     /*!< PORT2 HWSEL: HW0 (Bit 0)                                    */
#define PORT2_HWSEL_HW0_Msk                   (0x3UL)                   /*!< PORT2 HWSEL: HW0 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW1_Pos                   (2UL)                     /*!< PORT2 HWSEL: HW1 (Bit 2)                                    */
#define PORT2_HWSEL_HW1_Msk                   (0xcUL)                   /*!< PORT2 HWSEL: HW1 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW2_Pos                   (4UL)                     /*!< PORT2 HWSEL: HW2 (Bit 4)                                    */
#define PORT2_HWSEL_HW2_Msk                   (0x30UL)                  /*!< PORT2 HWSEL: HW2 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW3_Pos                   (6UL)                     /*!< PORT2 HWSEL: HW3 (Bit 6)                                    */
#define PORT2_HWSEL_HW3_Msk                   (0xc0UL)                  /*!< PORT2 HWSEL: HW3 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW4_Pos                   (8UL)                     /*!< PORT2 HWSEL: HW4 (Bit 8)                                    */
#define PORT2_HWSEL_HW4_Msk                   (0x300UL)                 /*!< PORT2 HWSEL: HW4 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW5_Pos                   (10UL)                    /*!< PORT2 HWSEL: HW5 (Bit 10)                                   */
#define PORT2_HWSEL_HW5_Msk                   (0xc00UL)                 /*!< PORT2 HWSEL: HW5 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW6_Pos                   (12UL)                    /*!< PORT2 HWSEL: HW6 (Bit 12)                                   */
#define PORT2_HWSEL_HW6_Msk                   (0x3000UL)                /*!< PORT2 HWSEL: HW6 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW7_Pos                   (14UL)                    /*!< PORT2 HWSEL: HW7 (Bit 14)                                   */
#define PORT2_HWSEL_HW7_Msk                   (0xc000UL)                /*!< PORT2 HWSEL: HW7 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW8_Pos                   (16UL)                    /*!< PORT2 HWSEL: HW8 (Bit 16)                                   */
#define PORT2_HWSEL_HW8_Msk                   (0x30000UL)               /*!< PORT2 HWSEL: HW8 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW9_Pos                   (18UL)                    /*!< PORT2 HWSEL: HW9 (Bit 18)                                   */
#define PORT2_HWSEL_HW9_Msk                   (0xc0000UL)               /*!< PORT2 HWSEL: HW9 (Bitfield-Mask: 0x03)                      */
#define PORT2_HWSEL_HW10_Pos                  (20UL)                    /*!< PORT2 HWSEL: HW10 (Bit 20)                                  */
#define PORT2_HWSEL_HW10_Msk                  (0x300000UL)              /*!< PORT2 HWSEL: HW10 (Bitfield-Mask: 0x03)                     */
#define PORT2_HWSEL_HW11_Pos                  (22UL)                    /*!< PORT2 HWSEL: HW11 (Bit 22)                                  */
#define PORT2_HWSEL_HW11_Msk                  (0xc00000UL)              /*!< PORT2 HWSEL: HW11 (Bitfield-Mask: 0x03)                     */



/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define PPB_BASE                        0xE000E000UL
#define ERU0_BASE                       0x40010600UL
#define PAU_BASE                        0x40000000UL
#define NVM_BASE                        0x40050000UL
#define WDT_BASE                        0x40020000UL
#define RTC_BASE                        0x40010A00UL
#define PRNG_BASE                       0x48020000UL
#define USIC0_BASE                      0x48000008UL
#define USIC0_CH0_BASE                  0x48000000UL
#define USIC0_CH1_BASE                  0x48000200UL
#define SCU_GENERAL_BASE                0x40010000UL
#define SCU_INTERRUPT_BASE              0x40010038UL
#define SCU_POWER_BASE                  0x40010200UL
#define SCU_CLK_BASE                    0x40010300UL
#define SCU_RESET_BASE                  0x40010400UL
#define SCU_ANALOG_BASE                 0x40011000UL
#define CCU40_BASE                      0x48040000UL
#define CCU40_CC40_BASE                 0x48040100UL
#define CCU40_CC41_BASE                 0x48040200UL
#define CCU40_CC42_BASE                 0x48040300UL
#define CCU40_CC43_BASE                 0x48040400UL
#define VADC_BASE                       0x48030000UL
#define SHS0_BASE                       0x48034000UL
#define PORT0_BASE                      0x40040000UL
#define PORT1_BASE                      0x40040100UL
#define PORT2_BASE                      0x40040200UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define PPB                             ((PPB_Type                *) PPB_BASE)
#define ERU0                            ((ERU_GLOBAL_TypeDef                *) ERU0_BASE)
#define PAU                             ((PAU_Type                *) PAU_BASE)
#define NVM                             ((NVM_Type                *) NVM_BASE)
#define WDT                             ((WDT_GLOBAL_TypeDef                *) WDT_BASE)
#define RTC                             ((RTC_GLOBAL_TypeDef                *) RTC_BASE)
#define PRNG                            ((PRNG_Type               *) PRNG_BASE)
#define USIC0                           ((USIC_GLOBAL_TypeDef               *) USIC0_BASE)
#define USIC0_CH0                       ((USIC_CH_TypeDef            *) USIC0_CH0_BASE)
#define USIC0_CH1                       ((USIC_CH_TypeDef            *) USIC0_CH1_BASE)
#define SCU_GENERAL                     ((SCU_GENERAL_Type        *) SCU_GENERAL_BASE)
#define SCU_INTERRUPT                   ((SCU_INTERRUPT_TypeDef      *) SCU_INTERRUPT_BASE)
#define SCU_POWER                       ((SCU_POWER_Type          *) SCU_POWER_BASE)
#define SCU_CLK                         ((SCU_CLK_TypeDef            *) SCU_CLK_BASE)
#define SCU_RESET                       ((SCU_RESET_Type          *) SCU_RESET_BASE)
#define SCU_ANALOG                      ((SCU_ANALOG_Type         *) SCU_ANALOG_BASE)
#define CCU40                           ((CCU4_GLOBAL_TypeDef               *) CCU40_BASE)
#define CCU40_CC40                      ((CCU4_CC4_TypeDef           *) CCU40_CC40_BASE)
#define CCU40_CC41                      ((CCU4_CC4_TypeDef           *) CCU40_CC41_BASE)
#define CCU40_CC42                      ((CCU4_CC4_TypeDef           *) CCU40_CC42_BASE)
#define CCU40_CC43                      ((CCU4_CC4_TypeDef           *) CCU40_CC43_BASE)
#define VADC                            ((VADC_GLOBAL_TypeDef               *) VADC_BASE)
#define SHS0                            ((SHS_Type                *) SHS0_BASE)
#define PORT0                           ((PORT0_Type              *) PORT0_BASE)
#define PORT1                           ((PORT1_Type              *) PORT1_BASE)
#define PORT2                           ((PORT2_Type              *) PORT2_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group XMC1100 */
/** @} */ /* End of group Infineon */

#ifdef __cplusplus
}
#endif


#endif  /* XMC1100_H */
